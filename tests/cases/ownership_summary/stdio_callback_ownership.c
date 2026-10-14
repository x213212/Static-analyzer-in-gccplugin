#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__GLIBC__)
/* Negative ownership regression for a real GNU stdio callback, not an
   analyzer contract or a replacement definition of puts.  On glibc an
   unbuffered cookie stream forwards this complete 4095-byte write using
   the original source buffer.  The stream consumes and frees that buffer.

   The argument is LIVE on entry to puts.  A requires-live-string rule is
   therefore valid here, but an unconditional no-free/no-capture effect
   would invent a leak at the end of stdio_callback_ownership.  In
   particular the allocation is not published to the cookie beforehand.

   Define STATIC_ANALYZER_RUN_STDIO_CALLBACK_TEST to run this regression
   under a memory checker as well as compile it with the analyzer. */
static unsigned int stdio_callback_release_count;

static ssize_t consume_owned_output(void *cookie, const char *buffer,
                                    size_t count) {
  (void)cookie;
  if (count == 4095) {
    free((void *)buffer);
    ++stdio_callback_release_count;
  }
  /* The other write contains puts' newline and is not an owned buffer. */
  return (ssize_t)count;
}

void stdio_callback_ownership(void) {
  FILE *saved_stdout = stdout;
  cookie_io_functions_t hooks = {0};
  hooks.write = consume_owned_output;
  FILE *custom_stdout = fopencookie(NULL, "w", hooks);
  if (custom_stdout == NULL)
    abort();
  if (setvbuf(custom_stdout, NULL, _IONBF, 0) != 0) {
    fclose(custom_stdout);
    abort();
  }
  stdout = custom_stdout;

  char *value = (char *)malloc(4096);
  if (value == NULL) {
    stdout = saved_stdout;
    fclose(custom_stdout);
    abort();
  }
  memset(value, 'A', 4095);
  value[4095] = '\0';
  puts(value);

  stdout = saved_stdout;
  fclose(custom_stdout);
}

#ifdef STATIC_ANALYZER_RUN_STDIO_CALLBACK_TEST
int main(void) {
  stdio_callback_ownership();
  return stdio_callback_release_count != 1;
}
#endif
#else
/* The runtime counterexample specifically exercises GNU fopencookie. */
void stdio_callback_ownership(void) {}
#endif
