/*
    buggy parent : 441ed41
    commit id : d6ed263fc7458e97ad032837ffdab6013d039543
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

static const char http_push_usage[] =
"git-http-push [--all] [--dry-run] [--force] [--verbose] <remote> [<head>...]\n";

#ifndef XML_STATUS_OK
enum XML_Status {
  XML_STATUS_OK = 1,
  XML_STATUS_ERROR = 0
};
#define XML_STATUS_OK    1
#define XML_STATUS_ERROR 0
#endif

#define PREV_BUF_SIZE 4096
#define RANGE_HEADER_SIZE 30

/* DAV methods */
#define DAV_LOCK "LOCK"
#define DAV_MKCOL "MKCOL"
#define DAV_MOVE "MOVE"
#define DAV_PROPFIND "PROPFIND"
#define DAV_PUT "PUT"
#define DAV_UNLOCK "UNLOCK"
#define DAV_DELETE "DELETE"

/* DAV lock flags */
#define DAV_PROP_LOCKWR (1u << 0)
#define DAV_PROP_LOCKEX (1u << 1)
#define DAV_LOCK_OK (1u << 2)

/* DAV XML properties */
#define DAV_CTX_LOCKENTRY ".multistatus.response.propstat.prop.supportedlock.lockentry"
#define DAV_CTX_LOCKTYPE_WRITE ".multistatus.response.propstat.prop.supportedlock.lockentry.locktype.write"
#define DAV_CTX_LOCKTYPE_EXCLUSIVE ".multistatus.response.propstat.prop.supportedlock.lockentry.lockscope.exclusive"
#define DAV_ACTIVELOCK_OWNER ".prop.lockdiscovery.activelock.owner.href"
#define DAV_ACTIVELOCK_TIMEOUT ".prop.lockdiscovery.activelock.timeout"
#define DAV_ACTIVELOCK_TOKEN ".prop.lockdiscovery.activelock.locktoken.href"
#define DAV_PROPFIND_RESP ".multistatus.response"
#define DAV_PROPFIND_NAME ".multistatus.response.href"
#define DAV_PROPFIND_COLLECTION ".multistatus.response.propstat.prop.resourcetype.collection"

/* DAV request body templates */
#define PROPFIND_SUPPORTEDLOCK_REQUEST "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<D:propfind xmlns:D=\"DAV:\">\n<D:prop xmlns:R=\"%s\">\n<D:supportedlock/>\n</D:prop>\n</D:propfind>"
#define PROPFIND_ALL_REQUEST "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<D:propfind xmlns:D=\"DAV:\">\n<D:allprop/>\n</D:propfind>"
#define LOCK_REQUEST "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<D:lockinfo xmlns:D=\"DAV:\">\n<D:lockscope><D:exclusive/></D:lockscope>\n<D:locktype><D:write/></D:locktype>\n<D:owner>\n<D:href>mailto:%s</D:href>\n</D:owner>\n</D:lockinfo>"

#define LOCK_TIME 600
#define LOCK_REFRESH 30

/* bits #0-15 in revision.h */

#define LOCAL    (1u<<16)
#define REMOTE   (1u<<17)
#define FETCHING (1u<<18)
#define PUSHING  (1u<<19)

/* We allow "recursive" symbolic refs. Only within reason, though */
#define MAXDEPTH 5

enum match_refs_flags {
	MATCH_REFS_NONE		= 0,
	MATCH_REFS_ALL 		= (1 << 0),
	MATCH_REFS_MIRROR	= (1 << 1),
};


static int pushing;
static int aborted;
static signed char remote_dir_exists[256];

static struct curl_slist *no_pragma_header;

static int push_verbosely;
static int push_all = MATCH_REFS_NONE;
static int force_all;
static int dry_run;

static struct object_list *objects;

struct repo
{
	char *url;
	int path_len;
	int has_info_refs;
	int can_update_info_refs;
	int has_info_packs;
	struct packed_git *packs;
	struct remote_lock *locks;
};

static struct repo *remote;

enum transfer_state {
	NEED_FETCH,
	RUN_FETCH_LOOSE,
	RUN_FETCH_PACKED,
	NEED_PUSH,
	RUN_MKCOL,
	RUN_PUT,
	RUN_MOVE,
	ABORTED,
	COMPLETE,
};

static struct transfer_request *request_queue_head;

struct xml_ctx
{
	char *name;
	int len;
	char *cdata;
	void (*userFunc)(struct xml_ctx *ctx, int tag_closed);
	void *userData;
};

struct remote_lock
{
	char *url;
	char *owner;
	char *token;
	time_t start_time;
	long timeout;
	int refreshing;
	struct remote_lock *next;
};

/* Flags that control remote_ls processing */
#define PROCESS_FILES (1u << 0)
#define PROCESS_DIRS  (1u << 1)
#define RECURSIVE     (1u << 2)

/* Flags that remote_ls passes to callback functions */
#define IS_DIR (1u << 0)

struct remote_ls_ctx
{
	char *path;
	void (*userFunc)(struct remote_ls_ctx *ls);
	void *userData;
	int flags;
	char *dentry_name;
	int dentry_flags;
	struct remote_ls_ctx *parent;
};

typedef int CURLcode;
typedef struct {} CURL;

struct slot_results
{
	CURLcode curl_result;
	long http_code;
};

struct active_request_slot
{
	CURL *curl;
	FILE *local;
	int in_use;
	CURLcode curl_result;
	long http_code;
	int *finished;
	struct slot_results *results;
	void *callback_data;
	void (*callback_func)(void *data);
	struct active_request_slot *next;
};

#define CURLOPT_URL 0
#define CURLOPT_NOBODY 0
#define CURLE_OK 0

static int remote_exists(const char *path)
{
	char *url = xmalloc(strlen(remote->url) + strlen(path) + 1);	/* allocation site */
	struct active_request_slot *slot;
	struct slot_results results;
	int ret = -1;

	sprintf(url, "%s%s", remote->url, path);

	slot = get_active_slot();
	slot->results = &results;
	curl_easy_setopt(slot->curl, CURLOPT_URL, url);
	curl_easy_setopt(slot->curl, CURLOPT_NOBODY, 1);

	if (start_active_slot(slot)) {
		run_active_slot(slot);
		free(url);	/* double-free */
		if (results.http_code == 404)
			ret = 0;
		else if (results.curl_result == CURLE_OK)
			ret = 1;
		else
			fprintf(stderr, "HEAD HTTP error %ld\n", results.http_code);
	} else {
		free(url);	/* double-free */
		fprintf(stderr, "Unable to start HEAD request\n");
	}

	free(url);
	return ret;
}

int main()
{
    remote_exists("path");
    return 0;
}




