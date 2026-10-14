#ifndef STATIC_ANALYZER_LOCAL_CONFINEMENT_H
#define STATIC_ANALYZER_LOCAL_CONFINEMENT_H

namespace static_analyzer {
namespace local_confinement {

// Reset at the start of each plugin analysis pass, before GIMPLE can change.
void reset();

// A sufficient whole-function theorem for fresh allocations created in cfun:
// neither their pointers nor pointers hidden in local containers can be
// released or published on a normal-return path. Every pointer-bearing call
// is checked transitively against its actually bound GIMPLE body. This is
// NOT a borrowing contract for arbitrary inputs, nor an access/return theorem.
// Failure, unsupported IR, recursion or exhausted work means false.
bool fresh_local_ownership_is_confined();

}  // namespace local_confinement
}  // namespace static_analyzer

#endif
