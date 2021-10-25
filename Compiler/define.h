#define FOR_EACH_TABLE(TABLE, TAR)                           \
	for ((TAR) = ((TABLE) == NULL) ? NULL : (TABLE)->target; \
		 (TAR) != NULL;                                      \
		 (TABLE) = (TABLE)->next , (TAR) = ((TABLE) == NULL) ? NULL : (TABLE)->target)
#define FOR_EACH_USE_TABLE(USE, STMT) \
	for ((STMT) = ((USE) == NULL) ? NULL : (USE)->stmt; (USE) != NULL; (USE) = (USE)->next, (STMT) = ((USE) == NULL) ? NULL : (USE)->stmt)

/*allocation and deallocation table*/
struct ptb
{
	tree target;
	location_t loc;
	ptb *next;
	int state;
	bool may_realloc, is_phi_stmt;
	gimple *last_stmt, *dealloc_stmt;
	basic_block bb;
	cgraph_node *node;
	gimple *caller;
	function *fun;
	bool removed;
	bool inbranch;
	gimple *swap_stmt;
	tree swap_target;
	int swap_type;
	int pthread_type = 0;
	int size = 0;
};

/*ptb declaration*/
ptb start1, start2, start3, start4, start5, start6, start7, start8, start9;
ptb *ptable, *retable, *ftable;
ptb *phitable, *return_table, *use_table, *fopen_table;
ptb *locktable;
ptb *unlocktable;
struct symbolicinfoPatharray
{
	// gimple *cond_stmt;
	basic_block bb;
	int boolt = 1;
};

struct breakpoint{
	string name;
	int line;
};
struct symbolicinfo
{
	gimple *cond_stmt;
	gimple *switch_stmt;
	tree cond_rhs;
	tree cond_lhs;
	vector<basic_block> switchs;
	basic_block cond_truebranch;
	basic_block cond_falsebranch;
	int prevlogic = 1;
	// tree symbolicExecutionPathConstraintNode;
	vector<basic_block> symbolicExecutionPathConstraint;
	vector<symbolicinfoPatharray> symbolicExecutionPathConstraintarrayTrue;
	vector<symbolicinfoPatharray> symbolicExecutionPathConstraintarrayFalse;
	// hash_map<basic_block, symbolicinfoPatharray> *syminfo;
};

struct gimple_array
{
	gimple *stmt;
	gimple *ret_stmt;
	tree aptr;
	tree target;
	gimple_array *next;
	int size = 0;
};

/*function decl tree and rhs tree*/
struct fdecl
{
	tree fndecl;
	tree fn;
};

/*function decl and location*/
struct var_points_to
{
	vector<vector<pair<fdecl, location_t>>> may_malloc_path;
	tree decl;
};

/*

int return_type
return type
normal 0
pointer 0
function pointer
*/
struct function_path
{
	//gimple *stmt;
	//n/p treee
	cgraph_node *cgnext;
	tree prev;
	tree next;

	//int return_type;
};
struct return_type
{
	int return_type_stmt_num = 0;
	gimple *stmt;
	tree return_tree;

	//int return_type;
};
struct assign_type
{
	gimple *stmt;
	tree assign_tree;
	tree form_tree;
	// int reutnr_type_num = 0;
	//int return_type;
};
struct attr_type
{
	gimple *stmt;
	tree attr_tree;
	int attr_type_num = 0;
	//int return_type;
};

struct free_type
{
	gimple *stmt;
	tree free_tree;
	// int reutnr_type_num = 0;
	//int return_type;
};
struct relate_type
{
	gimple *stmt;
	gimple *laststmt;
	tree relate_tree;
	tree relate_funtree;
	unsigned int now_fucntion = 0;
	unsigned int now_stmt = 0;
	int now_basicblock = 0;
	// int reutnr_type_num = 0;
	//int return_type;
};

struct pthread_detched_type
{
	gimple *stmt;
	tree pthread_detched_tree;
	// int reutnr_type_num = 0;
	//int return_type;
};

struct pthread_exit_type
{
	gimple *stmt;
	tree pthread_exit_tree;
	// int reutnr_type_num = 0;
	//int return_type;
};

/*define return_type struct*/
struct function_return_array
{
	int return_type_num = 0;
	int pthread_type_num = -1;

	vector<return_type> return_type_array;
};

/*define return_type struct*/
struct function_path_array
{
	int funtion_type_num = 0;
	vector<function_path> function_path_array;
};

/*define assign_type struct*/
struct function_assign_array
{
	int pass = 0;
	vector<assign_type> assign_type_array;
};

/*define assign_type struct*/
struct pthread_attr_array
{
	gimple *stmt;
	tree attr_tree;
	int attr_type_num = 0;
	// int pass = 0;
	// vector<attr_type> attr_type_array;
};

/*define free_type struct*/
struct function_free_array
{
	vector<free_type> free_type_array;
};

/*define pthread_detched struct*/
struct function_pthread_detched_array
{
	vector<pthread_detched_type> pthread_detched_array;
};

/*define pthread_detched struct*/
struct function_pthread_exit_array
{
	vector<pthread_exit_type> pthread_exit_array;
};
struct function_graph_array
{
	// Graph graph_type_array;
	cgraph_node *graph_node;
};
/*define free_type struct*/
struct function_relate_array
{
	vector<relate_type> relate_type_array;
};
// struct function_relate_array2
// {
// 	<relate_type> relate_type_array;
// };
// struct function_relate_array
// {
// 	Graph graph_type_array;
// 	cgraph_node *graph_node;
// };
/*collect function return types */
hash_map<tree, function_return_array> *function_return_collect;
/*collect function var decl ssa name */
hash_map<tree, function_assign_array> *function_assign_collect;
/*collect function pthread attr */
hash_map<tree, pthread_attr_array> *pthread_attr_setdetachstates;
/*collect function pthread_detched */
hash_map<tree, function_pthread_detched_array> *function_pthread_detched_collect;
/*collect function pthread_exit */
hash_map<tree, function_pthread_exit_array> *function_pthread_exit_collect;
/*collect function path */
hash_map<tree, function_path_array> *function_path_collect;
/*collect function free */
hash_map<tree, function_free_array> *function_free_collect;
/*collect fucntion graph_array*/
hash_map<tree, function_graph_array> *function_graph_collect;
/*collect fucntion graph_array*/
hash_map<tree, function_relate_array> *function_relate_collect;
// /*collect fucntion maxbasicblock*/
// hash_map<tree, int> *function_maxbasicblock_collect;

/*record each DFS graph*/
// hash_map<cgraph_node *, Graph> *fDFS;

/*use function tree search cgraph_node*/
hash_map<tree, cgraph_node *> *fnode;

/*rhs function search function decl and location*/
hash_map<tree, var_points_to> *tvpt;
hash_map<tree, gimple_array> *treeGimpleArray;

/**/
hash_map<basic_block, symbolicinfo> *syminfo;
// hash_map<gimple *, tree> *symbolicExecutionPathConstraintNode;

/*state*/
unsigned int POINTER_NOT_EXIST = 0;
unsigned int POINTER_STATE_IS_FREE = 1;
unsigned int POINTER_STATE_IS_NORMAL = 2;
unsigned int POINTER_STATE_IS_MALLOC = 3;
unsigned int POINTER_STATE_IS_FILE = 4;
unsigned int POINTER_STATE_MAY_IS_FREE = 5;

const unsigned int POINTER_NOT = 0;
const unsigned int POINTER_MAY = 1;
const unsigned int POINTER_MUST = 2;

unsigned int IS_MALLOC_FUCNTION = 1;
unsigned int IS_OTHRER_FUCNTION = 2;
unsigned int IS_HEAP_FUCNTION = -1;
unsigned int PTABLE_IS_NULL = -2;
unsigned int FUNCITON_THREAD = 666;
unsigned int FUNCITON_HEAP = 777;
unsigned int CREATE_JOINABLE = 0;
unsigned int CREATE_DETACHED = 1;
unsigned int DISABLE_TREACE = -100;
/*dump file */
FILE *fp;
ofstream cfginfo("cfginfo.txt");

/*interprocedural analysis*/
bool ipa = true;
bool vscode_extensionmod =false;
bool debugmod = false;
bool threadmod = false;
bool relatemod = true;
bool freemod = true;
bool freemodv2 = false;
bool retmod = false; //關聯return stmt
bool pthread_detachedmod = true;
bool pthread_exitmod = true;
bool tracerelatestmt = true;

/*collect  function stack*/
class CStack : public std::stack<tree>
{
public:
	using std::stack<tree>::c; // expose the container
};
/*collect stmt stack*/
class CstmtStack : public std::stack<gimple *>
{
public:
	using std::stack<gimple *>::c; // expose the container
};

vector<gimple *> new_gimple_array;
vector<tree> new_gimpletree_array;
vector<tree> traceStack;
vector<tree> pathStack;
vector<basic_block> symbolicExecution;
vector<basic_block> symbolicExecutionswitch;
vector<breakpoint> vbreakpoint;


// CStack pathStack;
// CStack traceStack;
CStack stmtStack;
CstmtStack stmtStack2;
//stack<tree> pathStack;  // 建立堆疊
function *main_fun;
//int check_stmtStack(gimple *stmt);
static gimple *now_stmt;
unsigned int SDBMHash(char *str);
void printf_bbinfo(basic_block bb, int flag);
void set_PathConstraintarray(basic_block bb, int flag);
void set_bbinfo(basic_block bb);
void check_bbinfo(basic_block bb);
int check_stmtStack(tree target);
void checkPointerConstraint(tree function_tree, ptb *ptable, gimple_array *user_tmp, tree checkTree, int threadcheck);
void record_fucntion(cgraph_node *node);

void collect_function_call(gimple *gc, cgraph_node *node, basic_block bb);
void collect_FunctionMapping_Assign(gimple *gc, cgraph_node *node, basic_block bb,ptb *ptable);
void collect_FunctionMapping_Ret(tree function_tree, gimple *u_stmt, gimple_array *user_tmp, ptb *table_temp, ptb *ptable);

int trace_function_path(tree function_tree, int fucntion_level, tree mallocStmt_tree, int *freecount);
void walk_function_path(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp);
void tracefree_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp);
void tracefree_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp);

function_assign_array ret_function_varstmt(tree function_tree);
void collect_function_return(gimple *gc, cgraph_node *node, basic_block bb);
void FunctionStmtMappingRet(ptb *ptable, ptb *ftable, gimple_array *user_tmp);
void FunctionStmtMappingAssign(ptb *ptable, gimple_array *user_tmp);

struct timespec diff(struct timespec start, struct timespec end);

void printfBasicblock();
void dump_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp);
void new_search_imm_use(gimple_array *used_stmt, tree target, tree target2);
void Prenew_search_imm_use(gimple_array *used_stmt, tree target, tree target2);
void printfPointerConstraint(ptb *ptable, gimple_array *user_tmp);
void print_function_return(tree function_tree);
void print_function_return2(tree function_tree);
void trace_fucntion_relate_stmt(cgraph_node *node, tree function_tree, tree mallocStmt_tree);
