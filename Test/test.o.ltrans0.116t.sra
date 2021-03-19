
;; Function test22 (test22, funcdef_no=2, decl_uid=3987, cgraph_uid=2, symbol_order=61) (executed once)

__attribute__((noinline))
test22 (int * k)
{
  <bb 2> [100.00%]:
  free (k_2(D));
  return;

}



;; Function foo2 (foo2, funcdef_no=0, decl_uid=3989, cgraph_uid=4, symbol_order=54) (executed once)

__attribute__((noinline))
foo2 (int z)
{
  int tmp;
  int * p4;
  int * p3;
  int * a2;
  long unsigned int _1;
  long unsigned int _2;
  long unsigned int _3;
  int * _4;

  <bb 2> [100.00%]:
  _1 = (long unsigned int) z_6(D);
  a2_9 = malloc (_1);
  _2 = (long unsigned int) z_6(D);
  p3_11 = malloc (_2);
  _3 = (long unsigned int) z_6(D);
  p4_13 = malloc (_3);
  *a2_9 = 10;
  if (tmp_15(D) > 10)
    goto <bb 3>; [29.56%]
  else
    goto <bb 4>; [70.44%]

  <bb 3> [29.56%]:
  pthread_mutex_unlock (&mLock);
  *p3_11 = 10;
  goto <bb 5>; [100.00%]

  <bb 4> [70.44%]:
  *p4_13 = 10;

  <bb 5> [100.00%]:
  # _4 = PHI <p3_11(3), p4_13(4)>
  return _4;

}



;; Function foo (foo, funcdef_no=1, decl_uid=3988, cgraph_uid=3, symbol_order=56) (executed once)

__attribute__((noinline))
foo (int z)
{
  int * _4;

  <bb 2> [100.00%]:
  _4 = foo2 (z_2(D));
  return _4;

}



;; Function child (child, funcdef_no=3, decl_uid=3986, cgraph_uid=1, symbol_order=57) (executed once)

child (void * data)
{
  int data2;
  int * a;
  int * pData;
  int * * ppData;
  void * _1;
  int * _2;
  int * _3;
  int * _4;
  int * _5;
  int * _6;

  <bb 2> [100.00%]:
  pthread_mutex_lock (&mLock);
  ppData_10 = malloc (10);
  _1 = malloc (20);
  pData = _1;
  a_14 = foo (1);
  data2 = 0;
  pData = &data2;
  _2 = pData;
  *_2 = 10;
  _3 = MEM[(int * *)&pData + 8B];
  *_3 = 10;
  _4 = MEM[(int * *)&pData + 16B];
  *_4 = 10;
  _5 = pData;
  *_5 = 12;
  _6 = pData;
  free (_6);
  free (&pData);
  pthread_exit (0B);

}



;; Function main (main, funcdef_no=4, decl_uid=3985, cgraph_uid=0, symbol_order=62) (executed once)

main ()
{
  pthread_t t;
  int * q;
  char buff[50];
  int * p3;
  int * p2;
  int * p;
  long unsigned int _1;

  <bb 2> [100.00%]:
  p3_4 = foo (2);
  *p3_4 = 1;
  free (p3_4);
  p_8 = foo2 (2);
  *p_8 = 2;
  p2_11 = foo2 (2);
  *p2_11 = 4;
  free (p_8);
  free (p2_11);
  q_16 = malloc (5);
  *q_16 = 10;
  test22 (q_16);
  pthread_mutex_destroy (&mLock);
  pthread_create (&t, 0B, child, &buff);
  _1 = t;
  pthread_join (_1, 0B);
  pthread_mutex_destroy (&mLock);
  buff ={v} {CLOBBER};
  t ={v} {CLOBBER};
  return 0;

}


