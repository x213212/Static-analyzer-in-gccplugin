
;; Function foo3 (foo3, funcdef_no=51, decl_uid=4311, cgraph_uid=51, symbol_order=53)

__attribute__((noinline))
foo3 (int * z)
{
  _1 = malloc (1);
  _2 = (long int) _1;
  _3 = (int) _2;
  *z = _3;
  return;
}



;; Function foo2 (foo2, funcdef_no=52, decl_uid=4309, cgraph_uid=52, symbol_order=54)

__attribute__((noinline))
foo2 (int z)
{
  int tmp;
  int * p4;
  int * p3;
  int * a2;
  int * D.4370;

  _1 = (long unsigned int) z;
  a2 = malloc (_1);
  _2 = (long unsigned int) z;
  p3 = malloc (_2);
  _3 = (long unsigned int) z;
  p4 = malloc (_3);
  if (tmp > 10) goto <D.4368>; else goto <D.4369>;
  <D.4368>:
  pthread_mutex_unlock (&mLock);
  *p3 = 10;
  D.4370 = p3;
  goto <D.4371>;
  <D.4369>:
  *p4 = 10;
  D.4370 = p4;
  goto <D.4371>;
  free (a2);
  D.4370 = a2;
  goto <D.4371>;
  <D.4371>:
  return D.4370;
}



;; Function foo4 (foo4, funcdef_no=53, decl_uid=4319, cgraph_uid=53, symbol_order=55)

foo4 (int z)
{
  int * p2;
  int * b;
  int * D.4372;

  _1 = malloc (1);
  b = _1;
  p2 = malloc (1);
  b.0_2 = b;
  *b.0_2 = 2;
  *p2 = 1;
  p2 = &b;
  free (p2);
  D.4372 = p2;
  goto <D.4374>;
  <D.4374>:
  b = {CLOBBER};
  goto <D.4373>;
  <D.4373>:
  return D.4372;
}



;; Function foo (foo, funcdef_no=54, decl_uid=4307, cgraph_uid=54, symbol_order=56)

__attribute__((noinline))
foo (int z)
{
  int * p2;
  int * D.4375;

  p2 = malloc (1);
  D.4375 = p2;
  goto <D.4376>;
  <D.4376>:
  return D.4375;
}



;; Function child (child, funcdef_no=55, decl_uid=4340, cgraph_uid=55, symbol_order=57)

child (void * data)
{
  int data2;
  int * a;
  int * pData;
  int * * ppData;

  pthread_mutex_lock (&mLock);
  ppData = malloc (10);
  _1 = malloc (20);
  pData = _1;
  a = foo (1);
  data2 = 0;
  ppData = &pData;
  pData = &data2;
  _2 = *ppData;
  *_2 = 10;
  _3 = ppData + 8;
  _4 = *_3;
  *_4 = 10;
  _5 = ppData + 16;
  _6 = *_5;
  *_6 = 10;
  _7 = *ppData;
  *_7 = 10;
  pData.1_8 = pData;
  free (pData.1_8);
  free (ppData);
  pthread_exit (0B);
}



;; Function boo (boo, funcdef_no=56, decl_uid=4350, cgraph_uid=56, symbol_order=58)

boo (int * b)
{
  free (b);
  printf ("asdda\n");
  return;
}



;; Function printf (<unset-asm-name>, funcdef_no=31, decl_uid=798, cgraph_uid=31, symbol_order=31)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
printf (const char * restrict __fmt)
{
  int D.4377;

  D.4377 = __printf_chk (1, __fmt, __builtin_va_arg_pack ());
  goto <D.4378>;
  <D.4378>:
  return D.4377;
}



;; Function test44 (test44, funcdef_no=57, decl_uid=4317, cgraph_uid=57, symbol_order=59)

__attribute__((noinline))
test44 (int * k)
{
  free (k);
  return;
}



;; Function test33 (test33, funcdef_no=58, decl_uid=4315, cgraph_uid=58, symbol_order=60)

__attribute__((noinline))
test33 (int * k)
{
  test44 (k);
  return;
}



;; Function test22 (test22, funcdef_no=59, decl_uid=4313, cgraph_uid=59, symbol_order=61)

__attribute__((noinline))
test22 (int * k)
{
  free (k);
  return;
}



;; Function main (main, funcdef_no=60, decl_uid=4361, cgraph_uid=60, symbol_order=62)

main ()
{
  pthread_t t;
  char buff[50];
  int * p2;
  int * p;
  int D.4379;

  p = foo2 (2);
  p2 = foo2 (2);
  test22 (p);
  test33 (p2);
  free (p);
  free (p2);
  pthread_mutex_destroy (&mLock);
  pthread_create (&t, 0B, child, &buff);
  t.2_1 = t;
  pthread_join (t.2_1, 0B);
  pthread_mutex_destroy (&mLock);
  D.4379 = 0;
  goto <D.4381>;
  <D.4381>:
  buff = {CLOBBER};
  t = {CLOBBER};
  goto <D.4380>;
  D.4379 = 0;
  goto <D.4380>;
  <D.4380>:
  return D.4379;
}


