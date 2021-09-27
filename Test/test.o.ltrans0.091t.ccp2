
;; Function foo (foo, funcdef_no=0, decl_uid=3986, cgraph_uid=1, symbol_order=51) (executed once)

__attribute__((noinline))
foo (int * z)
{
  void * _1;
  long int _2;
  int _3;

  <bb 2> [100.00%]:
  _1 = malloc (10);
  _2 = (long int) _1;
  _3 = (int) _2;
  *z_6(D) = _3;
  return;

}



;; Function main (main, funcdef_no=1, decl_uid=3985, cgraph_uid=0, symbol_order=52) (executed once)

main ()
{
  int test;
  int * p;

  <bb 2> [100.00%]:
  foo (p_3(D));
  if (test_5(D) != 0)
    goto <bb 3>; [36.64%]
  else
    goto <bb 4>; [63.36%]

  <bb 3> [36.64%]:
  foo (p_3(D));
  free (p_3(D));

  <bb 4> [100.00%]:
  free (p_3(D));
  return 0;

}


