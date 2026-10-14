#ifndef STATIC_ANALYZER_PROVIDER_TRANSPORT_FIXTURE_H
#define STATIC_ANALYZER_PROVIDER_TRANSPORT_FIXTURE_H

/* Keep the original function/parameter identity in a transport-focused test.
   Volatile pointee reads also prevent moving the memory use into the caller.
   These attributes are not ownership or analyzer contracts. */
#if defined(__GNUC__)
#define PT_NOINLINE __attribute__((noinline, noclone))
#else
#define PT_NOINLINE
#endif

#endif
