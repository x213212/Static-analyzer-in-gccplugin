#include "gcc-plugin.h"
#include "tree.h"
#include "gimple.h"
#include "basic-block.h"
#include "cfg.h"

#include "static_analyzer/cfg_evidence.h"

#include <deque>
#include <set>

namespace static_analyzer {
namespace {

bool block_reaches(basic_block source, basic_block destination) {
  if (source == destination)
    return true;

  std::deque<basic_block> worklist;
  std::set<basic_block> visited;
  worklist.push_back(source);
  while (!worklist.empty()) {
    basic_block current = worklist.front();
    worklist.pop_front();
    if (!visited.insert(current).second)
      continue;

    edge successor;
    edge_iterator iterator;
    FOR_EACH_EDGE(successor, iterator, current->succs) {
      if (successor->dest == destination)
        return true;
      if (visited.find(successor->dest) == visited.end())
        worklist.push_back(successor->dest);
    }
  }
  return false;
}

}  // namespace

bool statements_may_execute_in_sequence(const gimple *first,
                                        const gimple *second) {
  if (first == nullptr || second == nullptr || first == second)
    return false;
  basic_block first_block = gimple_bb(first);
  basic_block second_block = gimple_bb(second);
  if (first_block == nullptr || second_block == nullptr)
    return false;
  return block_reaches(first_block, second_block) ||
         block_reaches(second_block, first_block);
}

}  // namespace static_analyzer
