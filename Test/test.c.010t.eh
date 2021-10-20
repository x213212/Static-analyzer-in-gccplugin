
;; Function foo3 (foo3, funcdef_no=51, decl_uid=4314, cgraph_uid=51, symbol_order=51)

foo3 (struct st * p)
{
  int * q;
  int * D.4348;

  _1 = p->flag;
  if (_1 != 0) goto <D.4345>; else goto <D.4346>;
  <D.4345>:
  q = malloc (1);
  goto <D.4347>;
  <D.4346>:
  q = p->f;
  <D.4347>:
  D.4348 = q;
  goto <D.4349>;
  <D.4349>:
  return D.4348;
}



;; Function foo (foo, funcdef_no=52, decl_uid=4307, cgraph_uid=52, symbol_order=52)

__attribute__((noinline))
foo (int * z)
{
  free (z);
  return;
}



;; Function main (main, funcdef_no=53, decl_uid=4320, cgraph_uid=53, symbol_order=53)

main ()
{
  int i;
  int i;
  struct st * p;
  int d;
  int c;
  int b;
  int a;
  int test5;
  int test4;
  int test3;
  int test2;
  int test;
  int * p5;
  int * p4;
  int * p3;
  int * p2;
  int D.4350;

  a = 10;
  b = 22;
  c = 22;
  d = 22;
  p = malloc (10);
  i = 0;
  goto <D.4338>;
  <D.4337>:
  _1 = (long unsigned int) i;
  _2 = _1 * 16;
  _3 = p + _2;
  _4 = malloc (1);
  _3->f = _4;
  i = i + 1;
  <D.4338>:
  if (i <= 9) goto <D.4337>; else goto <D.4339>;
  <D.4339>:
  i = 0;
  goto <D.4342>;
  <D.4341>:
  _5 = (long unsigned int) i;
  _6 = _5 * 16;
  _7 = p + _6;
  _8 = _7->f;
  free (_8);
  i = i + 1;
  <D.4342>:
  if (i <= 9) goto <D.4341>; else goto <D.4343>;
  <D.4343>:
  D.4350 = 0;
  goto <D.4351>;
  D.4350 = 0;
  goto <D.4351>;
  D.4350 = 0;
  goto <D.4351>;
  <D.4351>:
  return D.4350;
}


