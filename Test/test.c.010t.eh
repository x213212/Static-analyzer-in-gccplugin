
;; Function foo (foo, funcdef_no=51, decl_uid=4307, cgraph_uid=51, symbol_order=51)

__attribute__((noinline))
foo (int * z)
{
  _1 = malloc (10);
  _2 = (long int) _1;
  _3 = (int) _2;
  *z = _3;
  return;
}



;; Function main (main, funcdef_no=52, decl_uid=4313, cgraph_uid=52, symbol_order=52)

main ()
{
  int test;
  int * p2;
  int * p;
  int D.4321;

  foo (p);
  if (test != 0) goto <D.4319>; else goto <D.4320>;
  <D.4319>:
  foo (p);
  free (p);
  <D.4320>:
  free (p);
  D.4321 = 0;
  goto <D.4322>;
  D.4321 = 0;
  goto <D.4322>;
  <D.4322>:
  return D.4321;
}


