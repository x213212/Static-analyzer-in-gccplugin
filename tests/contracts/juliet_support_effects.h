#ifndef STATIC_ANALYZER_TESTS_CONTRACTS_JULIET_SUPPORT_EFFECTS_H
#define STATIC_ANALYZER_TESTS_CONTRACTS_JULIET_SUPPORT_EFFECTS_H

/*
 * Opt-in trusted library contracts for the Juliet test-support functions.
 *
 * This header is benchmark evidence, not part of the unmodified official
 * Juliet corpus and not part of the raw Juliet headline.  A validation run
 * must explicitly include it and must identify that run as contract-assisted.
 *
 * These attributes are trusted axioms.  They are justified below from the
 * corresponding function bodies in testcasesupport/io.c; the analyzer must
 * never infer them from a function name.  Revalidate the contracts whenever
 * io.c changes.  An incorrect trusted contract can make proof-only results
 * unsound.
 */
#include "../../Test/benchmarks/Juliet_Test_Suite_v1.3_for_C_Cpp/C/testcasesupport/std_testcase.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * io.c: printLine first tests `line != NULL`, then reads the pointed-to C
 * string through printf("%s\n", line).  It neither frees nor retains line.
 */
void printLine(const char *line)
    __attribute__((static_analyzer_borrow(1),
                   static_analyzer_access_if_nonnull(1)));

/*
 * io.c: printWLine first tests `line != NULL`, then reads the pointed-to wide
 * string through wprintf(L"%ls\n", line).  It neither frees nor retains line.
 */
void printWLine(const wchar_t *line)
    __attribute__((static_analyzer_borrow(1),
                   static_analyzer_access_if_nonnull(1)));

/*
 * io.c: printStructLine unconditionally reads intOne and intTwo from its
 * pointer argument before passing those scalar values to printf.  It neither
 * frees nor retains the pointer.
 */
void printStructLine(const twoIntsStruct *structTwoIntsStruct)
    __attribute__((static_analyzer_borrow(1),
                   static_analyzer_must_access(1)));

/*
 * The selected wide-string Juliet families also pass the owned destination
 * through the ISO C library before calling printWLine.  These declaration
 * contracts model the library specification, not a spelling heuristic:
 * wcscpy reads/writes but neither retains nor releases either pointer, and
 * wmemset writes (possibly zero elements) without retaining or releasing its
 * destination.  The return-alias of wcscpy is intentionally not used here;
 * these testcase calls discard it.
 */
wchar_t *wcscpy(wchar_t *destination, const wchar_t *source)
    __attribute__((static_analyzer_borrow(1),
                   static_analyzer_borrow(2),
                   static_analyzer_must_access(1),
                   static_analyzer_must_access(2)));
wchar_t *wmemset(wchar_t *destination, wchar_t value, size_t count)
    __attribute__((static_analyzer_borrow(1)));

#ifdef __cplusplus
}
#endif

#endif  /* STATIC_ANALYZER_TESTS_CONTRACTS_JULIET_SUPPORT_EFFECTS_H */
