
;; Function main (main, funcdef_no=51, decl_uid=4320, cgraph_uid=51, symbol_order=51)

main ()
{
  int * c;
  int * q;
  int * p;
  pthread_t t;
  int D.4327;

  _1 = foo (12);
  q = _1;
  p = foo (10);
  p = &q;
  q.0_2 = q;
  free (q.0_2);
  q.1_3 = q;
  free (q.1_3);
  D.4327 = 0;
  goto <D.4329>;
  <D.4329>:
  q = {CLOBBER};
  goto <D.4328>;
  D.4327 = 0;
  goto <D.4328>;
  <D.4328>:
  return D.4327;
}


