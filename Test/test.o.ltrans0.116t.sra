
;; Function foo (foo, funcdef_no=0, decl_uid=3986, cgraph_uid=1, symbol_order=52)

__attribute__((noinline))
foo (int * z)
{
  <bb 2> [100.00%]:
  free (z_2(D));
  return;

}



;; Function main (main, funcdef_no=1, decl_uid=3985, cgraph_uid=0, symbol_order=53) (executed once)

main ()
{
  int i;
  int i;
  int * p100;
  struct st * p;
  struct st * p9;
  int * p6;
  int * q;
  int test5;
  int test3;
  int test2;
  int test;
  int * p5;
  int * p4;
  int * p3;
  long int _1;
  int _2;
  long int _3;
  int _4;
  void * _5;
  void * _6;
  long unsigned int _7;
  long unsigned int _8;
  struct st * _9;
  void * _10;
  void * _11;
  long unsigned int _12;
  long unsigned int _13;
  struct st * _14;
  int * _15;
  long unsigned int _16;
  long unsigned int _17;
  struct st * _18;
  int * _19;
  int _20;
  int _24;

  <bb 2> [9.10%]:
  p6_36 = malloc (100);
  p6_39 = realloc (p6_36, 100);
  _1 = (long int) "0";
  _2 = (int) _1;
  *p6_36 = _2;
  _3 = (long int) "0";
  _4 = (int) _3;
  MEM[(int *)p6_36 + 4B] = _4;
  free (p6_39);
  p9_44 = malloc (0);
  _5 = malloc (1);
  MEM[(struct st *)p9_44 + 48B].f = _5;
  free (p9_44);
  p_49 = malloc (10);
  _6 = malloc (1);
  MEM[(struct st *)p_49 + 48B].f = _6;

  <bb 3> [90.91%]:
  # i_138 = PHI <i_87(3), 0(2)>
  _7 = (long unsigned int) i_138;
  _8 = _7 * 16;
  _9 = p_49 + _8;
  _10 = malloc (1);
  _9->f = _10;
  i_87 = i_138 + 1;
  if (i_87 <= 9)
    goto <bb 3>; [90.91%]
  else
    goto <bb 4>; [9.09%]

  <bb 4> [9.09%]:
  _11 = malloc (1);
  MEM[(struct st *)p_49 + 32B].f = _11;
  foo (p_49);
  if (test_55(D) != 0)
    goto <bb 30>; [31.00%]
  else
    goto <bb 5>; [69.00%]

  <bb 5> [6.28%]:
  # i_37 = PHI <0(4)>
  if (i_37 <= 9)
    goto <bb 6>; [90.91%]
  else
    goto <bb 7>; [9.09%]

  <bb 6> [62.73%]:
  # i_139 = PHI <i_84(6), i_37(5)>
  _12 = (long unsigned int) i_139;
  _13 = _12 * 16;
  _14 = p_49 + _13;
  _15 = _14->f;
  free (_15);
  _16 = (long unsigned int) i_139;
  _17 = _16 * 16;
  _18 = p9_44 + _17;
  _19 = _18->f;
  free (_19);
  i_84 = i_139 + 1;
  if (i_84 <= 9)
    goto <bb 6>; [90.91%]
  else
    goto <bb 7>; [9.09%]

  <bb 7> [6.27%]:
  p100_57 = malloc (1);
  if (test_55(D) != 0)
    goto <bb 8>; [33.00%]
  else
    goto <bb 9>; [67.00%]

  <bb 8> [2.07%]:
  p100_59 = malloc (2);
  free (p100_59);

  <bb 9> [6.27%]:
  # p100_23 = PHI <p100_57(7), p100_59(8)>
  free (p100_23);

label2 [11.28%]:
  foo (p5_62(D));
  goto <bb 12>; [100.00%]

  <bb 11> [66.82%]:

  <bb 12> [78.11%]:
  foo (p5_62(D));
  _20 = test5_65(D) / 10;
  switch (_20) <default: <L4> [33.33%], case 5: <L2> [33.33%], case 10: <L3> [33.33%]>

<L2> [26.04%]:
  foo (p5_62(D));
  goto <bb 16>; [100.00%]

<L3> [26.04%]:
  foo (p5_62(D));
  goto <bb 16>; [100.00%]

<L4> [26.04%]:
  foo (p5_62(D));

  <bb 16> [78.11%]:
  if (test_55(D) != 0)
    goto <bb 17>; [92.50%]
  else
    goto <bb 26>; [7.50%]

  <bb 17> [72.25%]:
  if (test_55(D) > test2_69(D))
    goto <bb 18>; [7.50%]
  else
    goto <bb 21>; [92.50%]

  <bb 18> [5.42%]:
  foo (p_49);
  if (test_55(D) < test2_69(D))
    goto <bb 19>; [50.00%]
  else
    goto <bb 20>; [50.00%]

  <bb 19> [2.71%]:
  foo (p_49);
  goto <bb 24>; [100.00%]

  <bb 20> [2.71%]:
  foo (p_49);
  goto <bb 24>; [100.00%]

  <bb 21> [66.83%]:
  foo (p_49);
  if (test_55(D) < test2_69(D))
    goto <bb 22>; [50.00%]
  else
    goto <bb 23>; [50.00%]

  <bb 22> [33.41%]:
  foo (p3_75(D));
  goto <bb 11>; [100.00%]

  <bb 23> [33.41%]:
  foo (p5_62(D));
  goto <bb 11>; [100.00%]

  <bb 24> [5.42%]:
  if (test3_80(D) != 0)
    goto <bb 25>; [92.50%]
  else
    goto <bb 30>; [7.50%]

  <bb 25> [5.01%]:
  foo (p_49);
  goto <bb 10> (label2); [100.00%]

  <bb 26> [5.86%]:
  if (test_55(D) == test2_69(D))
    goto <bb 27>; [17.77%]
  else
    goto <bb 28>; [82.23%]

  <bb 27> [1.04%]:
  foo (p_49);
  goto <bb 30>; [100.00%]

  <bb 28> [4.82%]:
  if (test_55(D) > test2_69(D))
    goto <bb 29>; [36.64%]
  else
    goto <bb 30>; [63.36%]

  <bb 29> [1.76%]:
  foo (p4_70(D));

  <bb 30> [9.08%]:
  # _24 = PHI <200(4), 0(24), 0(27), 0(28), 0(29)>
  return _24;

}


