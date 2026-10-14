#include <stddef.h>

typedef struct URLContext URLContext;
typedef struct AVDictionary AVDictionary;
extern int ffurl_get_multi_file_handle(URLContext *, int **, int *);
extern int av_dict_set(AVDictionary **, const char *, const char *, int);
extern void av_dict_free(AVDictionary **);
extern void av_freep(void *);
extern void arbitrary_consumer(AVDictionary **);

int output_error_path_loses_owned_cell(URLContext *input) {
  int *array = NULL, count;
  if (ffurl_get_multi_file_handle(input, &array, &count)) return -1;
  if (count != 2) return -2;
  av_freep(&array);
  return 0;
}

int output_error_path_is_repaired(URLContext *input) {
  int *array = NULL, count;
  if (ffurl_get_multi_file_handle(input, &array, &count)) return -1;
  if (count != 2) { av_freep(&array); return -2; }
  av_freep(&array);
  return 0;
}

void dictionary_error_path_leaks(int fail) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "first", "alpha", 0);
  av_dict_set(&dictionary, "second", "beta", 0);
  if (fail) return;
  av_dict_free(&dictionary);
}

void guarded_release_is_safe(void) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "first", "alpha", 0);
  if (dictionary != NULL) av_dict_free(&dictionary);
}

void deletion_is_not_an_allocation(void) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "first", NULL, 0);
}

void unknown_consumer_stays_unknown(void) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "first", "alpha", 0);
  arbitrary_consumer(&dictionary);
}

void borrowed_input_is_not_new_owned_storage(AVDictionary *dictionary) {
  av_dict_set(&dictionary, "first", "alpha", 0);
}

void contradictory_error_path_is_safe(int condition) {
  AVDictionary *dictionary = NULL;
  if (condition) av_dict_set(&dictionary, "first", "alpha", 0);
  if (!condition) return;
  av_dict_free(&dictionary);
}

AVDictionary *returned_ownership_is_not_a_local_leak(void) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "key", "value", 0);
  return dictionary;
}

/* Externally observable publication must survive GCC's earlier optimization. */
AVDictionary *published;
void published_ownership_is_not_a_proven_leak(void) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "key", "value", 0);
  published = dictionary;
}

void opaque_assembly_is_not_proven_non_releasing(void) {
  AVDictionary *dictionary = NULL;
  av_dict_set(&dictionary, "key", "value", 0);
  __asm__ volatile("" : "+m"(dictionary) : : "memory");
}
