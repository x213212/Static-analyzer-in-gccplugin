
;; Function test22 (test22, funcdef_no=1, decl_uid=3988, cgraph_uid=4, symbol_order=61)

test22 (int * k)
{
  <bb 2> [100.00%]:
  test22 (k_2(D));
  free (k_2(D));
  return;

}



;; Function foo2 (foo2, funcdef_no=0, decl_uid=3989, cgraph_uid=5, symbol_order=54)

foo2 (int z)
{
  int tmp;
  int * p4;
  int * p3;
  int * a2;
  long unsigned int _1;
  long unsigned int _2;
  long unsigned int _3;

  <bb 2> [100.00%]:
  _1 = (long unsigned int) z_5(D);
  a2_8 = malloc (_1);
  _2 = (long unsigned int) z_5(D);
  p3_10 = malloc (_2);
  _3 = (long unsigned int) z_5(D);
  p4_12 = malloc (_3);
  *a2_8 = 10;
  if (tmp_14(D) > 10)
    goto <bb 3>; [29.56%]
  else
    goto <bb 4>; [70.44%]

  <bb 3> [29.56%]:
  pthread_mutex_unlock (&mLock);
  *a2_8 = 10;
  free (a2_8);
  goto <bb 5>; [100.00%]

  <bb 4> [70.44%]:
  *a2_8 = 20;

  <bb 5> [100.00%]:
  return a2_8;

}



;; Function child (child, funcdef_no=2, decl_uid=3986, cgraph_uid=1, symbol_order=57)

child (void * data)
{
  int * p2;
  int * D.4047;
  int data2;
  int * a;
  int * pData;
  int * * ppData;
  union pthread_mutex_t mLock2;
  void * _1;
  int * _2;
  int * _3;
  int * _4;
  int * _5;
  int * _6;
  int * _40;

  <bb 2> [100.00%]:
  pthread_mutex_lock (&mLock2);
  pthread_mutex_lock (&mLock2);
  pthread_mutex_lock (&mLock);
  pthread_mutex_lock (&mLock);
  ppData_13 = malloc (10);
  _1 = malloc (20);
  pData = _1;
  p2_39 = malloc (1);
  *p2_39 = 1;
  _40 = foo2 (1);
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
  pthread_mutex_unlock (&mLock);
  free (&pData);
  mLock2 ={v} {CLOBBER};
  pData ={v} {CLOBBER};
  data2 ={v} {CLOBBER};
  return;

}



;; Function main (main, funcdef_no=4, decl_uid=3985, cgraph_uid=0, symbol_order=62) (executed once)

main ()
{
  int * p2;
  int * D.4058;
  pthread_t t;
  int * q;
  char buff[50];
  int * p3;
  int * p2;
  int * p;
  long unsigned int _1;
  int * _37;

  <bb 2> [100.00%]:
  p2_36 = malloc (1);
  *p2_36 = 1;
  _37 = foo2 (2);
  *_37 = 1;
  free (_37);
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


