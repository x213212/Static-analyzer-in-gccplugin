
;; Function test44 (test44, funcdef_no=1, decl_uid=3989, cgraph_uid=4, symbol_order=59) (executed once)

__attribute__((noinline))
test44 (int * k)
{
  <bb 2> [100.00%]:
  free (k_2(D));
  return;

}



;; Function test33 (test33, funcdef_no=2, decl_uid=3988, cgraph_uid=3, symbol_order=60) (executed once)

__attribute__((noinline))
test33 (int * k)
{
  <bb 2> [100.00%]:
  test44 (k_2(D));
  return;

}



;; Function test22 (test22, funcdef_no=3, decl_uid=3987, cgraph_uid=2, symbol_order=61) (executed once)

__attribute__((noinline))
test22 (int * k)
{
  <bb 2> [100.00%]:
  free (k_2(D));
  return;

}



;; Function foo (foo, funcdef_no=0, decl_uid=3990, cgraph_uid=5, symbol_order=56) (executed once)

__attribute__((noinline))
foo (int z)
{
  int * p2;

  <bb 2> [100.00%]:
  p2_3 = malloc (1);
  return p2_3;

}



;; Function child (child, funcdef_no=4, decl_uid=3986, cgraph_uid=1, symbol_order=57) (executed once)

child (void * data)
{
  int data2;
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
  data2 = 0;
  pData = &data2;
  _2 = pData;
  *_2 = 10;
  _3 = MEM[(int * *)&pData + 8B];
  *_3 = 10;
  _4 = MEM[(int * *)&pData + 16B];
  *_4 = 10;
  _5 = pData;
  *_5 = 10;
  _6 = pData;
  free (_6);
  free (&pData);
  pthread_mutex_unlock (&mLock);
  pthread_exit (0B);

}



;; Function main (main, funcdef_no=5, decl_uid=3985, cgraph_uid=0, symbol_order=62) (executed once)

main ()
{
  pthread_t t;
  char buff[50];
  int * p2;
  int * p;
  long unsigned int _1;

  <bb 2> [100.00%]:
  p_4 = foo (2);
  p2_6 = foo (2);
  test22 (p_4);
  test33 (p2_6);
  pthread_mutex_destroy (&mLock);
  pthread_create (&t, 0B, child, &buff);
  _1 = t;
  pthread_join (_1, 0B);
  pthread_mutex_destroy (&mLock);
  buff ={v} {CLOBBER};
  t ={v} {CLOBBER};
  return 0;

}


