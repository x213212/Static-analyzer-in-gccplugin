#include <stdlib.h>

extern void tick(unsigned int);

/* There are 2^30 syntactic paths between the release guard and the use
   guard.  The only apparent release-then-use path additionally requires both
   gate and !gate, so it is infeasible.  The independent diamonds are kept in
   late GIMPLE and have side effects, but none contains a lifetime event.  */
__attribute__((noinline, optimize("O0")))
void thirty_diamonds_unsat(int gate, const unsigned char *conditions) {
  int *pointer = (int *)malloc(sizeof(*pointer));

  if (gate)
    free(pointer);

  if (conditions[0]) tick(0);
  if (conditions[1]) tick(1);
  if (conditions[2]) tick(2);
  if (conditions[3]) tick(3);
  if (conditions[4]) tick(4);
  if (conditions[5]) tick(5);
  if (conditions[6]) tick(6);
  if (conditions[7]) tick(7);
  if (conditions[8]) tick(8);
  if (conditions[9]) tick(9);
  if (conditions[10]) tick(10);
  if (conditions[11]) tick(11);
  if (conditions[12]) tick(12);
  if (conditions[13]) tick(13);
  if (conditions[14]) tick(14);
  if (conditions[15]) tick(15);
  if (conditions[16]) tick(16);
  if (conditions[17]) tick(17);
  if (conditions[18]) tick(18);
  if (conditions[19]) tick(19);
  if (conditions[20]) tick(20);
  if (conditions[21]) tick(21);
  if (conditions[22]) tick(22);
  if (conditions[23]) tick(23);
  if (conditions[24]) tick(24);
  if (conditions[25]) tick(25);
  if (conditions[26]) tick(26);
  if (conditions[27]) tick(27);
  if (conditions[28]) tick(28);
  if (conditions[29]) tick(29);

  if (!gate) {
    *pointer = 1;
    free(pointer);
  }
}
