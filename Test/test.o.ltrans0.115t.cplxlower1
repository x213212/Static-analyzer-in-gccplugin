
;; Function child (child, funcdef_no=0, decl_uid=3986, cgraph_uid=1, symbol_order=57) (executed once)

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

  <bb 2> [100.00%]:
  pthread_mutex_lock (&mLock);
  ppData_9 = malloc (10);
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
  pthread_mutex_unlock (&mLock);
  pthread_exit (0B);

}



;; Function main (main, funcdef_no=1, decl_uid=3985, cgraph_uid=0, symbol_order=59) (executed once)

main ()
{
  pthread_t t;
  char buff[50];
  int * p;
  long unsigned int _1;
  void * _12;
  long int _13;
  int _14;

  <bb 2> [100.00%]:
  _12 = malloc (1);
  _13 = (long int) _12;
  _14 = (int) _13;
  *p_3(D) = _14;
  pthread_mutex_destroy (&mLock);
  pthread_create (&t, 0B, child, &buff);
  _1 = t;
  pthread_join (_1, 0B);
  pthread_mutex_destroy (&mLock);
  buff ={v} {CLOBBER};
  t ={v} {CLOBBER};
  return 0;

}


