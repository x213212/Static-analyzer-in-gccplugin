
;; Function int safe_cond_timed_wait(pthread_cond_t*, pthread_mutex_t*, int, const char*) (_Z20safe_cond_timed_waitP14pthread_cond_tP15pthread_mutex_tiPKc, funcdef_no=1944, decl_uid=38994, cgraph_uid=685, symbol_order=733)

int safe_cond_timed_wait(pthread_cond_t*, pthread_mutex_t*, int, const char*) (union pthread_cond_t * cond, union pthread_mutex_t * mutex, int seconds, const char * caller)
{
  struct ErrorContext * D.45292;
  int D.45291;
  int pret;
  struct timezone tz;
  struct timeval tv;
  struct timespec waitTimer;
  int D.45289;

  gettimeofday (&tv, &tz);
  _1 = tv.tv_sec;
  _2 = (long int) seconds;
  _3 = _1 + _2;
  waitTimer.tv_sec = _3;
  _4 = tv.tv_usec;
  _5 = _4 * 1000;
  waitTimer.tv_nsec = _5;
  D.45291 = pthread_cond_timedwait (cond, mutex, &waitTimer);
  pret = D.45291;
  _6 = pret != 0;
  _7 = pret != 4;
  _8 = _6 & _7;
  if (_8 != 0) goto <D.45284>; else goto <D.45285>;
  <D.45284>:
  _9 = pret != 16;
  _10 = pret != 110;
  _11 = _9 & _10;
  if (_11 != 0) goto <D.45286>; else goto <D.45287>;
  <D.45286>:
  D.45292 = pbzip2::ErrorContext::getInstance ();
  _12 = D.45292;
  pbzip2::ErrorContext::saveError (_12);
  pthread_mutex_unlock (mutex);
  handle_error (1, 1, "pbzip2: *ERROR: %s:  pthread_cond_timedwait() call invalid [pret=%d].  This machine\n         does not have compatible pthreads library.  Aborting.\n", caller, pret);
  cleanupAndQuit (-9);
  goto <D.45288>;
  <D.45287>:
  <D.45288>:
  <D.45285>:
  D.45289 = 0;
  goto <D.45294>;
  <D.45294>:
  waitTimer = {CLOBBER};
  tv = {CLOBBER};
  tz = {CLOBBER};
  goto <D.45290>;
  <D.45290>:
  return D.45289;
  <D.45293>:
  waitTimer = {CLOBBER};
  tv = {CLOBBER};
  tz = {CLOBBER};
  resx 1
}



;; Function ssize_t do_write(int, const void*, size_t) (_Z8do_writeiPKvm, funcdef_no=1945, decl_uid=26104, cgraph_uid=686, symbol_order=734)

ssize_t do_write(int, const void*, size_t) (int fd, const void * buf, size_t count)
{
  struct ErrorContext * D.45309;
  ssize_t D.45308;
  const char * pbuf;
  ssize_t nbytes;
  ssize_t bytesRemaining;
  ssize_t D.45306;
  bool iftmp.1;
  bool retval.0;

  bytesRemaining = (ssize_t) count;
  nbytes = 0;
  pbuf = buf;
  <D.45296>:
  if (bytesRemaining > 0) goto <D.45301>; else goto <D.45299>;
  <D.45301>:
  bytesRemaining.2_1 = (long unsigned int) bytesRemaining;
  D.45308 = write (fd, pbuf, bytesRemaining.2_1);
  nbytes = D.45308;
  if (nbytes > 0) goto <D.45302>; else goto <D.45299>;
  <D.45302>:
  iftmp.1 = 1;
  goto <D.45300>;
  <D.45299>:
  iftmp.1 = 0;
  <D.45300>:
  retval.0 = iftmp.1;
  if (retval.0 != 0) goto <D.45303>; else goto <D.39200>;
  <D.45303>:
  bytesRemaining = bytesRemaining - nbytes;
  nbytes.3_2 = (sizetype) nbytes;
  pbuf = pbuf + nbytes.3_2;
  goto <D.45296>;
  <D.39200>:
  if (nbytes < 0) goto <D.45304>; else goto <D.45305>;
  <D.45304>:
  D.45309 = pbzip2::ErrorContext::getInstance ();
  _3 = D.45309;
  pbzip2::ErrorContext::saveError (_3);
  D.45306 = nbytes;
  goto <D.45307>;
  <D.45305>:
  bytesRemaining.4_4 = (long unsigned int) bytesRemaining;
  _5 = count - bytesRemaining.4_4;
  D.45306 = (ssize_t) _5;
  goto <D.45307>;
  <D.45307>:
  return D.45306;
}



;; Function ssize_t do_read(int, void*, size_t) (_Z7do_readiPvm, funcdef_no=1946, decl_uid=26100, cgraph_uid=687, symbol_order=735)

ssize_t do_read(int, void*, size_t) (int fd, void * buf, size_t count)
{
  struct ErrorContext * D.45323;
  ssize_t D.45322;
  char * pbuf;
  ssize_t nbytes;
  ssize_t bytesRemaining;
  ssize_t D.45320;
  bool iftmp.6;
  bool retval.5;

  bytesRemaining = (ssize_t) count;
  nbytes = 0;
  pbuf = buf;
  <D.45310>:
  if (bytesRemaining > 0) goto <D.45315>; else goto <D.45313>;
  <D.45315>:
  bytesRemaining.7_1 = (long unsigned int) bytesRemaining;
  D.45322 = read (fd, pbuf, bytesRemaining.7_1);
  nbytes = D.45322;
  if (nbytes > 0) goto <D.45316>; else goto <D.45313>;
  <D.45316>:
  iftmp.6 = 1;
  goto <D.45314>;
  <D.45313>:
  iftmp.6 = 0;
  <D.45314>:
  retval.5 = iftmp.6;
  if (retval.5 != 0) goto <D.45317>; else goto <D.39210>;
  <D.45317>:
  bytesRemaining = bytesRemaining - nbytes;
  nbytes.8_2 = (sizetype) nbytes;
  pbuf = pbuf + nbytes.8_2;
  goto <D.45310>;
  <D.39210>:
  if (nbytes < 0) goto <D.45318>; else goto <D.45319>;
  <D.45318>:
  D.45323 = pbzip2::ErrorContext::getInstance ();
  _3 = D.45323;
  pbzip2::ErrorContext::saveError (_3);
  D.45320 = nbytes;
  goto <D.45321>;
  <D.45319>:
  bytesRemaining.9_4 = (long unsigned int) bytesRemaining;
  _5 = count - bytesRemaining.9_4;
  D.45320 = (ssize_t) _5;
  goto <D.45321>;
  <D.45321>:
  return D.45320;
}



;; Function ssize_t read(int, void*, size_t) (read, funcdef_no=1052, decl_uid=25064, cgraph_uid=340, symbol_order=342)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
ssize_t read(int, void*, size_t) (int __fd, void * __buf, size_t __nbytes)
{
  ssize_t D.45335;
  ssize_t D.45334;
  ssize_t D.45333;
  ssize_t D.45328;

  _1 = __builtin_object_size (__buf, 0);
  if (_1 != 18446744073709551615) goto <D.45324>; else goto <D.45325>;
  <D.45324>:
  _2 = __builtin_constant_p (__nbytes);
  if (_2 == 0) goto <D.45326>; else goto <D.45327>;
  <D.45326>:
  _3 = __builtin_object_size (__buf, 0);
  D.45333 = __read_chk (__fd, __buf, __nbytes, _3);
  D.45328 = D.45333;
  goto <D.45332>;
  <D.45327>:
  _4 = __builtin_object_size (__buf, 0);
  if (__nbytes > _4) goto <D.45329>; else goto <D.45330>;
  <D.45329>:
  _5 = __builtin_object_size (__buf, 0);
  D.45334 = __read_chk_warn (__fd, __buf, __nbytes, _5);
  D.45328 = D.45334;
  goto <D.45332>;
  <D.45330>:
  goto <D.45331>;
  <D.45325>:
  <D.45331>:
  D.45335 = __read_alias (__fd, __buf, __nbytes);
  D.45328 = D.45335;
  goto <D.45332>;
  <D.45332>:
  return D.45328;
}



;; Function int safe_open_output(const char*) (_Z16safe_open_outputPKc, funcdef_no=1947, decl_uid=39213, cgraph_uid=688, symbol_order=736)

int safe_open_output(const char*) (const char * path)
{
  struct ErrorContext * D.45342;
  int D.45341;
  int ret;
  int D.45339;

  D.45341 = open (path, 193, 384);
  ret = D.45341;
  if (ret == -1) goto <D.45336>; else goto <D.45337>;
  <D.45336>:
  D.45342 = pbzip2::ErrorContext::getInstance ();
  _1 = D.45342;
  pbzip2::ErrorContext::saveError (_1);
  goto <D.45338>;
  <D.45337>:
  <D.45338>:
  D.45339 = ret;
  goto <D.45340>;
  <D.45340>:
  return D.45339;
}



;; Function int open(const char*, int, ...) (*open64, funcdef_no=1908, decl_uid=36338, cgraph_uid=649, symbol_order=651)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
int open(const char*, int, ...) (const char * __path, int __oflag)
{
  int D.45360;
  int D.45359;
  int D.45358;
  int D.45357;
  int D.45353;

  _1 = __builtin_va_arg_pack_len ();
  if (_1 > 1) goto <D.45343>; else goto <D.45344>;
  <D.45343>:
  __open_too_many_args ();
  goto <D.45345>;
  <D.45344>:
  <D.45345>:
  __oflag.10_2 = __oflag;
  _3 = __builtin_constant_p (__oflag.10_2);
  if (_3 != 0) goto <D.45346>; else goto <D.45347>;
  <D.45346>:
  __oflag.11_4 = __oflag;
  _5 = __oflag.11_4 & 64;
  if (_5 != 0) goto <D.45348>; else goto <D.45350>;
  <D.45350>:
  __oflag.12_6 = __oflag;
  _7 = __oflag.12_6 & 4259840;
  if (_7 == 4259840) goto <D.45348>; else goto <D.45349>;
  <D.45348>:
  _8 = __builtin_va_arg_pack_len ();
  if (_8 <= 0) goto <D.45351>; else goto <D.45352>;
  <D.45351>:
  __open_missing_mode ();
  __oflag.13_9 = __oflag;
  D.45357 = __open_2 (__path, __oflag.13_9);
  D.45353 = D.45357;
  goto <D.45356>;
  <D.45352>:
  <D.45349>:
  __oflag.14_10 = __oflag;
  D.45358 = __open_alias (__path, __oflag.14_10, __builtin_va_arg_pack ());
  D.45353 = D.45358;
  goto <D.45356>;
  <D.45347>:
  _11 = __builtin_va_arg_pack_len ();
  if (_11 <= 0) goto <D.45354>; else goto <D.45355>;
  <D.45354>:
  __oflag.15_12 = __oflag;
  D.45359 = __open_2 (__path, __oflag.15_12);
  D.45353 = D.45359;
  goto <D.45356>;
  <D.45355>:
  __oflag.16_13 = __oflag;
  D.45360 = __open_alias (__path, __oflag.16_13, __builtin_va_arg_pack ());
  D.45353 = D.45360;
  goto <D.45356>;
  <D.45356>:
  return D.45353;
}



;; Function FILE* safe_fopen_output(const char*, const char*) (_Z17safe_fopen_outputPKcS0_, funcdef_no=1948, decl_uid=39218, cgraph_uid=689, symbol_order=737)

FILE* safe_fopen_output(const char*, const char*) (const char * path, const char * mode)
{
  struct ErrorContext * D.45369;
  int D.45368;
  struct FILE * fp;
  int fh;
  struct FILE * D.45363;

  D.45368 = safe_open_output (path);
  fh = D.45368;
  if (fh == -1) goto <D.45361>; else goto <D.45362>;
  <D.45361>:
  D.45363 = 0B;
  goto <D.45367>;
  <D.45362>:
  fp = fdopen (fh, mode);
  if (fp == 0B) goto <D.45364>; else goto <D.45365>;
  <D.45364>:
  D.45369 = pbzip2::ErrorContext::getInstance ();
  _1 = D.45369;
  pbzip2::ErrorContext::saveError (_1);
  close (fh);
  goto <D.45366>;
  <D.45365>:
  <D.45366>:
  D.45363 = fp;
  goto <D.45367>;
  <D.45367>:
  return D.45363;
}



;; Function int do_remove(const char*) (_Z9do_removePKc, funcdef_no=1953, decl_uid=39240, cgraph_uid=694, symbol_order=742)

int do_remove(const char*) (const char * pathname)
{
  struct ErrorContext * D.45375;
  int ret;
  int D.45373;

  ret = remove (pathname);
  if (ret == -1) goto <D.45370>; else goto <D.45371>;
  <D.45370>:
  D.45375 = pbzip2::ErrorContext::getInstance ();
  _1 = D.45375;
  pbzip2::ErrorContext::saveError (_1);
  goto <D.45372>;
  <D.45371>:
  <D.45372>:
  D.45373 = ret;
  goto <D.45374>;
  <D.45374>:
  return D.45373;
}



;; Function bool check_file_exists(const char*) (_Z17check_file_existsPKc, funcdef_no=1954, decl_uid=39244, cgraph_uid=695, symbol_order=743)

bool check_file_exists(const char*) (const char * filename)
{
  struct ErrorContext * D.45381;
  int D.45380;
  int hOutfile;
  bool D.45378;

  D.45380 = open (filename, 0);
  hOutfile = D.45380;
  if (hOutfile == -1) goto <D.45376>; else goto <D.45377>;
  <D.45376>:
  D.45381 = pbzip2::ErrorContext::getInstance ();
  _1 = D.45381;
  pbzip2::ErrorContext::saveError (_1);
  D.45378 = 0;
  goto <D.45379>;
  <D.45377>:
  close (hOutfile);
  D.45378 = 1;
  goto <D.45379>;
  <D.45379>:
  return D.45378;
}



;; Function int handle_error(ExitFlag, int, const char*, ...) (_Z12handle_error8ExitFlagiPKcz, funcdef_no=1964, decl_uid=26096, cgraph_uid=705, symbol_order=753)

int handle_error(ExitFlag, int, const char*, ...) (ExitFlag exitFlag, int exitCode, const char * fmt)
{
  struct ErrorContext * D.45389;
  struct  args[1];
  int D.45387;

  __builtin_va_start (&args, 0);
  stderr.17_1 = stderr;
  vfprintf (stderr.17_1, fmt, &args);
  D.45389 = pbzip2::ErrorContext::getInstance ();
  _2 = D.45389;
  stderr.18_3 = stderr;
  pbzip2::ErrorContext::printErrorMessages (_2, stderr.18_3);
  stderr.19_4 = stderr;
  fflush (stderr.19_4);
  __builtin_va_end (&args);
  if (exitFlag == 2) goto <D.45382>; else goto <D.45383>;
  <D.45382>:
  syncSetTerminateFlag (1);
  abort ();
  <D.45383>:
  if (exitFlag == 1) goto <D.45384>; else goto <D.45385>;
  <D.45384>:
  syncSetTerminateFlag (1);
  goto <D.45386>;
  <D.45385>:
  <D.45386>:
  D.45387 = exitCode;
  goto <D.45391>;
  <D.45391>:
  args = {CLOBBER};
  goto <D.45388>;
  <D.45388>:
  return D.45387;
  <D.45390>:
  args = {CLOBBER};
  resx 1
}



;; Function int vfprintf(FILE*, const char*, __va_list_tag*) (vfprintf, funcdef_no=931, decl_uid=1158, cgraph_uid=265, symbol_order=267)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
int vfprintf(FILE*, const char*, __va_list_tag*) (struct FILE * restrict __stream, const char * restrict __fmt, struct  * __ap)
{
  int D.45394;
  int D.45392;

  D.45394 = __vfprintf_chk (__stream, 1, __fmt, __ap);
  D.45392 = D.45394;
  goto <D.45393>;
  <D.45393>:
  return D.45392;
}



;; Function void syncSetTerminateFlag(int) (_Z20syncSetTerminateFlagi, funcdef_no=1958, decl_uid=38968, cgraph_uid=699, symbol_order=747)

void syncSetTerminateFlag(int) (int newValue)
{
  safe_mutex_lock (&TerminateFlagMutex);
  terminateFlag = newValue;
  terminateFlag.20_1 = terminateFlag;
  if (terminateFlag.20_1 != 0) goto <D.45395>; else goto <D.45396>;
  <D.45395>:
  pthread_cond_signal (&TerminateCond);
  safe_mutex_unlock (&TerminateFlagMutex);
  OutMutex.21_2 = OutMutex;
  safe_mutex_lock (OutMutex.21_2);
  notTooMuchNumBuffered.22_3 = notTooMuchNumBuffered;
  pthread_cond_broadcast (notTooMuchNumBuffered.22_3);
  OutMutex.23_4 = OutMutex;
  safe_mutex_unlock (OutMutex.23_4);
  FifoQueue.24_5 = FifoQueue;
  if (FifoQueue.24_5 != 0B) goto <D.45397>; else goto <D.45398>;
  <D.45397>:
  FifoQueue.25_6 = FifoQueue;
  _7 = FifoQueue.25_6->mut;
  safe_mutex_lock (_7);
  FifoQueue.26_8 = FifoQueue;
  _9 = FifoQueue.26_8->notFull;
  pthread_cond_broadcast (_9);
  FifoQueue.27_10 = FifoQueue;
  _11 = FifoQueue.27_10->notEmpty;
  pthread_cond_broadcast (_11);
  FifoQueue.28_12 = FifoQueue;
  _13 = FifoQueue.28_12->mut;
  safe_mutex_unlock (_13);
  goto <D.45399>;
  <D.45398>:
  <D.45399>:
  goto <D.45400>;
  <D.45396>:
  safe_mutex_unlock (&TerminateFlagMutex);
  <D.45400>:
  return;
}



;; Function void safe_mutex_lock(pthread_mutex_t*) (_Z15safe_mutex_lockP15pthread_mutex_t, funcdef_no=1939, decl_uid=38982, cgraph_uid=680, symbol_order=728)

void safe_mutex_lock(pthread_mutex_t*) (union pthread_mutex_t * mutex)
{
  int ret;

  ret = pthread_mutex_lock (mutex);
  if (ret != 0) goto <D.45401>; else goto <D.45402>;
  <D.45401>:
  stderr.29_1 = stderr;
  fprintf (stderr.29_1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret);
  cleanupAndQuit (-5);
  goto <D.45403>;
  <D.45402>:
  <D.45403>:
  return;
}



;; Function int fprintf(FILE*, const char*, ...) (fprintf, funcdef_no=928, decl_uid=1095, cgraph_uid=262, symbol_order=264)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
int fprintf(FILE*, const char*, ...) (struct FILE * restrict __stream, const char * restrict __fmt)
{
  int D.45406;
  int D.45404;

  D.45406 = __fprintf_chk (__stream, 1, __fmt, __builtin_va_arg_pack ());
  D.45404 = D.45406;
  goto <D.45405>;
  <D.45405>:
  return D.45404;
}



;; Function void safe_mutex_unlock(pthread_mutex_t*) (_Z17safe_mutex_unlockP15pthread_mutex_t, funcdef_no=1940, decl_uid=38984, cgraph_uid=681, symbol_order=729)

void safe_mutex_unlock(pthread_mutex_t*) (union pthread_mutex_t * mutex)
{
  int ret;

  ret = pthread_mutex_unlock (mutex);
  if (ret != 0) goto <D.45407>; else goto <D.45408>;
  <D.45407>:
  stderr.30_1 = stderr;
  fprintf (stderr.30_1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret);
  cleanupAndQuit (-6);
  goto <D.45409>;
  <D.45408>:
  <D.45409>:
  return;
}



;; Function int waitForPreviousBlock(int, int) (_Z20waitForPreviousBlockii, funcdef_no=1965, decl_uid=39053, cgraph_uid=706, symbol_order=754)

int waitForPreviousBlock(int, int) (int blockNumToWait, int errBlockNumber)
{
  int D.45422;
  int D.45414;
  bool retval.31;

  <D.45410>:
  D.45422 = syncGetTerminateFlag ();
  _1 = D.45422;
  retval.31 = _1 != 0;
  if (retval.31 != 0) goto <D.45412>; else goto <D.45413>;
  <D.45412>:
  D.45414 = -1;
  goto <D.45421>;
  <D.45413>:
  OutMutex.32_2 = OutMutex;
  safe_mutex_lock (OutMutex.32_2);
  MinErrorBlock.33_3 = MinErrorBlock;
  if (MinErrorBlock.33_3 != -1) goto <D.45415>; else goto <D.45416>;
  <D.45415>:
  MinErrorBlock.34_4 = MinErrorBlock;
  if (errBlockNumber > MinErrorBlock.34_4) goto <D.45417>; else goto <D.45418>;
  <D.45417>:
  OutMutex.35_5 = OutMutex;
  safe_mutex_unlock (OutMutex.35_5);
  D.45414 = 2;
  goto <D.45421>;
  <D.45418>:
  <D.45416>:
  NextBlockToWrite.36_6 = NextBlockToWrite;
  if (errBlockNumber <= NextBlockToWrite.36_6) goto <D.45419>; else goto <D.45420>;
  <D.45419>:
  OutMutex.37_7 = OutMutex;
  safe_mutex_unlock (OutMutex.37_7);
  D.45414 = 0;
  goto <D.45421>;
  <D.45420>:
  OutMutex.38_8 = OutMutex;
  safe_cond_timed_wait (&ErrStateChangeCond, OutMutex.38_8, 1, "waitForPreviousBlock");
  OutMutex.39_9 = OutMutex;
  safe_mutex_unlock (OutMutex.39_9);
  goto <D.45410>;
  <D.45421>:
  return D.45414;
}



;; Function int syncGetTerminateFlag() (_Z20syncGetTerminateFlagv, funcdef_no=1957, decl_uid=38966, cgraph_uid=698, symbol_order=746)

int syncGetTerminateFlag() ()
{
  int ret;
  int D.45423;

  safe_mutex_lock (&TerminateFlagMutex);
  ret = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);
  D.45423 = ret;
  goto <D.45424>;
  <D.45424>:
  return D.45423;
}



;; Function int decompressErrCheckSingle(int, const outBuff*, int, const bz_stream&, const char*, bool) (_Z24decompressErrCheckSingleiPK7outBuffiRK9bz_streamPKcb, funcdef_no=1968, decl_uid=39070, cgraph_uid=709, symbol_order=757)

int decompressErrCheckSingle(int, const outBuff*, int, const bz_stream&, const char*, bool) (int bzret, const struct outBuff * fileData, int outSequenceNumber, const struct bz_stream & strm, const char * errmsg, bool isTrailingGarbageErr)
{
  int D.45441;
  int prevState;
  int lastGoodBlock;
  int D.45433;
  bool iftmp.41;
  bool retval.40;

  _1 = fileData->blockNumber;
  lastGoodBlock = getLastGoodBlockBeforeErr (_1, outSequenceNumber);
  if (lastGoodBlock == -1) goto <D.45427>; else goto <D.45430>;
  <D.45430>:
  _2 = isIgnoredTrailingGarbage ();
  _3 = ~_2;
  if (_3 != 0) goto <D.45427>; else goto <D.45428>;
  <D.45427>:
  iftmp.41 = 1;
  goto <D.45429>;
  <D.45428>:
  iftmp.41 = 0;
  <D.45429>:
  retval.40 = iftmp.41;
  if (retval.40 != 0) goto <D.45431>; else goto <D.45432>;
  <D.45431>:
  issueDecompressError (bzret, fileData, outSequenceNumber, strm, errmsg, -1);
  D.45433 = -1;
  goto <D.45440>;
  <D.45432>:
  _4 = fileData->blockNumber;
  syncSetLastGoodBlock (lastGoodBlock, _4);
  _5 = fileData->blockNumber;
  D.45441 = waitForPreviousBlock (lastGoodBlock, _5);
  prevState = D.45441;
  if (prevState == 0) goto <D.45434>; else goto <D.45435>;
  <D.45434>:
  if (isTrailingGarbageErr != 0) goto <D.45436>; else goto <D.45437>;
  <D.45436>:
  stderr.42_6 = stderr;
  fprintf (stderr.42_6, "pbzip2: *WARNING: Trailing garbage after EOF ignored!\n");
  D.45433 = 1;
  goto <D.45440>;
  <D.45437>:
  issueDecompressError (bzret, fileData, outSequenceNumber, strm, errmsg, -1);
  D.45433 = -1;
  goto <D.45440>;
  <D.45435>:
  if (prevState == 2) goto <D.45438>; else goto <D.45439>;
  <D.45438>:
  D.45433 = 2;
  goto <D.45440>;
  <D.45439>:
  D.45433 = -1;
  goto <D.45440>;
  <D.45440>:
  return D.45433;
}



;; Function int getLastGoodBlockBeforeErr(int, int) (_Z25getLastGoodBlockBeforeErrii, funcdef_no=1966, decl_uid=39056, cgraph_uid=707, symbol_order=755)

int getLastGoodBlockBeforeErr(int, int) (int errBlockNumber, int outSequenceNumber)
{
  int D.45444;

  if (outSequenceNumber != -1) goto <D.45442>; else goto <D.45443>;
  <D.45442>:
  D.45444 = errBlockNumber;
  goto <D.45445>;
  <D.45443>:
  D.45444 = errBlockNumber + -1;
  goto <D.45445>;
  <D.45445>:
  return D.45444;
}



;; Function bool isIgnoredTrailingGarbage() (_Z24isIgnoredTrailingGarbagev, funcdef_no=1963, decl_uid=39050, cgraph_uid=704, symbol_order=752)

bool isIgnoredTrailingGarbage() ()
{
  bool D.45446;

  IgnoreTrailingGarbageFlag.43_1 = IgnoreTrailingGarbageFlag;
  D.45446 = IgnoreTrailingGarbageFlag.43_1 != 0;
  goto <D.45447>;
  <D.45447>:
  return D.45446;
}



;; Function void syncSetLastGoodBlock(int, int) (_Z20syncSetLastGoodBlockii, funcdef_no=1960, decl_uid=38973, cgraph_uid=701, symbol_order=749)

void syncSetLastGoodBlock(int, int) (int newValue, int errBlock)
{
  bool changed;

  changed = 0;
  OutMutex.44_1 = OutMutex;
  safe_mutex_lock (OutMutex.44_1);
  LastGoodBlock.45_2 = LastGoodBlock;
  if (LastGoodBlock.45_2 == -1) goto <D.45448>; else goto <D.45450>;
  <D.45450>:
  LastGoodBlock.46_3 = LastGoodBlock;
  if (newValue < LastGoodBlock.46_3) goto <D.45448>; else goto <D.45449>;
  <D.45448>:
  LastGoodBlock = newValue;
  changed = 1;
  <D.45449>:
  MinErrorBlock.47_4 = MinErrorBlock;
  if (MinErrorBlock.47_4 == -1) goto <D.45451>; else goto <D.45453>;
  <D.45453>:
  MinErrorBlock.48_5 = MinErrorBlock;
  if (errBlock < MinErrorBlock.48_5) goto <D.45451>; else goto <D.45452>;
  <D.45451>:
  MinErrorBlock = errBlock;
  changed = 1;
  <D.45452>:
  if (changed != 0) goto <D.45454>; else goto <D.45455>;
  <D.45454>:
  safe_cond_signal (&ErrStateChangeCond);
  safe_cond_signal (&OutBufferHeadNotEmpty);
  notTooMuchNumBuffered.49_6 = notTooMuchNumBuffered;
  pthread_cond_broadcast (notTooMuchNumBuffered.49_6);
  OutMutex.50_7 = OutMutex;
  safe_mutex_unlock (OutMutex.50_7);
  FifoQueue.51_8 = FifoQueue;
  if (FifoQueue.51_8 != 0B) goto <D.45456>; else goto <D.45457>;
  <D.45456>:
  FifoQueue.52_9 = FifoQueue;
  _10 = FifoQueue.52_9->mut;
  safe_mutex_lock (_10);
  FifoQueue.53_11 = FifoQueue;
  _12 = FifoQueue.53_11->notFull;
  pthread_cond_broadcast (_12);
  FifoQueue.54_13 = FifoQueue;
  _14 = FifoQueue.54_13->notEmpty;
  pthread_cond_broadcast (_14);
  FifoQueue.55_15 = FifoQueue;
  _16 = FifoQueue.55_15->mut;
  safe_mutex_unlock (_16);
  goto <D.45458>;
  <D.45457>:
  <D.45458>:
  goto <D.45459>;
  <D.45455>:
  OutMutex.56_17 = OutMutex;
  safe_mutex_unlock (OutMutex.56_17);
  <D.45459>:
  return;
}



;; Function void safe_cond_signal(pthread_cond_t*) (_Z16safe_cond_signalP14pthread_cond_t, funcdef_no=1941, decl_uid=38989, cgraph_uid=682, symbol_order=730)

void safe_cond_signal(pthread_cond_t*) (union pthread_cond_t * cond)
{
  int ret;

  ret = pthread_cond_signal (cond);
  if (ret != 0) goto <D.45460>; else goto <D.45461>;
  <D.45460>:
  stderr.57_1 = stderr;
  fprintf (stderr.57_1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret);
  cleanupAndQuit (-7);
  goto <D.45462>;
  <D.45461>:
  <D.45462>:
  return;
}



;; Function int issueDecompressError(int, const outBuff*, int, const bz_stream&, const char*, int) (_Z20issueDecompressErroriPK7outBuffiRK9bz_streamPKci, funcdef_no=1967, decl_uid=39063, cgraph_uid=708, symbol_order=756)

int issueDecompressError(int, const outBuff*, int, const bz_stream&, const char*, int) (int bzret, const struct outBuff * fileData, int outSequenceNumber, const struct bz_stream & strm, const char * errmsg, int exitCode)
{
  int D.45463;

  _1 = strm->avail_in;
  _2 = fileData->isLastInSequence;
  _3 = (int) _2;
  _4 = fileData->blockNumber;
  handle_error (1, exitCode, "pbzip2: %s: ret=%d; block=%d; seq=%d; isLastInSeq=%d; avail_in=%d\n", errmsg, bzret, _4, outSequenceNumber, _3, _1);
  D.45463 = exitCode;
  goto <D.45464>;
  <D.45464>:
  return D.45463;
}



;; Function int decompressErrCheck(int, const outBuff*, int, const bz_stream&) (_Z18decompressErrCheckiPK7outBuffiRK9bz_stream, funcdef_no=1970, decl_uid=39075, cgraph_uid=711, symbol_order=759)

int decompressErrCheck(int, const outBuff*, int, const bz_stream&) (int bzret, const struct outBuff * fileData, int outSequenceNumber, const struct bz_stream & strm)
{
  int D.45481;
  int D.45480;
  int D.45479;
  int D.45478;
  int D.45468;
  bool retval.58;

  retval.58 = hasTrailingGarbage (bzret, fileData, strm);
  if (retval.58 != 0) goto <D.45466>; else goto <D.45467>;
  <D.45466>:
  D.45478 = decompressErrCheckSingle (bzret, fileData, outSequenceNumber, strm, "*ERROR during BZ2_bzDecompress - trailing garbage", 1);
  D.45468 = D.45478;
  goto <D.45477>;
  <D.45467>:
  _1 = bzret != 4;
  _2 = bzret != 0;
  _3 = _1 & _2;
  if (_3 != 0) goto <D.45469>; else goto <D.45470>;
  <D.45469>:
  D.45479 = decompressErrCheckSingle (bzret, fileData, outSequenceNumber, strm, "*ERROR during BZ2_bzDecompress - failure exit code", 0);
  D.45468 = D.45479;
  goto <D.45477>;
  <D.45470>:
  _4 = strm->avail_in;
  if (_4 != 0) goto <D.45471>; else goto <D.45472>;
  <D.45471>:
  D.45480 = decompressErrCheckSingle (bzret, fileData, outSequenceNumber, strm, "*ERROR unconsumed in after BZ2_bzDecompress loop", 0);
  D.45468 = D.45480;
  goto <D.45477>;
  <D.45472>:
  if (bzret != 4) goto <D.45473>; else goto <D.45474>;
  <D.45473>:
  _5 = fileData->isLastInSequence;
  if (_5 != 0) goto <D.45475>; else goto <D.45476>;
  <D.45475>:
  D.45481 = decompressErrCheckSingle (bzret, fileData, outSequenceNumber, strm, "*ERROR on decompress - last in segment reached before BZ_STREAM_END", 0);
  D.45468 = D.45481;
  goto <D.45477>;
  <D.45476>:
  <D.45474>:
  D.45468 = 0;
  goto <D.45477>;
  <D.45477>:
  return D.45468;
}



;; Function bool hasTrailingGarbage(int, const outBuff*, const bz_stream&) (_Z18hasTrailingGarbageiPK7outBuffRK9bz_stream, funcdef_no=1969, decl_uid=39079, cgraph_uid=710, symbol_order=758)

bool hasTrailingGarbage(int, const outBuff*, const bz_stream&) (int bzret, const struct outBuff * fileData, const struct bz_stream & strm)
{
  bool iftmp.59;
  bool D.45482;

  if (bzret == 4) goto <D.45487>; else goto <D.45484>;
  <D.45487>:
  _1 = strm->avail_in;
  if (_1 != 0) goto <D.45485>; else goto <D.45488>;
  <D.45488>:
  _2 = fileData->isLastInSequence;
  _3 = ~_2;
  if (_3 != 0) goto <D.45485>; else goto <D.45484>;
  <D.45485>:
  iftmp.59 = 1;
  goto <D.45486>;
  <D.45484>:
  iftmp.59 = 0;
  <D.45486>:
  D.45482 = iftmp.59;
  goto <D.45489>;
  <D.45489>:
  return D.45482;
}



;; Function int initSignalMask() (_Z14initSignalMaskv, funcdef_no=1971, decl_uid=38978, cgraph_uid=712, symbol_order=760)

int initSignalMask() ()
{
  int ret;
  int D.45493;

  ret = 0;
  ret = sigemptyset (&SignalMask);
  _1 = sigaddset (&SignalMask, 2);
  ret = ret | _1;
  _2 = sigaddset (&SignalMask, 15);
  ret = ret | _2;
  _3 = sigaddset (&SignalMask, 6);
  ret = ret | _3;
  SIG_HANDLER_QUIT_SIGNAL.60_4 = SIG_HANDLER_QUIT_SIGNAL;
  _5 = sigaddset (&SignalMask, SIG_HANDLER_QUIT_SIGNAL.60_4);
  ret = ret | _5;
  _6 = sigaddset (&SignalMask, 1);
  ret = ret | _6;
  if (ret == 0) goto <D.45490>; else goto <D.45491>;
  <D.45490>:
  ret = pthread_sigmask (0, &SignalMask, 0B);
  goto <D.45492>;
  <D.45491>:
  <D.45492>:
  D.45493 = ret;
  goto <D.45494>;
  <D.45494>:
  return D.45493;
}



;; Function int initChildThreadAttributes() (_Z25initChildThreadAttributesv, funcdef_no=1972, decl_uid=39323, cgraph_uid=713, symbol_order=761)

int initChildThreadAttributes() ()
{
  int ret;
  int D.45503;

  ret = pthread_attr_init (&ChildThreadAttributes);
  if (ret < 0) goto <D.45495>; else goto <D.45496>;
  <D.45495>:
  stderr.61_1 = stderr;
  fprintf (stderr.61_1, "Can\'t initialize thread atrributes [err=%d]! Aborting...\n", ret);
  exit (-1);
  <D.45496>:
  ChildThreadStackSize.62_2 = ChildThreadStackSize;
  if (ChildThreadStackSize.62_2 > 0) goto <D.45497>; else goto <D.45498>;
  <D.45497>:
  ChildThreadStackSize.63_3 = ChildThreadStackSize;
  _4 = (long unsigned int) ChildThreadStackSize.63_3;
  ret = pthread_attr_setstacksize (&ChildThreadAttributes, _4);
  if (ret != 0) goto <D.45499>; else goto <D.45500>;
  <D.45499>:
  stderr.64_5 = stderr;
  fprintf (stderr.64_5, "Can\'t set thread stacksize [err=%d]! Countinue with default one.\n", ret);
  goto <D.45501>;
  <D.45500>:
  <D.45501>:
  goto <D.45502>;
  <D.45498>:
  <D.45502>:
  D.45503 = ret;
  goto <D.45504>;
  <D.45504>:
  return D.45503;
}



;; Function int setupSignalHandling() (_Z19setupSignalHandlingv, funcdef_no=1973, decl_uid=38979, cgraph_uid=714, symbol_order=762)

int setupSignalHandling() ()
{
  int ret;
  int D.45508;

  ret = initSignalMask ();
  if (ret == 0) goto <D.45505>; else goto <D.45506>;
  <D.45505>:
  ret = pthread_create (&SignalHandlerThread, &ChildThreadAttributes, signalHandlerProc, 0B);
  goto <D.45507>;
  <D.45506>:
  <D.45507>:
  D.45508 = ret;
  goto <D.45509>;
  <D.45509>:
  return D.45508;
}



;; Function int setupTerminator() (_Z15setupTerminatorv, funcdef_no=1974, decl_uid=38980, cgraph_uid=715, symbol_order=763)

int setupTerminator() ()
{
  int D.45510;

  D.45510 = pthread_create (&TerminatorThread, &ChildThreadAttributes, terminatorThreadProc, 0B);
  goto <D.45511>;
  <D.45511>:
  return D.45510;
}



;; Function void cleanupUnfinishedWork() (_Z21cleanupUnfinishedWorkv, funcdef_no=1975, decl_uid=38975, cgraph_uid=716, symbol_order=764)

void cleanupUnfinishedWork() ()
{
  int ret;
  struct stat statBuf;

  unfinishedWorkCleaned.65_1 = unfinishedWorkCleaned;
  if (unfinishedWorkCleaned.65_1 != 0) goto <D.45512>; else goto <D.45513>;
  <D.45512>:
  goto <D.45533>;
  <D.45513>:
  ret = 0;
  sigInFilename.66_2 = sigInFilename;
  if (sigInFilename.66_2 == 0B) goto <D.45514>; else goto <D.45516>;
  <D.45516>:
  sigOutFilename.67_3 = sigOutFilename;
  if (sigOutFilename.67_3 == 0B) goto <D.45514>; else goto <D.45517>;
  <D.45517>:
  OutputStdOut.68_4 = OutputStdOut;
  if (OutputStdOut.68_4 == 1) goto <D.45514>; else goto <D.45515>;
  <D.45514>:
  unfinishedWorkCleaned = 1;
  goto <D.45533>;
  <D.45515>:
  QuietMode.69_5 = QuietMode;
  if (QuietMode.69_5 != 1) goto <D.45518>; else goto <D.45519>;
  <D.45518>:
  sigOutFilename.70_6 = sigOutFilename;
  stderr.71_7 = stderr;
  fprintf (stderr.71_7, "Cleanup unfinished work [Outfile: %s]...\n", sigOutFilename.70_6);
  goto <D.45520>;
  <D.45519>:
  <D.45520>:
  sigInFilename.72_8 = sigInFilename;
  ret = stat (sigInFilename.72_8, &statBuf);
  if (ret == 0) goto <D.45521>; else goto <D.45522>;
  <D.45521>:
  QuietMode.73_9 = QuietMode;
  if (QuietMode.73_9 != 1) goto <D.45523>; else goto <D.45524>;
  <D.45523>:
  sigOutFilename.74_10 = sigOutFilename;
  stderr.75_11 = stderr;
  fprintf (stderr.75_11, "Deleting output file: %s, if it exists...\n", sigOutFilename.74_10);
  goto <D.45525>;
  <D.45524>:
  <D.45525>:
  sigOutFilename.76_12 = sigOutFilename;
  ret = remove (sigOutFilename.76_12);
  if (ret != 0) goto <D.45526>; else goto <D.45527>;
  <D.45526>:
  _13 = __errno_location ();
  _14 = *_13;
  stderr.77_15 = stderr;
  pbzip2::ErrorContext::syncPrintErrnoMsg (stderr.77_15, _14);
  stderr.78_16 = stderr;
  fprintf (stderr.78_16, "pbzip2:  *WARNING: Deletion of output file (apparently) failed.\n");
  goto <D.45528>;
  <D.45527>:
  stderr.79_17 = stderr;
  fprintf (stderr.79_17, "pbzip2:  *INFO: Deletion of output file succeeded.\n");
  sigOutFilename = 0B;
  <D.45528>:
  goto <D.45529>;
  <D.45522>:
  stderr.80_18 = stderr;
  fprintf (stderr.80_18, "pbzip2:  *WARNING: Output file was not deleted since input file no longer exists.\n");
  sigOutFilename.81_19 = sigOutFilename;
  stderr.82_20 = stderr;
  fprintf (stderr.82_20, "pbzip2:  *WARNING: Output file: %s, may be incomplete!\n", sigOutFilename.81_19);
  <D.45529>:
  unfinishedWorkCleaned = 1;
  statBuf = {CLOBBER};
  goto <D.45532>;
  <D.45533>:
  statBuf = {CLOBBER};
  goto <D.45530>;
  <D.45532>:
  <D.45530>:
  return;
  <D.45531>:
  statBuf = {CLOBBER};
  resx 1
}



;; Function int stat(const char*, stat*) (*stat64, funcdef_no=1898, decl_uid=35993, cgraph_uid=639, symbol_order=641)

__attribute__((__leaf__, __gnu_inline__))
int stat(const char*, stat*) (const char * __path, struct stat * __statbuf)
{
  int D.45534;

  D.45534 = __xstat (1, __path, __statbuf);
  goto <D.45535>;
  <D.45535>:
  return D.45534;
}



;; Function void* terminatorThreadProc(void*) (terminatorThreadProc, funcdef_no=1976, decl_uid=39088, cgraph_uid=717, symbol_order=765)

void* terminatorThreadProc(void*) (void * arg)
{
  int D.45546;
  int ret;
  void * D.45544;

  ret = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret != 0) goto <D.45536>; else goto <D.45537>;
  <D.45536>:
  _1 = __errno_location ();
  _2 = *_1;
  stderr.83_3 = stderr;
  pbzip2::ErrorContext::syncPrintErrnoMsg (stderr.83_3, _2);
  stderr.84_4 = stderr;
  fprintf (stderr.84_4, "Terminator thread: pthread_mutex_lock error [%d]! Aborting...\n", ret);
  syncSetTerminateFlag (1);
  cleanupAndQuit (1);
  goto <D.45538>;
  <D.45537>:
  <D.45538>:
  <D.45539>:
  finishedFlag.85_5 = finishedFlag;
  if (finishedFlag.85_5 != 0) goto <D.39339>; else goto <D.45540>;
  <D.45540>:
  terminateFlag.86_6 = terminateFlag;
  if (terminateFlag.86_6 != 0) goto <D.39339>; else goto <D.45541>;
  <D.45541>:
  D.45546 = pthread_cond_wait (&TerminateCond, &TerminateFlagMutex);
  ret = D.45546;
  goto <D.45539>;
  <D.39339>:
  finishedFlag.87_7 = finishedFlag;
  if (finishedFlag.87_7 != 0) goto <D.45542>; else goto <D.45543>;
  <D.45542>:
  ret = pthread_mutex_unlock (&TerminateFlagMutex);
  D.45544 = 0B;
  goto <D.45545>;
  <D.45543>:
  ret = pthread_mutex_unlock (&TerminateFlagMutex);
  stderr.88_8 = stderr;
  fprintf (stderr.88_8, "Terminator thread: premature exit requested - quitting...\n");
  cleanupAndQuit (1);
  D.45544 = 0B;
  goto <D.45545>;
  <D.45545>:
  return D.45544;
}



;; Function void* signalHandlerProc(void*) (signalHandlerProc, funcdef_no=1977, decl_uid=39086, cgraph_uid=718, symbol_order=766)

void* signalHandlerProc(void*) (void * arg)
{
  int D.45554;
  int ret;
  int signalCaught;
  void * D.45552;

  D.45554 = sigwait (&SignalMask, &signalCaught);
  ret = D.45554;
  if (ret != 0) goto <D.45547>; else goto <D.45548>;
  <D.45547>:
  stderr.89_1 = stderr;
  fprintf (stderr.89_1, "\n *signalHandlerProc - sigwait error: %d\n", ret);
  goto <D.45549>;
  <D.45548>:
  signalCaught.90_2 = signalCaught;
  SIG_HANDLER_QUIT_SIGNAL.91_3 = SIG_HANDLER_QUIT_SIGNAL;
  if (signalCaught.90_2 == SIG_HANDLER_QUIT_SIGNAL.91_3) goto <D.45550>; else goto <D.45551>;
  <D.45550>:
  D.45552 = 0B;
  goto <D.45556>;
  <D.45551>:
  signalCaught.92_4 = signalCaught;
  stderr.93_5 = stderr;
  fprintf (stderr.93_5, "\n *Control-C or similar caught [sig=%d], quitting...\n", signalCaught.92_4);
  syncSetTerminateFlag (1);
  <D.45549>:
  D.45552 = 0B;
  goto <D.45556>;
  <D.45556>:
  signalCaught = {CLOBBER};
  goto <D.45553>;
  <D.45553>:
  return D.45552;
  <D.45555>:
  signalCaught = {CLOBBER};
  resx 1
}



;; Function void cleanupAndQuit(int) (_Z14cleanupAndQuiti, funcdef_no=1978, decl_uid=38977, cgraph_uid=719, symbol_order=767)

void cleanupAndQuit(int) (int exitCode)
{
  int ret;

  ret = pthread_mutex_lock (&ErrorHandlerMutex);
  if (ret != 0) goto <D.45557>; else goto <D.45558>;
  <D.45557>:
  stderr.94_1 = stderr;
  fprintf (stderr.94_1, "Cleanup Handler: Failed to lock ErrorHandlerMutex! May double cleanup...\n");
  goto <D.45559>;
  <D.45558>:
  <D.45559>:
  cleanupUnfinishedWork ();
  pthread_mutex_unlock (&ErrorHandlerMutex);
  exit (exitCode);
}



;; Function int producerDecompressCheckInterrupt(int, outBuff*&, int) (_Z32producerDecompressCheckInterruptiRP7outBuffi, funcdef_no=1980, decl_uid=39083, cgraph_uid=720, symbol_order=768)

int producerDecompressCheckInterrupt(int, outBuff*&, int) (int hInfile, struct outBuff * & fileData, int lastBlock)
{
  int D.45573;
  int D.45572;
  bool isInterrupted;
  int D.45570;
  bool retval.96;
  bool retval.95;

  isInterrupted = 0;
  D.45572 = syncGetLastGoodBlock ();
  _1 = D.45572;
  retval.95 = _1 != -1;
  if (retval.95 != 0) goto <D.45561>; else goto <D.45562>;
  <D.45561>:
  isInterrupted = 1;
  goto <D.45563>;
  <D.45562>:
  <D.45563>:
  D.45573 = syncGetTerminateFlag ();
  _2 = D.45573;
  retval.96 = _2 != 0;
  if (retval.96 != 0) goto <D.45565>; else goto <D.45566>;
  <D.45565>:
  isInterrupted = 1;
  goto <D.45567>;
  <D.45566>:
  <D.45567>:
  if (isInterrupted != 0) goto <D.45568>; else goto <D.45569>;
  <D.45568>:
  close (hInfile);
  disposeMemorySingle<outBuff> (fileData);
  D.45570 = 1;
  goto <D.45571>;
  <D.45569>:
  D.45570 = 0;
  goto <D.45571>;
  <D.45571>:
  return D.45570;
}



;; Function int syncGetLastGoodBlock() (_Z20syncGetLastGoodBlockv, funcdef_no=1961, decl_uid=38974, cgraph_uid=702, symbol_order=750)

int syncGetLastGoodBlock() ()
{
  int ret;
  int D.45574;

  OutMutex.97_1 = OutMutex;
  safe_mutex_lock (OutMutex.97_1);
  ret = LastGoodBlock;
  OutMutex.98_2 = OutMutex;
  safe_mutex_unlock (OutMutex.98_2);
  D.45574 = ret;
  goto <D.45575>;
  <D.45575>:
  return D.45574;
}



;; Function void disposeMemorySingle(C*&) [with C = outBuff] (_Z19disposeMemorySingleI7outBuffEvRPT_, funcdef_no=2253, decl_uid=39369, cgraph_uid=993, symbol_order=1041)

void disposeMemorySingle(C*&) [with C = outBuff] (struct outBuff * & pBuff)
{
  _1 = *pBuff;
  if (_1 != 0B) goto <D.45576>; else goto <D.45577>;
  <D.45576>:
  _2 = *pBuff;
  operator delete (_2, 40);
  *pBuff = 0B;
  goto <D.45578>;
  <D.45577>:
  <D.45578>:
  return;
}



;; Function int producer_decompress(int, off_t, queue*) (_Z19producer_decompressilP5queue, funcdef_no=1981, decl_uid=39006, cgraph_uid=721, symbol_order=769)

int producer_decompress(int, off_t, queue*) (int hInfile, off_t fileSize, struct queue * fifo)
{
  struct outBuff * D.45617;
  int D.45616;
  int D.45615;
  struct outBuff * D.45614;
  struct outBuff * fileData;
  struct BZ2StreamScanner bz2StreamScanner;
  bool iftmp.118;
  bool retval.117;
  bool retval.116;
  bool retval.109;
  int D.45590;
  bool retval.102;
  bool iftmp.100;
  bool retval.99;

  safe_mutex_lock (&ProgressIndicatorsMutex);
  InBytesProduced = 0;
  NumBlocks = 0;
  safe_mutex_unlock (&ProgressIndicatorsMutex);
  pbzip2::BZ2StreamScanner::BZ2StreamScanner (&bz2StreamScanner, hInfile, 1048576);
  D.45614 = pbzip2::BZ2StreamScanner::getNextStream (&bz2StreamScanner);
  _1 = D.45614;
  fileData = _1;
  <D.45579>:
  _2 = pbzip2::BZ2StreamScanner::failed (&bz2StreamScanner);
  _3 = ~_2;
  if (_3 != 0) goto <D.45584>; else goto <D.45582>;
  <D.45584>:
  fileData.101_4 = fileData;
  _5 = fileData.101_4->bufSize;
  if (_5 != 0) goto <D.45585>; else goto <D.45582>;
  <D.45585>:
  iftmp.100 = 1;
  goto <D.45583>;
  <D.45582>:
  iftmp.100 = 0;
  <D.45583>:
  retval.99 = iftmp.100;
  if (retval.99 != 0) goto <D.45586>; else goto <D.39378>;
  <D.45586>:
  NumBlocks.103_6 = NumBlocks;
  D.45615 = producerDecompressCheckInterrupt (hInfile, &fileData, NumBlocks.103_6);
  _7 = D.45615;
  retval.102 = _7 != 0;
  if (retval.102 != 0) goto <D.45588>; else goto <D.45589>;
  <D.45588>:
  _8 = fifo->mut;
  safe_mutex_lock (_8);
  _9 = fifo->notEmpty;
  safe_cond_broadcast (_9);
  _10 = fifo->mut;
  safe_mutex_unlock (_10);
  syncSetProducerDone (1);
  D.45590 = 0;
  goto <D.45619>;
  <D.45589>:
  QuietMode.104_11 = QuietMode;
  if (QuietMode.104_11 != 1) goto <D.45591>; else goto <D.45592>;
  <D.45591>:
  fileData.105_12 = fileData;
  _13 = fileData.105_12->bufSize;
  if (_13 > 250000000) goto <D.45593>; else goto <D.45594>;
  <D.45593>:
  fileData.106_14 = fileData;
  _15 = fileData.106_14->bufSize;
  _16 = (long unsigned int) _15;
  stderr.107_17 = stderr;
  fprintf (stderr.107_17, "pbzip2:  *WARNING: Compressed block size is large [%lu bytes].\n", _16);
  stderr.108_18 = stderr;
  fprintf (stderr.108_18, "          If program aborts, use regular BZIP2 to decompress.\n");
  goto <D.45595>;
  <D.45594>:
  <D.45595>:
  goto <D.45596>;
  <D.45592>:
  <D.45596>:
  _19 = fifo->mut;
  safe_mutex_lock (_19);
  <D.45597>:
  _20 = fifo->full;
  if (_20 == 0) goto <D.39380>; else goto <D.45598>;
  <D.45598>:
  _21 = fifo->mut;
  _22 = fifo->notFull;
  safe_cond_wait (_22, _21);
  NumBlocks.110_23 = NumBlocks;
  D.45616 = producerDecompressCheckInterrupt (hInfile, &fileData, NumBlocks.110_23);
  _24 = D.45616;
  retval.109 = _24 != 0;
  if (retval.109 != 0) goto <D.45600>; else goto <D.45601>;
  <D.45600>:
  _25 = fifo->notEmpty;
  safe_cond_broadcast (_25);
  syncSetProducerDone (1);
  _26 = fifo->mut;
  safe_mutex_unlock (_26);
  D.45590 = 0;
  goto <D.45619>;
  <D.45601>:
  goto <D.45597>;
  <D.39380>:
  fileData.111_27 = fileData;
  queue::add (fifo, fileData.111_27);
  _28 = fifo->notEmpty;
  safe_cond_signal (_28);
  safe_mutex_lock (&ProgressIndicatorsMutex);
  fileData.112_29 = fileData;
  _30 = fileData.112_29->bufSize;
  _31 = (long int) _30;
  InBytesProduced.113_32 = InBytesProduced;
  _33 = _31 + InBytesProduced.113_32;
  InBytesProduced = _33;
  fileData.114_34 = fileData;
  _35 = fileData.114_34->blockNumber;
  _36 = _35 + 1;
  NumBlocks = _36;
  safe_mutex_unlock (&ProgressIndicatorsMutex);
  _37 = fifo->mut;
  safe_mutex_unlock (_37);
  D.45617 = pbzip2::BZ2StreamScanner::getNextStream (&bz2StreamScanner);
  _38 = D.45617;
  fileData = _38;
  goto <D.45579>;
  <D.39378>:
  close (hInfile);
  fileData.115_39 = fileData;
  operator delete (fileData.115_39, 40);
  retval.116 = pbzip2::BZ2StreamScanner::failed (&bz2StreamScanner);
  if (retval.116 != 0) goto <D.45603>; else goto <D.45604>;
  <D.45603>:
  handle_error (1, 1, "pbzip2: producer_decompress: *ERROR: when reading bzip2 input stream\n");
  D.45590 = -1;
  goto <D.45619>;
  <D.45604>:
  _40 = pbzip2::BZ2StreamScanner::isBz2HeaderFound (&bz2StreamScanner);
  _41 = ~_40;
  if (_41 != 0) goto <D.45607>; else goto <D.45610>;
  <D.45610>:
  _42 = pbzip2::BZ2StreamScanner::eof (&bz2StreamScanner);
  _43 = ~_42;
  if (_43 != 0) goto <D.45607>; else goto <D.45608>;
  <D.45607>:
  iftmp.118 = 1;
  goto <D.45609>;
  <D.45608>:
  iftmp.118 = 0;
  <D.45609>:
  retval.117 = iftmp.118;
  if (retval.117 != 0) goto <D.45611>; else goto <D.45612>;
  <D.45611>:
  handle_error (1, 1, "pbzip2: producer_decompress: *ERROR: input file is not a valid bzip2 stream\n");
  D.45590 = -1;
  goto <D.45619>;
  <D.45612>:
  syncSetProducerDone (1);
  _44 = fifo->mut;
  safe_mutex_lock (_44);
  _45 = fifo->notEmpty;
  safe_cond_broadcast (_45);
  _46 = fifo->mut;
  safe_mutex_unlock (_46);
  D.45590 = 0;
  goto <D.45619>;
  <D.45619>:
  pbzip2::BZ2StreamScanner::~BZ2StreamScanner (&bz2StreamScanner);
  goto <D.45621>;
  <D.45621>:
  bz2StreamScanner = {CLOBBER};
  fileData = {CLOBBER};
  goto <D.45613>;
  <D.45613>:
  return D.45590;
  <D.45618>:
  pbzip2::BZ2StreamScanner::~BZ2StreamScanner (&bz2StreamScanner);
  resx 2
  <D.45620>:
  bz2StreamScanner = {CLOBBER};
  fileData = {CLOBBER};
  resx 1
}



;; Function void safe_cond_wait(pthread_cond_t*, pthread_mutex_t*) (_Z14safe_cond_waitP14pthread_cond_tP15pthread_mutex_t, funcdef_no=1943, decl_uid=38987, cgraph_uid=684, symbol_order=732)

void safe_cond_wait(pthread_cond_t*, pthread_mutex_t*) (union pthread_cond_t * cond, union pthread_mutex_t * mutex)
{
  int D.45625;
  int ret;

  D.45625 = pthread_cond_wait (cond, mutex);
  ret = D.45625;
  if (ret != 0) goto <D.45622>; else goto <D.45623>;
  <D.45622>:
  stderr.119_1 = stderr;
  fprintf (stderr.119_1, "pthread_cond_wait error [%d]! Aborting immediately!\n", ret);
  pthread_mutex_unlock (mutex);
  cleanupAndQuit (-8);
  goto <D.45624>;
  <D.45623>:
  <D.45624>:
  return;
}



;; Function void queue::add(queue::ElementTypePtr) (_ZN5queue3addEP7outBuff, funcdef_no=1076, decl_uid=26062, cgraph_uid=364, symbol_order=366)

void queue::add(queue::ElementTypePtr) (struct queue * const this, struct ElementType * element)
{
  _1 = element->sequenceNumber;
  if (_1 > 1) goto <D.45626>; else goto <D.45627>;
  <D.45626>:
  _2 = this->lastElement;
  _2->next = element;
  goto <D.45628>;
  <D.45627>:
  _3 = this->qData;
  _4 = this->tail;
  _5 = (long unsigned int) _4;
  _6 = _5 * 8;
  _7 = _3 + _6;
  *_7 = element;
  _8 = this->tail;
  _9 = _8 + 1;
  this->tail = _9;
  _10 = this->tail;
  _11 = this->size;
  if (_10 == _11) goto <D.45629>; else goto <D.45630>;
  <D.45629>:
  this->tail = 0;
  goto <D.45631>;
  <D.45630>:
  <D.45631>:
  _12 = this->tail;
  _13 = this->head;
  if (_12 == _13) goto <D.45632>; else goto <D.45633>;
  <D.45632>:
  this->topLevelFull = 1;
  goto <D.45634>;
  <D.45633>:
  <D.45634>:
  this->topLevelEmpty = 0;
  <D.45628>:
  this->lastElement = element;
  _14 = this->count;
  _15 = _14 + 1;
  this->count = _15;
  _16 = this->count;
  _17 = this->size;
  if (_16 == _17) goto <D.45635>; else goto <D.45636>;
  <D.45635>:
  this->full = 1;
  goto <D.45637>;
  <D.45636>:
  <D.45637>:
  this->empty = 0;
  return;
}



;; Function bool pbzip2::BZ2StreamScanner::failed() (_ZN6pbzip216BZ2StreamScanner6failedEv, funcdef_no=1431, decl_uid=29323, cgraph_uid=440, symbol_order=442)

bool pbzip2::BZ2StreamScanner::failed() (struct BZ2StreamScanner * const this)
{
  bool D.45638;

  _1 = this->_errState;
  D.45638 = _1 != 0;
  goto <D.45639>;
  <D.45639>:
  return D.45638;
}



;; Function bool pbzip2::BZ2StreamScanner::isBz2HeaderFound() const (_ZNK6pbzip216BZ2StreamScanner16isBz2HeaderFoundEv, funcdef_no=1432, decl_uid=29325, cgraph_uid=441, symbol_order=443)

bool pbzip2::BZ2StreamScanner::isBz2HeaderFound() const (const struct BZ2StreamScanner * const this)
{
  bool D.45640;

  D.45640 = this->_bz2HeaderFound;
  goto <D.45641>;
  <D.45641>:
  return D.45640;
}



;; Function bool pbzip2::BZ2StreamScanner::eof() const (_ZNK6pbzip216BZ2StreamScanner3eofEv, funcdef_no=1434, decl_uid=29329, cgraph_uid=443, symbol_order=445)

bool pbzip2::BZ2StreamScanner::eof() const (const struct BZ2StreamScanner * const this)
{
  bool D.45642;

  D.45642 = this->_eof;
  goto <D.45643>;
  <D.45643>:
  return D.45642;
}



;; Function void syncSetProducerDone(int) (_Z19syncSetProducerDonei, funcdef_no=1956, decl_uid=38965, cgraph_uid=697, symbol_order=745)

void syncSetProducerDone(int) (int newValue)
{
  ProducerDoneMutex.120_1 = ProducerDoneMutex;
  safe_mutex_lock (ProducerDoneMutex.120_1);
  producerDone = newValue;
  ProducerDoneMutex.121_2 = ProducerDoneMutex;
  safe_mutex_unlock (ProducerDoneMutex.121_2);
  return;
}



;; Function void safe_cond_broadcast(pthread_cond_t*) (_Z19safe_cond_broadcastP14pthread_cond_t, funcdef_no=1942, decl_uid=39108, cgraph_uid=683, symbol_order=731)

void safe_cond_broadcast(pthread_cond_t*) (union pthread_cond_t * cond)
{
  int ret;

  ret = pthread_cond_broadcast (cond);
  if (ret != 0) goto <D.45644>; else goto <D.45645>;
  <D.45644>:
  stderr.122_1 = stderr;
  fprintf (stderr.122_1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret);
  cleanupAndQuit (-7);
  goto <D.45646>;
  <D.45645>:
  <D.45646>:
  return;
}



;; Function int consumerDecompressCheckInterrupt(const outBuff*) (_Z32consumerDecompressCheckInterruptPK7outBuff, funcdef_no=1982, decl_uid=39383, cgraph_uid=722, symbol_order=770)

int consumerDecompressCheckInterrupt(const outBuff*) (const struct outBuff * lastElement)
{
  int D.45662;
  int D.45661;
  int minErrBlock;
  bool isInterrupted;
  int D.45659;
  bool retval.123;

  isInterrupted = 0;
  D.45661 = syncGetTerminateFlag ();
  _1 = D.45661;
  retval.123 = _1 != 0;
  if (retval.123 != 0) goto <D.45648>; else goto <D.45649>;
  <D.45648>:
  isInterrupted = 1;
  goto <D.45650>;
  <D.45649>:
  <D.45650>:
  D.45662 = syncGetMinErrorBlock ();
  minErrBlock = D.45662;
  if (minErrBlock != -1) goto <D.45653>; else goto <D.45654>;
  <D.45653>:
  if (lastElement == 0B) goto <D.45651>; else goto <D.45655>;
  <D.45655>:
  _2 = lastElement->blockNumber;
  if (minErrBlock <= _2) goto <D.45651>; else goto <D.45656>;
  <D.45656>:
  _3 = lastElement->isLastInSequence;
  if (_3 != 0) goto <D.45651>; else goto <D.45652>;
  <D.45651>:
  isInterrupted = 1;
  <D.45652>:
  <D.45654>:
  if (isInterrupted != 0) goto <D.45657>; else goto <D.45658>;
  <D.45657>:
  D.45659 = 1;
  goto <D.45660>;
  <D.45658>:
  D.45659 = 0;
  goto <D.45660>;
  <D.45660>:
  return D.45659;
}



;; Function int syncGetMinErrorBlock() (_Z20syncGetMinErrorBlockv, funcdef_no=1962, decl_uid=39270, cgraph_uid=703, symbol_order=751)

int syncGetMinErrorBlock() ()
{
  int ret;
  int D.45663;

  OutMutex.124_1 = OutMutex;
  safe_mutex_lock (OutMutex.124_1);
  ret = MinErrorBlock;
  OutMutex.125_2 = OutMutex;
  safe_mutex_unlock (OutMutex.125_2);
  D.45663 = ret;
  goto <D.45664>;
  <D.45664>:
  return D.45663;
}



;; Function void* consumer_decompress(void*) (consumer_decompress, funcdef_no=1983, decl_uid=39090, cgraph_uid=723, symbol_order=771)

void* consumer_decompress(void*) (void * q)
{
  int D.45754;
  int D.45753;
  struct outBuff * D.45751;
  struct outBuff * D.45750;
  int D.45749;
  int D.45748;
  int D.45747;
  int D.45746;
  int D.45745;
  int D.45744;
  struct outBuff * nextOutBlock;
  bool isLast;
  unsigned int len;
  struct outBuff * addret;
  unsigned int availIn;
  int bzret;
  struct bz_stream strm;
  int errState;
  unsigned int processedIn;
  int outSequenceNumber;
  struct outBuff * prevOutBlockInSequence;
  unsigned int outSize;
  char * DecompressedData;
  struct outBuff * lastFileData;
  struct outBuff * fileData;
  struct queue * fifo;
  struct outBuff D.39440;
  int iftmp.149;
  struct outBuff * iftmp.147;
  void * D.39439;
  bool retval.143;
  sizetype D.45709;
  bool iftmp.133;
  bool retval.132;
  bool iftmp.131;
  bool retval.130;
  bool iftmp.129;
  bool retval.128;
  void * D.45670;
  bool retval.126;

  fifo = q;
  fileData = 0B;
  lastFileData = 0B;
  DecompressedData = 0B;
  outSize = 0;
  prevOutBlockInSequence = 0B;
  outSequenceNumber = -1;
  processedIn = 0;
  errState = 0;
  strm.bzalloc = 0B;
  strm.bzfree = 0B;
  strm.opaque = 0B;
  <D.45665>:
  _1 = fifo->mut;
  safe_mutex_lock (_1);
  <D.45666>:
  fileData.127_2 = fileData;
  D.45744 = consumerDecompressCheckInterrupt (fileData.127_2);
  _3 = D.45744;
  retval.126 = _3 != 0;
  if (retval.126 != 0) goto <D.45668>; else goto <D.45669>;
  <D.45668>:
  _4 = fifo->mut;
  safe_mutex_unlock (_4);
  D.45670 = 0B;
  goto <D.45756>;
  <D.45669>:
  _5 = fifo->empty;
  if (_5 == 0) goto <D.45675>; else goto <D.45673>;
  <D.45675>:
  _6 = queue::remove (fifo, &fileData);
  if (_6 == 1) goto <D.45676>; else goto <D.45673>;
  <D.45676>:
  iftmp.129 = 1;
  goto <D.45674>;
  <D.45673>:
  iftmp.129 = 0;
  <D.45674>:
  retval.128 = iftmp.129;
  if (retval.128 != 0) goto <D.39445>; else goto <D.45677>;
  <D.45677>:
  _7 = fifo->empty;
  if (_7 != 0) goto <D.45683>; else goto <D.45680>;
  <D.45683>:
  D.45745 = syncGetProducerDone ();
  _8 = D.45745;
  if (_8 == 1) goto <D.45681>; else goto <D.45684>;
  <D.45684>:
  D.45746 = syncGetTerminateFlag ();
  _9 = D.45746;
  if (_9 != 0) goto <D.45681>; else goto <D.45680>;
  <D.45681>:
  iftmp.131 = 1;
  goto <D.45682>;
  <D.45680>:
  iftmp.131 = 0;
  <D.45682>:
  retval.130 = iftmp.131;
  if (retval.130 != 0) goto <D.45685>; else goto <D.45686>;
  <D.45685>:
  _10 = fifo->mut;
  pthread_mutex_unlock (_10);
  D.45747 = syncGetTerminateFlag ();
  _11 = D.45747;
  if (_11 == 0) goto <D.45691>; else goto <D.45689>;
  <D.45691>:
  if (outSequenceNumber != -1) goto <D.45692>; else goto <D.45689>;
  <D.45692>:
  iftmp.133 = 1;
  goto <D.45690>;
  <D.45689>:
  iftmp.133 = 0;
  <D.45690>:
  retval.132 = iftmp.133;
  if (retval.132 != 0) goto <D.45693>; else goto <D.45694>;
  <D.45693>:
  lastFileData.134_12 = lastFileData;
  _13 = lastFileData.134_12->sequenceNumber;
  lastFileData.135_14 = lastFileData;
  _15 = lastFileData.135_14->blockNumber;
  handle_error (1, -1, "pbzip2: *ERROR on decompress - premature end of archive stream (block=%d; seq=%d; outseq=%d)!\n", _15, _13, outSequenceNumber);
  goto <D.45695>;
  <D.45694>:
  <D.45695>:
  disposeMemorySingle<outBuff> (&lastFileData);
  D.45670 = 0B;
  goto <D.45756>;
  <D.45686>:
  _16 = fifo->mut;
  _17 = fifo->notEmpty;
  safe_cond_wait (_17, _16);
  goto <D.45666>;
  <D.39445>:
  _18 = fifo->notFull;
  safe_cond_signal (_18);
  _19 = fifo->mut;
  safe_mutex_unlock (_19);
  lastFileData.136_20 = lastFileData;
  if (lastFileData.136_20 != 0B) goto <D.45696>; else goto <D.45697>;
  <D.45696>:
  lastFileData.137_21 = lastFileData;
  operator delete (lastFileData.137_21, 40);
  goto <D.45698>;
  <D.45697>:
  <D.45698>:
  fileData.138_22 = fileData;
  lastFileData = fileData.138_22;
  outSize = 900000;
  bzret = 0;
  fileData.139_23 = fileData;
  _24 = fileData.139_23->sequenceNumber;
  if (_24 <= 1) goto <D.45699>; else goto <D.45700>;
  <D.45699>:
  Verbosity.140_25 = Verbosity;
  D.45748 = BZ2_bzDecompressInit (&strm, Verbosity.140_25, 0);
  bzret = D.45748;
  if (bzret != 0) goto <D.45701>; else goto <D.45702>;
  <D.45701>:
  handle_error (1, -1, "pbzip2: *ERROR during BZ2_bzDecompressInit: %d\n", bzret);
  D.45670 = 0B;
  goto <D.45756>;
  <D.45702>:
  goto <D.45703>;
  <D.45700>:
  <D.45703>:
  fileData.141_26 = fileData;
  _27 = fileData.141_26->bufSize;
  strm.avail_in = _27;
  fileData.142_28 = fileData;
  _29 = fileData.142_28->buf;
  strm.next_in = _29;
  <D.45704>:
  if (bzret != 0) goto <D.39447>; else goto <D.45705>;
  <D.45705>:
  _30 = strm.avail_in;
  if (_30 == 0) goto <D.39447>; else goto <D.45706>;
  <D.45706>:
  if (DecompressedData == 0B) goto <D.45707>; else goto <D.45708>;
  <D.45707>:
  D.45709 = (sizetype) outSize;
  DecompressedData = operator new [] (D.45709, &nothrow);
  if (DecompressedData == 0B) goto <D.45710>; else goto <D.45711>;
  <D.45710>:
  handle_error (1, -1, " *ERROR: Could not allocate memory (DecompressedData)!  Aborting...\n");
  D.45670 = 0B;
  goto <D.45756>;
  <D.45711>:
  processedIn = 0;
  strm.avail_out = outSize;
  strm.next_out = DecompressedData;
  goto <D.45712>;
  <D.45708>:
  <D.45712>:
  availIn = strm.avail_in;
  D.45749 = BZ2_bzDecompress (&strm);
  bzret = D.45749;
  _31 = strm.avail_in;
  _32 = availIn - _31;
  processedIn = processedIn + _32;
  if (bzret == 0) goto <D.45716>; else goto <D.45713>;
  <D.45716>:
  _33 = strm.avail_out;
  if (_33 == 0) goto <D.45714>; else goto <D.45713>;
  <D.45713>:
  if (bzret == 4) goto <D.45714>; else goto <D.45715>;
  <D.45714>:
  addret = 0B;
  _34 = strm.avail_out;
  len = outSize - _34;
  isLast = bzret == 4;
  fileData.144_35 = fileData;
  retval.143 = hasTrailingGarbage (bzret, fileData.144_35, &strm);
  if (retval.143 != 0) goto <D.45718>; else goto <D.45719>;
  <D.45718>:
  fileData.145_36 = fileData;
  _37 = fileData.145_36->blockNumber;
  fileData.146_38 = fileData;
  _39 = fileData.146_38->blockNumber;
  syncSetLastGoodBlock (_39, _37);
  goto <D.45720>;
  <D.45719>:
  <D.45720>:
  if (outSequenceNumber > 0) goto <D.45721>; else goto <D.45722>;
  <D.45721>:
  outSequenceNumber = outSequenceNumber + 1;
  D.39439 = operator new (40, &nothrow);
  if (D.39439 != 0B) goto <D.45724>; else goto <D.45725>;
  <D.45724>:
  _40 = (int) isLast;
  fileData.148_41 = fileData;
  _42 = fileData.148_41->blockNumber;
  outBuff::outBuff (D.39439, DecompressedData, len, _42, outSequenceNumber, processedIn, _40, 0B);
  iftmp.147 = D.39439;
  goto <D.45726>;
  <D.45725>:
  iftmp.147 = D.39439;
  <D.45726>:
  nextOutBlock = iftmp.147;
  if (nextOutBlock == 0B) goto <D.45727>; else goto <D.45728>;
  <D.45727>:
  BZ2_bzDecompressEnd (&strm);
  handle_error (1, -1, " *ERROR: Could not allocate memory (nextOutBlock)!  Aborting...\n");
  D.45670 = 0B;
  goto <D.45756>;
  <D.45728>:
  D.45750 = outputBufferSeqAddNext (prevOutBlockInSequence, nextOutBlock);
  addret = D.45750;
  goto <D.45729>;
  <D.45722>:
  if (bzret == 0) goto <D.45731>; else goto <D.45732>;
  <D.45731>:
  iftmp.149 = 1;
  goto <D.45733>;
  <D.45732>:
  iftmp.149 = 0;
  <D.45733>:
  outSequenceNumber = iftmp.149;
  _43 = (int) isLast;
  fileData.150_44 = fileData;
  _45 = fileData.150_44->blockNumber;
  outBuff::outBuff (&D.39440, DecompressedData, len, _45, outSequenceNumber, processedIn, _43, 0B);
  D.45751 = outputBufferAdd (&D.39440, "consumer_decompress");
  addret = D.45751;
  D.39440 = {CLOBBER};
  <D.45729>:
  if (addret == 0B) goto <D.45734>; else goto <D.45735>;
  <D.45734>:
  BZ2_bzDecompressEnd (&strm);
  D.45670 = 0B;
  goto <D.45756>;
  <D.45735>:
  prevOutBlockInSequence = addret;
  DecompressedData = 0B;
  <D.45715>:
  goto <D.45704>;
  <D.39447>:
  fileData.151_46 = fileData;
  D.45753 = decompressErrCheck (bzret, fileData.151_46, outSequenceNumber, &strm);
  errState = D.45753;
  if (bzret == 4) goto <D.45736>; else goto <D.45737>;
  <D.45736>:
  D.45754 = BZ2_bzDecompressEnd (&strm);
  bzret = D.45754;
  _47 = bzret != 0;
  errState.152_48 = (unsigned int) errState;
  _49 = errState.152_48 <= 1;
  _50 = _47 & _49;
  if (_50 != 0) goto <D.45738>; else goto <D.45739>;
  <D.45738>:
  handle_error (1, -1, "pbzip2: *ERROR during BZ2_bzDecompressEnd: %d\n", bzret);
  D.45670 = 0B;
  goto <D.45756>;
  <D.45739>:
  outSequenceNumber = -1;
  prevOutBlockInSequence = 0B;
  goto <D.45740>;
  <D.45737>:
  <D.45740>:
  fileData.153_51 = fileData;
  _52 = &fileData.153_51->buf;
  disposeMemory<char> (_52);
  if (errState != 0) goto <D.45741>; else goto <D.45742>;
  <D.45741>:
  D.45670 = 0B;
  goto <D.45756>;
  <D.45742>:
  goto <D.45665>;
  D.45670 = 0B;
  goto <D.45756>;
  <D.45756>:
  fileData = {CLOBBER};
  lastFileData = {CLOBBER};
  strm = {CLOBBER};
  goto <D.45743>;
  <D.45743>:
  return D.45670;
  <D.45752>:
  D.39440 = {CLOBBER};
  resx 3
  <D.45755>:
  fileData = {CLOBBER};
  lastFileData = {CLOBBER};
  strm = {CLOBBER};
  resx 1
}



;; Function int queue::remove(queue::ElementType*&) (_ZN5queue6removeERP7outBuff, funcdef_no=1077, decl_uid=26065, cgraph_uid=365, symbol_order=367)

int queue::remove(queue::ElementType*&) (struct queue * const this, struct ElementType * & element)
{
  struct ElementType * & headElem;
  int D.45764;

  _1 = this->qData;
  _2 = this->head;
  _3 = (long unsigned int) _2;
  _4 = _3 * 8;
  headElem = _1 + _4;
  _5 = *element;
  if (_5 != 0B) goto <D.45759>; else goto <D.45757>;
  <D.45759>:
  _6 = *element;
  _7 = _6->isLastInSequence;
  _8 = ~_7;
  if (_8 != 0) goto <D.45760>; else goto <D.45757>;
  <D.45760>:
  _9 = *element;
  _10 = _9->next;
  if (_10 != 0B) goto <D.45761>; else goto <D.45762>;
  <D.45761>:
  _11 = *element;
  _12 = _11->next;
  *element = _12;
  goto <D.45763>;
  <D.45762>:
  D.45764 = 0;
  goto <D.45776>;
  <D.45763>:
  goto <D.45758>;
  <D.45757>:
  _13 = this->topLevelEmpty;
  if (_13 != 0) goto <D.45765>; else goto <D.45766>;
  <D.45765>:
  D.45764 = 0;
  goto <D.45776>;
  <D.45766>:
  _14 = *headElem;
  *element = _14;
  _15 = this->head;
  _16 = _15 + 1;
  this->head = _16;
  _17 = this->head;
  _18 = this->size;
  if (_17 == _18) goto <D.45767>; else goto <D.45768>;
  <D.45767>:
  this->head = 0;
  goto <D.45769>;
  <D.45768>:
  <D.45769>:
  _19 = this->head;
  _20 = this->tail;
  if (_19 == _20) goto <D.45770>; else goto <D.45771>;
  <D.45770>:
  this->topLevelEmpty = 1;
  goto <D.45772>;
  <D.45771>:
  <D.45772>:
  this->topLevelFull = 0;
  <D.45758>:
  _21 = this->count;
  _22 = _21 + -1;
  this->count = _22;
  _23 = this->count;
  if (_23 == 0) goto <D.45773>; else goto <D.45774>;
  <D.45773>:
  this->empty = 1;
  goto <D.45775>;
  <D.45774>:
  <D.45775>:
  this->full = 0;
  D.45764 = 1;
  goto <D.45776>;
  <D.45776>:
  return D.45764;
}



;; Function int syncGetProducerDone() (_Z19syncGetProducerDonev, funcdef_no=1955, decl_uid=38963, cgraph_uid=696, symbol_order=744)

int syncGetProducerDone() ()
{
  int ret;
  int D.45777;

  ProducerDoneMutex.154_1 = ProducerDoneMutex;
  safe_mutex_lock (ProducerDoneMutex.154_1);
  ret = producerDone;
  ProducerDoneMutex.155_2 = ProducerDoneMutex;
  safe_mutex_unlock (ProducerDoneMutex.155_2);
  D.45777 = ret;
  goto <D.45778>;
  <D.45778>:
  return D.45777;
}



;; Function outBuff::outBuff(char*, unsigned int, int, int, unsigned int, bool, outBuff*) (_ZN7outBuffC2EPcjiijbPS_, funcdef_no=1070, decl_uid=26014, cgraph_uid=358, symbol_order=360)

outBuff::outBuff(char*, unsigned int, int, int, unsigned int, bool, outBuff*) (struct outBuff * const this, char * aBuf, unsigned int aBufSize, int aBlockNumber, int aSequenceNumber, unsigned int aInSize, bool isLast, struct outBuff * aNext)
{
  MEM[(struct  &)this] = {CLOBBER};
  this->buf = aBuf;
  this->bufSize = aBufSize;
  this->blockNumber = aBlockNumber;
  this->sequenceNumber = aSequenceNumber;
  this->inSize = aInSize;
  this->isLastInSequence = isLast;
  this->next = aNext;
  return;
}



;; Function void disposeMemory(C*&) [with C = char] (_Z13disposeMemoryIcEvRPT_, funcdef_no=2254, decl_uid=39441, cgraph_uid=994, symbol_order=1042)

void disposeMemory(C*&) [with C = char] (char * & pBuff)
{
  _1 = *pBuff;
  if (_1 != 0B) goto <D.45779>; else goto <D.45780>;
  <D.45779>:
  _2 = *pBuff;
  if (_2 != 0B) goto <D.45781>; else goto <D.45782>;
  <D.45781>:
  _3 = *pBuff;
  operator delete [] (_3);
  goto <D.45783>;
  <D.45782>:
  <D.45783>:
  *pBuff = 0B;
  goto <D.45784>;
  <D.45780>:
  <D.45784>:
  return;
}



;; Function void* fileWriter(void*) (fileWriter, funcdef_no=1984, decl_uid=39092, cgraph_uid=724, symbol_order=772)

void* fileWriter(void*) (void * outname)
{
  struct ErrorContext * D.45883;
  int D.45882;
  long int D.45881;
  long int D.45880;
  int D.45879;
  int D.45878;
  int D.45877;
  int percentCompleteOld;
  struct outBuff * outBlock;
  struct outBuff * prevBlockInSequence;
  off_t bytesProcessed;
  int ret;
  size_t outBufferPos;
  int currBlock;
  int hOutfile;
  int percentComplete;
  off_t CompressedSize;
  char * OutFilename;
  bool retval.172;
  bool iftmp.167;
  bool retval.166;
  bool retval.160;
  bool iftmp.158;
  bool retval.157;
  void * D.45789;

  CompressedSize = 0;
  percentComplete = 0;
  hOutfile = 1;
  currBlock = 0;
  outBufferPos = 0;
  ret = -1;
  bytesProcessed = 0;
  OutFilename = outname;
  prevBlockInSequence = 0B;
  OutputStdOut.156_1 = OutputStdOut;
  if (OutputStdOut.156_1 == 0) goto <D.45785>; else goto <D.45786>;
  <D.45785>:
  D.45877 = safe_open_output (OutFilename);
  hOutfile = D.45877;
  if (hOutfile == -1) goto <D.45787>; else goto <D.45788>;
  <D.45787>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not create output file [%s]!\n", OutFilename);
  D.45789 = 0B;
  goto <D.45876>;
  <D.45788>:
  goto <D.45790>;
  <D.45786>:
  <D.45790>:
  <D.45791>:
  D.45878 = syncGetProducerDone ();
  _2 = D.45878;
  if (_2 == 1) goto <D.45796>; else goto <D.45794>;
  <D.45796>:
  NumBlocks.159_3 = NumBlocks;
  if (currBlock >= NumBlocks.159_3) goto <D.45797>; else goto <D.45794>;
  <D.45797>:
  if (prevBlockInSequence == 0B) goto <D.45798>; else goto <D.45794>;
  <D.45798>:
  iftmp.158 = 1;
  goto <D.45795>;
  <D.45794>:
  iftmp.158 = 0;
  <D.45795>:
  retval.157 = iftmp.158;
  if (retval.157 != 0) goto <D.39463>; else goto <D.45799>;
  <D.45799>:
  D.45879 = syncGetTerminateFlag ();
  _4 = D.45879;
  retval.160 = _4 != 0;
  if (retval.160 != 0) goto <D.39463>; else goto <D.45801>;
  <D.45801>:
  OutMutex.161_5 = OutMutex;
  safe_mutex_lock (OutMutex.161_5);
  LastGoodBlock.162_6 = LastGoodBlock;
  if (LastGoodBlock.162_6 != -1) goto <D.45802>; else goto <D.45803>;
  <D.45802>:
  NextBlockToWrite.163_7 = NextBlockToWrite;
  LastGoodBlock.164_8 = LastGoodBlock;
  if (NextBlockToWrite.163_7 > LastGoodBlock.164_8) goto <D.45804>; else goto <D.45805>;
  <D.45804>:
  OutMutex.165_9 = OutMutex;
  safe_mutex_unlock (OutMutex.165_9);
  goto <D.39463>;
  <D.45805>:
  <D.45803>:
  _10 = std::vector<outBuff>::operator[] (&OutputBuffer, outBufferPos);
  _11 = _10->buf;
  if (_11 == 0B) goto <D.45811>; else goto <D.45808>;
  <D.45811>:
  if (prevBlockInSequence == 0B) goto <D.45809>; else goto <D.45812>;
  <D.45812>:
  _12 = prevBlockInSequence->next;
  if (_12 == 0B) goto <D.45809>; else goto <D.45808>;
  <D.45809>:
  iftmp.167 = 1;
  goto <D.45810>;
  <D.45808>:
  iftmp.167 = 0;
  <D.45810>:
  retval.166 = iftmp.167;
  if (retval.166 != 0) goto <D.45813>; else goto <D.45814>;
  <D.45813>:
  OutMutex.168_13 = OutMutex;
  safe_cond_timed_wait (&OutBufferHeadNotEmpty, OutMutex.168_13, 1, "fileWriter");
  OutMutex.169_14 = OutMutex;
  safe_mutex_unlock (OutMutex.169_14);
  // predicted unlikely by continue predictor.
  goto <D.39464>;
  <D.45814>:
  OutMutex.170_15 = OutMutex;
  safe_mutex_unlock (OutMutex.170_15);
  if (prevBlockInSequence != 0B) goto <D.45815>; else goto <D.45816>;
  <D.45815>:
  outBlock = prevBlockInSequence->next;
  goto <D.45817>;
  <D.45816>:
  outBlock = std::vector<outBuff>::operator[] (&OutputBuffer, outBufferPos);
  <D.45817>:
  _16 = outBlock->bufSize;
  _17 = (long unsigned int) _16;
  _18 = outBlock->buf;
  D.45880 = do_write (hOutfile, _18, _17);
  _19 = D.45880;
  ret = (int) _19;
  if (ret < 0) goto <D.45818>; else goto <D.45819>;
  <D.45818>:
  OutputStdOut.171_20 = OutputStdOut;
  if (OutputStdOut.171_20 == 0) goto <D.45820>; else goto <D.45821>;
  <D.45820>:
  close (hOutfile);
  goto <D.45822>;
  <D.45821>:
  <D.45822>:
  _21 = outBlock->bufSize;
  handle_error (1, -1, "pbzip2: *ERROR: Could not write %d bytes to file [ret=%d]!  Aborting...\n", _21, ret);
  D.45789 = 0B;
  goto <D.45876>;
  <D.45819>:
  _22 = (long int) ret;
  CompressedSize = CompressedSize + _22;
  _23 = outBlock->inSize;
  _24 = (long int) _23;
  bytesProcessed = bytesProcessed + _24;
  _25 = outBlock->buf;
  if (_25 != 0B) goto <D.45823>; else goto <D.45824>;
  <D.45823>:
  _26 = outBlock->buf;
  operator delete [] (_26);
  goto <D.45825>;
  <D.45824>:
  <D.45825>:
  outBlock->buf = 0B;
  outBlock->bufSize = 0;
  _27 = outBlock->isLastInSequence;
  if (_27 != 0) goto <D.45826>; else goto <D.45827>;
  <D.45826>:
  outBufferPos = outBufferPos + 1;
  NumBufferedBlocksMax.173_28 = NumBufferedBlocksMax;
  retval.172 = outBufferPos == NumBufferedBlocksMax.173_28;
  if (retval.172 != 0) goto <D.45829>; else goto <D.45830>;
  <D.45829>:
  outBufferPos = 0;
  goto <D.45831>;
  <D.45830>:
  <D.45831>:
  currBlock = currBlock + 1;
  goto <D.45832>;
  <D.45827>:
  <D.45832>:
  OutMutex.174_29 = OutMutex;
  safe_mutex_lock (OutMutex.174_29);
  _30 = outBlock->isLastInSequence;
  if (_30 != 0) goto <D.45833>; else goto <D.45834>;
  <D.45833>:
  NextBlockToWrite.175_31 = NextBlockToWrite;
  _32 = NextBlockToWrite.175_31 + 1;
  NextBlockToWrite = _32;
  OutBufferPosToWrite = outBufferPos;
  goto <D.45835>;
  <D.45834>:
  <D.45835>:
  _33 = outBlock->sequenceNumber;
  if (_33 > 1) goto <D.45836>; else goto <D.45837>;
  <D.45836>:
  NumBufferedTailBlocks.176_34 = NumBufferedTailBlocks;
  _35 = NumBufferedTailBlocks.176_34 + 18446744073709551615;
  NumBufferedTailBlocks = _35;
  goto <D.45838>;
  <D.45837>:
  <D.45838>:
  notTooMuchNumBuffered.177_36 = notTooMuchNumBuffered;
  safe_cond_broadcast (notTooMuchNumBuffered.177_36);
  safe_cond_broadcast (&ErrStateChangeCond);
  OutMutex.178_37 = OutMutex;
  safe_mutex_unlock (OutMutex.178_37);
  _38 = outBlock->sequenceNumber;
  if (_38 > 2) goto <D.45839>; else goto <D.45840>;
  <D.45839>:
  operator delete (prevBlockInSequence, 40);
  goto <D.45841>;
  <D.45840>:
  <D.45841>:
  _39 = outBlock->isLastInSequence;
  if (_39 != 0) goto <D.45842>; else goto <D.45843>;
  <D.45842>:
  prevBlockInSequence = 0B;
  _40 = outBlock->sequenceNumber;
  if (_40 > 1) goto <D.45844>; else goto <D.45845>;
  <D.45844>:
  operator delete (outBlock, 40);
  goto <D.45846>;
  <D.45845>:
  <D.45846>:
  goto <D.45847>;
  <D.45843>:
  prevBlockInSequence = outBlock;
  <D.45847>:
  QuietMode.179_41 = QuietMode;
  if (QuietMode.179_41 != 1) goto <D.45848>; else goto <D.45849>;
  <D.45848>:
  percentCompleteOld = percentComplete;
  InFileSize.180_42 = InFileSize;
  if (InFileSize.180_42 > 0) goto <D.45850>; else goto <D.45851>;
  <D.45850>:
  _43 = (double) bytesProcessed;
  _44 = _43 * 1.0e+2;
  InFileSize.181_45 = InFileSize;
  _46 = (double) InFileSize.181_45;
  _47 = _44 / _46;
  percentComplete = (int) _47;
  goto <D.45852>;
  <D.45851>:
  <D.45852>:
  if (percentComplete != percentCompleteOld) goto <D.45853>; else goto <D.45854>;
  <D.45853>:
  stderr.182_48 = stderr;
  fprintf (stderr.182_48, "Completed: %d%%             \r", percentComplete);
  stderr.183_49 = stderr;
  fflush (stderr.183_49);
  goto <D.45855>;
  <D.45854>:
  <D.45855>:
  goto <D.45856>;
  <D.45849>:
  <D.45856>:
  <D.39464>:
  goto <D.45791>;
  <D.39463>:
  if (currBlock == 0) goto <D.45857>; else goto <D.45858>;
  <D.45857>:
  D.45881 = do_write (hOutfile, &Bz2HeaderZero, 14);
  _50 = D.45881;
  ret = (int) _50;
  if (ret < 0) goto <D.45859>; else goto <D.45860>;
  <D.45859>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not write to file!  Aborting...\n");
  D.45789 = 0B;
  goto <D.45876>;
  <D.45860>:
  goto <D.45861>;
  <D.45858>:
  <D.45861>:
  OutputStdOut.184_51 = OutputStdOut;
  if (OutputStdOut.184_51 == 0) goto <D.45862>; else goto <D.45863>;
  <D.45862>:
  D.45882 = close (hOutfile);
  ret = D.45882;
  if (ret == -1) goto <D.45864>; else goto <D.45865>;
  <D.45864>:
  D.45883 = pbzip2::ErrorContext::getInstance ();
  _52 = D.45883;
  pbzip2::ErrorContext::saveError (_52);
  handle_error (1, -1, "pbzip2: *ERROR: Could not close output file!  Aborting...\n");
  D.45789 = 0B;
  goto <D.45876>;
  <D.45865>:
  goto <D.45866>;
  <D.45863>:
  <D.45866>:
  QuietMode.185_53 = QuietMode;
  if (QuietMode.185_53 != 1) goto <D.45867>; else goto <D.45868>;
  <D.45867>:
  CompressedSize.186_54 = (long unsigned int) CompressedSize;
  stderr.187_55 = stderr;
  fprintf (stderr.187_55, "    Output Size: %lu bytes\n", CompressedSize.186_54);
  goto <D.45869>;
  <D.45868>:
  <D.45869>:
  FifoQueue.188_56 = FifoQueue;
  if (FifoQueue.188_56 != 0B) goto <D.45870>; else goto <D.45871>;
  <D.45870>:
  FifoQueue.189_57 = FifoQueue;
  _58 = FifoQueue.189_57->mut;
  safe_mutex_lock (_58);
  FifoQueue.190_59 = FifoQueue;
  _60 = FifoQueue.190_59->notEmpty;
  safe_cond_broadcast (_60);
  FifoQueue.191_61 = FifoQueue;
  _62 = FifoQueue.191_61->notFull;
  safe_cond_broadcast (_62);
  FifoQueue.192_63 = FifoQueue;
  _64 = FifoQueue.192_63->mut;
  safe_mutex_unlock (_64);
  goto <D.45872>;
  <D.45871>:
  <D.45872>:
  OutMutex.193_65 = OutMutex;
  safe_mutex_lock (OutMutex.193_65);
  notTooMuchNumBuffered.194_66 = notTooMuchNumBuffered;
  safe_cond_broadcast (notTooMuchNumBuffered.194_66);
  OutMutex.195_67 = OutMutex;
  safe_mutex_unlock (OutMutex.195_67);
  QuietMode.196_68 = QuietMode;
  if (QuietMode.196_68 != 1) goto <D.45873>; else goto <D.45874>;
  <D.45873>:
  percentComplete = 100;
  stderr.197_69 = stderr;
  fprintf (stderr.197_69, "Completed: %d%%             \r", percentComplete);
  stderr.198_70 = stderr;
  fflush (stderr.198_70);
  goto <D.45875>;
  <D.45874>:
  <D.45875>:
  D.45789 = 0B;
  goto <D.45876>;
  <D.45876>:
  return D.45789;
}



;; Function std::vector<_Tp, _Alloc>::reference std::vector<_Tp, _Alloc>::operator[](std::vector<_Tp, _Alloc>::size_type) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EEixEm, funcdef_no=2255, decl_uid=38066, cgraph_uid=995, symbol_order=1043)

std::vector<_Tp, _Alloc>::reference std::vector<_Tp, _Alloc>::operator[](std::vector<_Tp, _Alloc>::size_type) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this, size_type __n)
{
  struct value_type & D.45884;

  _1 = this->D.38528._M_impl._M_start;
  _2 = __n * 40;
  D.45884 = _1 + _2;
  goto <D.45885>;
  <D.45885>:
  return D.45884;
}



;; Function int directcompress(int, off_t, int, const char*) (_Z14directcompressiliPKc, funcdef_no=1985, decl_uid=39011, cgraph_uid=725, symbol_order=773)

int directcompress(int, off_t, int, const char*) (int hInfile, off_t fileSize, int blockSize, const char * OutFilename)
{
  long int D.45994;
  int D.45993;
  long int D.45992;
  int D.45991;
  int D.45990;
  int percentCompleteOld;
  int ret;
  int rret;
  int currBlock;
  int hOutfile;
  int percentComplete;
  unsigned int outSize;
  off_t inSize;
  off_t bytesLeft;
  off_t CompressedSize;
  char * CompressedData;
  char * FileData;
  sizetype D.45928;
  sizetype inSize.203;
  bool retval.200;
  int D.45890;

  FileData = 0B;
  CompressedData = 0B;
  CompressedSize = 0;
  bytesLeft = 0;
  inSize = 0;
  outSize = 0;
  percentComplete = 0;
  hOutfile = 1;
  currBlock = 0;
  rret = 0;
  ret = 0;
  bytesLeft = fileSize;
  OutputStdOut.199_1 = OutputStdOut;
  if (OutputStdOut.199_1 == 0) goto <D.45886>; else goto <D.45887>;
  <D.45886>:
  D.45990 = safe_open_output (OutFilename);
  hOutfile = D.45990;
  if (hOutfile == -1) goto <D.45888>; else goto <D.45889>;
  <D.45888>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not create output file [%s]!\n", OutFilename);
  D.45890 = -1;
  goto <D.45996>;
  <D.45889>:
  goto <D.45891>;
  <D.45887>:
  <D.45891>:
  <D.45892>:
  if (bytesLeft <= 0) goto <D.39483>; else goto <D.45893>;
  <D.45893>:
  D.45991 = syncGetTerminateFlag ();
  _2 = D.45991;
  retval.200 = _2 != 0;
  if (retval.200 != 0) goto <D.45895>; else goto <D.45896>;
  <D.45895>:
  close (hInfile);
  OutputStdOut.201_3 = OutputStdOut;
  if (OutputStdOut.201_3 == 0) goto <D.45897>; else goto <D.45898>;
  <D.45897>:
  close (hOutfile);
  goto <D.45899>;
  <D.45898>:
  <D.45899>:
  stderr.202_4 = stderr;
  fprintf (stderr.202_4, "directcompress: terminating - terminateFlag set\n");
  D.45890 = -1;
  goto <D.45996>;
  <D.45896>:
  _5 = (long int) blockSize;
  if (bytesLeft > _5) goto <D.45900>; else goto <D.45901>;
  <D.45900>:
  inSize = (off_t) blockSize;
  goto <D.45902>;
  <D.45901>:
  inSize = bytesLeft;
  <D.45902>:
  FileData = 0B;
  inSize.203 = (sizetype) inSize;
  FileData = operator new [] (inSize.203, &nothrow);
  if (FileData == 0B) goto <D.45904>; else goto <D.45905>;
  <D.45904>:
  close (hInfile);
  OutputStdOut.204_6 = OutputStdOut;
  if (OutputStdOut.204_6 == 0) goto <D.45906>; else goto <D.45907>;
  <D.45906>:
  close (hOutfile);
  goto <D.45908>;
  <D.45907>:
  <D.45908>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (FileData)!  Aborting...\n");
  D.45890 = -1;
  goto <D.45996>;
  <D.45905>:
  inSize.205_7 = (long unsigned int) inSize;
  D.45992 = do_read (hInfile, FileData, inSize.205_7);
  _8 = D.45992;
  rret = (int) _8;
  if (rret == 0) goto <D.45909>; else goto <D.45910>;
  <D.45909>:
  if (FileData != 0B) goto <D.45911>; else goto <D.45912>;
  <D.45911>:
  if (FileData != 0B) goto <D.45913>; else goto <D.45914>;
  <D.45913>:
  operator delete [] (FileData);
  goto <D.45915>;
  <D.45914>:
  <D.45915>:
  goto <D.45916>;
  <D.45912>:
  <D.45916>:
  goto <D.39483>;
  <D.45910>:
  if (rret < 0) goto <D.45917>; else goto <D.45918>;
  <D.45917>:
  close (hInfile);
  if (FileData != 0B) goto <D.45919>; else goto <D.45920>;
  <D.45919>:
  if (FileData != 0B) goto <D.45921>; else goto <D.45922>;
  <D.45921>:
  operator delete [] (FileData);
  goto <D.45923>;
  <D.45922>:
  <D.45923>:
  goto <D.45924>;
  <D.45920>:
  <D.45924>:
  OutputStdOut.206_9 = OutputStdOut;
  if (OutputStdOut.206_9 == 0) goto <D.45925>; else goto <D.45926>;
  <D.45925>:
  close (hOutfile);
  goto <D.45927>;
  <D.45926>:
  <D.45927>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not read from file!  Aborting...\n");
  D.45890 = -1;
  goto <D.45996>;
  <D.45918>:
  _10 = (long int) rret;
  bytesLeft = bytesLeft - _10;
  _11 = (double) inSize;
  _12 = _11 * 1.0100000000000000088817841970012523233890533447265625e+0;
  _13 = _12 + 6.0e+2;
  _14 = (int) _13;
  _15 = (unsigned int) _14;
  outSize = _15;
  outSize.207_16 = outSize;
  D.45928 = (sizetype) outSize.207_16;
  CompressedData = operator new [] (D.45928, &nothrow);
  if (CompressedData == 0B) goto <D.45929>; else goto <D.45930>;
  <D.45929>:
  close (hInfile);
  if (FileData != 0B) goto <D.45931>; else goto <D.45932>;
  <D.45931>:
  if (FileData != 0B) goto <D.45933>; else goto <D.45934>;
  <D.45933>:
  operator delete [] (FileData);
  goto <D.45935>;
  <D.45934>:
  <D.45935>:
  goto <D.45936>;
  <D.45932>:
  <D.45936>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (CompressedData)!  Aborting...\n");
  D.45890 = -1;
  goto <D.45996>;
  <D.45930>:
  Verbosity.208_17 = Verbosity;
  BWTblockSize.209_18 = BWTblockSize;
  _19 = (unsigned int) inSize;
  D.45993 = BZ2_bzBuffToBuffCompress (CompressedData, &outSize, FileData, _19, BWTblockSize.209_18, Verbosity.208_17, 30);
  ret = D.45993;
  if (ret != 0) goto <D.45937>; else goto <D.45938>;
  <D.45937>:
  close (hInfile);
  if (FileData != 0B) goto <D.45939>; else goto <D.45940>;
  <D.45939>:
  if (FileData != 0B) goto <D.45941>; else goto <D.45942>;
  <D.45941>:
  operator delete [] (FileData);
  goto <D.45943>;
  <D.45942>:
  <D.45943>:
  goto <D.45944>;
  <D.45940>:
  <D.45944>:
  handle_error (1, -1, "pbzip2: *ERROR during compression: %d!  Aborting...\n", ret);
  D.45890 = -1;
  goto <D.45996>;
  <D.45938>:
  outSize.210_20 = outSize;
  _21 = (long unsigned int) outSize.210_20;
  D.45994 = do_write (hOutfile, CompressedData, _21);
  _22 = D.45994;
  ret = (int) _22;
  if (ret <= 0) goto <D.45945>; else goto <D.45946>;
  <D.45945>:
  close (hInfile);
  if (FileData != 0B) goto <D.45947>; else goto <D.45948>;
  <D.45947>:
  if (FileData != 0B) goto <D.45949>; else goto <D.45950>;
  <D.45949>:
  operator delete [] (FileData);
  goto <D.45951>;
  <D.45950>:
  <D.45951>:
  goto <D.45952>;
  <D.45948>:
  <D.45952>:
  if (CompressedData != 0B) goto <D.45953>; else goto <D.45954>;
  <D.45953>:
  if (CompressedData != 0B) goto <D.45955>; else goto <D.45956>;
  <D.45955>:
  operator delete [] (CompressedData);
  goto <D.45957>;
  <D.45956>:
  <D.45957>:
  goto <D.45958>;
  <D.45954>:
  <D.45958>:
  OutputStdOut.211_23 = OutputStdOut;
  if (OutputStdOut.211_23 == 0) goto <D.45959>; else goto <D.45960>;
  <D.45959>:
  close (hOutfile);
  goto <D.45961>;
  <D.45960>:
  <D.45961>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not write to file!  Aborting...\n");
  D.45890 = -1;
  goto <D.45996>;
  <D.45946>:
  _24 = (long int) ret;
  CompressedSize = CompressedSize + _24;
  currBlock = currBlock + 1;
  percentCompleteOld = percentComplete;
  _25 = currBlock * 100;
  NumBlocksEstimated.212_26 = NumBlocksEstimated;
  percentComplete = _25 / NumBlocksEstimated.212_26;
  QuietMode.213_27 = QuietMode;
  if (QuietMode.213_27 != 1) goto <D.45962>; else goto <D.45963>;
  <D.45962>:
  if (percentComplete != percentCompleteOld) goto <D.45964>; else goto <D.45965>;
  <D.45964>:
  stderr.214_28 = stderr;
  fprintf (stderr.214_28, "Completed: %d%%             \r", percentComplete);
  stderr.215_29 = stderr;
  fflush (stderr.215_29);
  goto <D.45966>;
  <D.45965>:
  <D.45966>:
  goto <D.45967>;
  <D.45963>:
  <D.45967>:
  if (FileData != 0B) goto <D.45968>; else goto <D.45969>;
  <D.45968>:
  if (FileData != 0B) goto <D.45970>; else goto <D.45971>;
  <D.45970>:
  operator delete [] (FileData);
  goto <D.45972>;
  <D.45971>:
  <D.45972>:
  FileData = 0B;
  goto <D.45973>;
  <D.45969>:
  <D.45973>:
  if (CompressedData != 0B) goto <D.45974>; else goto <D.45975>;
  <D.45974>:
  if (CompressedData != 0B) goto <D.45976>; else goto <D.45977>;
  <D.45976>:
  operator delete [] (CompressedData);
  goto <D.45978>;
  <D.45977>:
  <D.45978>:
  CompressedData = 0B;
  goto <D.45979>;
  <D.45975>:
  <D.45979>:
  _30 = (long int) rret;
  if (inSize != _30) goto <D.45980>; else goto <D.45981>;
  <D.45980>:
  inSize = (off_t) rret;
  goto <D.45982>;
  <D.45981>:
  <D.45982>:
  goto <D.45892>;
  <D.39483>:
  close (hInfile);
  OutputStdOut.216_31 = OutputStdOut;
  if (OutputStdOut.216_31 == 0) goto <D.45983>; else goto <D.45984>;
  <D.45983>:
  close (hOutfile);
  goto <D.45985>;
  <D.45984>:
  <D.45985>:
  QuietMode.217_32 = QuietMode;
  if (QuietMode.217_32 != 1) goto <D.45986>; else goto <D.45987>;
  <D.45986>:
  CompressedSize.218_33 = (long unsigned int) CompressedSize;
  stderr.219_34 = stderr;
  fprintf (stderr.219_34, "    Output Size: %lu bytes\n", CompressedSize.218_33);
  goto <D.45988>;
  <D.45987>:
  <D.45988>:
  syncSetProducerDone (1);
  D.45890 = 0;
  goto <D.45996>;
  <D.45996>:
  outSize = {CLOBBER};
  goto <D.45989>;
  <D.45989>:
  return D.45890;
  <D.45995>:
  outSize = {CLOBBER};
  resx 1
}



;; Function int directdecompress(const char*, const char*) (_Z16directdecompressPKcS0_, funcdef_no=1986, decl_uid=39014, cgraph_uid=726, symbol_order=774)

int directdecompress(const char*, const char*) (const char * InFilename, const char * OutFilename)
{
  int D.46170;
  int D.46169;
  int D.46168;
  int D.46167;
  int D.46166;
  int D.46165;
  int D.46164;
  int D.46163;
  long unsigned int D.46162;
  int D.46161;
  int D.46160;
  int D.46159;
  int D.46158;
  BZFILE * D.46157;
  int D.46156;
  struct FILE * D.46155;
  struct FILE * D.46154;
  int c;
  int c;
  int i;
  int ret;
  int nUnused;
  int streamNo;
  int nread;
  int bzerr;
  unsigned char * unusedTmp;
  unsigned char unused[5000];
  unsigned char obuf[5000];
  BZFILE * bzf;
  struct FILE * zStream;
  struct FILE * stream;
  bool retval.270;
  bool retval.267;
  bool retval.252;
  bool retval.246;
  bool retval.242;
  bool retval.236;
  bool retval.225;
  bool retval.223;
  bool retval.220;
  int D.46001;

  stream = 0B;
  zStream = 0B;
  bzf = 0B;
  ret = 0;
  nUnused = 0;
  streamNo = 0;
  _1 = strcmp (InFilename, "-");
  if (_1 != 0) goto <D.45997>; else goto <D.45998>;
  <D.45997>:
  D.46154 = fopen (InFilename, "rb");
  zStream = D.46154;
  if (zStream == 0B) goto <D.45999>; else goto <D.46000>;
  <D.45999>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not open input file [%s]!  Aborting...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46000>:
  goto <D.46002>;
  <D.45998>:
  zStream = stdin;
  <D.46002>:
  _2 = ferror (zStream);
  retval.220 = _2 != 0;
  if (retval.220 != 0) goto <D.46004>; else goto <D.46005>;
  <D.46004>:
  stdin.221_3 = stdin;
  if (zStream != stdin.221_3) goto <D.46006>; else goto <D.46007>;
  <D.46006>:
  fclose (zStream);
  goto <D.46008>;
  <D.46007>:
  <D.46008>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with input stream of file [%s]!  Aborting...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46005>:
  OutputStdOut.222_4 = OutputStdOut;
  if (OutputStdOut.222_4 == 0) goto <D.46009>; else goto <D.46010>;
  <D.46009>:
  D.46155 = safe_fopen_output (OutFilename, "wb");
  stream = D.46155;
  if (stream == 0B) goto <D.46011>; else goto <D.46012>;
  <D.46011>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not open output file [%s]!  Aborting...\n", OutFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46012>:
  goto <D.46013>;
  <D.46010>:
  stream = stdout;
  <D.46013>:
  _5 = ferror (stream);
  retval.223 = _5 != 0;
  if (retval.223 != 0) goto <D.46015>; else goto <D.46016>;
  <D.46015>:
  stdout.224_6 = stdout;
  if (stream != stdout.224_6) goto <D.46017>; else goto <D.46018>;
  <D.46017>:
  fclose (stream);
  goto <D.46019>;
  <D.46018>:
  <D.46019>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with output stream of file [%s]!  Aborting...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46016>:
  <D.46020>:
  D.46156 = syncGetTerminateFlag ();
  _7 = D.46156;
  retval.225 = _7 != 0;
  if (retval.225 != 0) goto <D.46022>; else goto <D.46023>;
  <D.46022>:
  stderr.226_8 = stderr;
  fprintf (stderr.226_8, "directdecompress: terminating1 - terminateFlag set\n");
  stdin.227_9 = stdin;
  if (zStream != stdin.227_9) goto <D.46024>; else goto <D.46025>;
  <D.46024>:
  fclose (zStream);
  goto <D.46026>;
  <D.46025>:
  <D.46026>:
  stdout.228_10 = stdout;
  if (stream != stdout.228_10) goto <D.46027>; else goto <D.46028>;
  <D.46027>:
  fclose (stream);
  goto <D.46029>;
  <D.46028>:
  <D.46029>:
  D.46001 = -1;
  goto <D.46172>;
  <D.46023>:
  nUnused.229_11 = nUnused;
  Verbosity.230_12 = Verbosity;
  D.46157 = BZ2_bzReadOpen (&bzerr, zStream, Verbosity.230_12, 0, &unused, nUnused.229_11);
  bzf = D.46157;
  if (bzf == 0B) goto <D.46030>; else goto <D.46032>;
  <D.46032>:
  bzerr.231_13 = bzerr;
  if (bzerr.231_13 != 0) goto <D.46030>; else goto <D.46031>;
  <D.46030>:
  bzerr.232_14 = bzerr;
  D.46158 = testBZ2ErrorHandling (bzerr.232_14, bzf, streamNo);
  ret = D.46158;
  stdin.233_15 = stdin;
  if (zStream != stdin.233_15) goto <D.46033>; else goto <D.46034>;
  <D.46033>:
  fclose (zStream);
  goto <D.46035>;
  <D.46034>:
  <D.46035>:
  stdout.234_16 = stdout;
  if (stream != stdout.234_16) goto <D.46036>; else goto <D.46037>;
  <D.46036>:
  fclose (stream);
  goto <D.46038>;
  <D.46037>:
  <D.46038>:
  if (ret != 0) goto <D.46039>; else goto <D.46040>;
  <D.46039>:
  syncSetTerminateFlag (1);
  goto <D.46041>;
  <D.46040>:
  <D.46041>:
  D.46001 = ret;
  goto <D.46172>;
  <D.46031>:
  streamNo = streamNo + 1;
  <D.46042>:
  bzerr.235_17 = bzerr;
  if (bzerr.235_17 != 0) goto <D.39506>; else goto <D.46043>;
  <D.46043>:
  D.46159 = syncGetTerminateFlag ();
  _18 = D.46159;
  retval.236 = _18 != 0;
  if (retval.236 != 0) goto <D.46045>; else goto <D.46046>;
  <D.46045>:
  stderr.237_19 = stderr;
  fprintf (stderr.237_19, "directdecompress: terminating2 - terminateFlag set\n");
  stdin.238_20 = stdin;
  if (zStream != stdin.238_20) goto <D.46047>; else goto <D.46048>;
  <D.46047>:
  fclose (zStream);
  goto <D.46049>;
  <D.46048>:
  <D.46049>:
  stdout.239_21 = stdout;
  if (stream != stdout.239_21) goto <D.46050>; else goto <D.46051>;
  <D.46050>:
  fclose (stream);
  goto <D.46052>;
  <D.46051>:
  <D.46052>:
  D.46001 = -1;
  goto <D.46172>;
  <D.46046>:
  D.46160 = BZ2_bzRead (&bzerr, bzf, &obuf, 5000);
  nread = D.46160;
  bzerr.240_22 = bzerr;
  if (bzerr.240_22 == -5) goto <D.46053>; else goto <D.46054>;
  <D.46053>:
  ForceOverwrite.241_23 = ForceOverwrite;
  if (ForceOverwrite.241_23 == 1) goto <D.46055>; else goto <D.46056>;
  <D.46055>:
  rewind (zStream);
  <D.46057>:
  D.46161 = fgetc (zStream);
  c = D.46161;
  if (c == -1) goto <D.39508>; else goto <D.46058>;
  <D.46058>:
  ungetc (c, zStream);
  D.46162 = fread (&obuf, 1, 5000, zStream);
  _24 = D.46162;
  nread = (int) _24;
  _25 = ferror (zStream);
  retval.242 = _25 != 0;
  if (retval.242 != 0) goto <D.46060>; else goto <D.46061>;
  <D.46060>:
  bzerr.243_26 = bzerr;
  D.46163 = testBZ2ErrorHandling (bzerr.243_26, bzf, streamNo);
  ret = D.46163;
  stdin.244_27 = stdin;
  if (zStream != stdin.244_27) goto <D.46062>; else goto <D.46063>;
  <D.46062>:
  fclose (zStream);
  goto <D.46064>;
  <D.46063>:
  <D.46064>:
  stdout.245_28 = stdout;
  if (stream != stdout.245_28) goto <D.46065>; else goto <D.46066>;
  <D.46065>:
  fclose (stream);
  goto <D.46067>;
  <D.46066>:
  <D.46067>:
  if (ret != 0) goto <D.46068>; else goto <D.46069>;
  <D.46068>:
  syncSetTerminateFlag (1);
  goto <D.46070>;
  <D.46069>:
  <D.46070>:
  D.46001 = ret;
  goto <D.46172>;
  <D.46061>:
  if (nread > 0) goto <D.46071>; else goto <D.46072>;
  <D.46071>:
  _29 = (long unsigned int) nread;
  fwrite (&obuf, 1, _29, stream);
  goto <D.46073>;
  <D.46072>:
  <D.46073>:
  _30 = ferror (stream);
  retval.246 = _30 != 0;
  if (retval.246 != 0) goto <D.46075>; else goto <D.46076>;
  <D.46075>:
  bzerr.247_31 = bzerr;
  D.46164 = testBZ2ErrorHandling (bzerr.247_31, bzf, streamNo);
  ret = D.46164;
  stdin.248_32 = stdin;
  if (zStream != stdin.248_32) goto <D.46077>; else goto <D.46078>;
  <D.46077>:
  fclose (zStream);
  goto <D.46079>;
  <D.46078>:
  <D.46079>:
  stdout.249_33 = stdout;
  if (stream != stdout.249_33) goto <D.46080>; else goto <D.46081>;
  <D.46080>:
  fclose (stream);
  goto <D.46082>;
  <D.46081>:
  <D.46082>:
  if (ret != 0) goto <D.46083>; else goto <D.46084>;
  <D.46083>:
  syncSetTerminateFlag (1);
  goto <D.46085>;
  <D.46084>:
  <D.46085>:
  D.46001 = ret;
  goto <D.46172>;
  <D.46076>:
  goto <D.46057>;
  <D.39508>:
  goto closeok;
  <D.46056>:
  goto <D.46086>;
  <D.46054>:
  <D.46086>:
  bzerr.250_34 = bzerr;
  if (bzerr.250_34 == 0) goto <D.46087>; else goto <D.46089>;
  <D.46089>:
  bzerr.251_35 = bzerr;
  if (bzerr.251_35 == 4) goto <D.46087>; else goto <D.46088>;
  <D.46087>:
  if (nread > 0) goto <D.46090>; else goto <D.46091>;
  <D.46090>:
  _36 = (long unsigned int) nread;
  fwrite (&obuf, 1, _36, stream);
  goto <D.46092>;
  <D.46091>:
  <D.46092>:
  <D.46088>:
  _37 = ferror (stream);
  retval.252 = _37 != 0;
  if (retval.252 != 0) goto <D.46094>; else goto <D.46095>;
  <D.46094>:
  bzerr.253_38 = bzerr;
  D.46165 = testBZ2ErrorHandling (bzerr.253_38, bzf, streamNo);
  ret = D.46165;
  stdin.254_39 = stdin;
  if (zStream != stdin.254_39) goto <D.46096>; else goto <D.46097>;
  <D.46096>:
  fclose (zStream);
  goto <D.46098>;
  <D.46097>:
  <D.46098>:
  stdout.255_40 = stdout;
  if (stream != stdout.255_40) goto <D.46099>; else goto <D.46100>;
  <D.46099>:
  fclose (stream);
  goto <D.46101>;
  <D.46100>:
  <D.46101>:
  if (ret != 0) goto <D.46102>; else goto <D.46103>;
  <D.46102>:
  syncSetTerminateFlag (1);
  goto <D.46104>;
  <D.46103>:
  <D.46104>:
  D.46001 = ret;
  goto <D.46172>;
  <D.46095>:
  goto <D.46042>;
  <D.39506>:
  bzerr.256_41 = bzerr;
  if (bzerr.256_41 != 4) goto <D.46105>; else goto <D.46106>;
  <D.46105>:
  bzerr.257_42 = bzerr;
  D.46166 = testBZ2ErrorHandling (bzerr.257_42, bzf, streamNo);
  ret = D.46166;
  stdin.258_43 = stdin;
  if (zStream != stdin.258_43) goto <D.46107>; else goto <D.46108>;
  <D.46107>:
  fclose (zStream);
  goto <D.46109>;
  <D.46108>:
  <D.46109>:
  stdout.259_44 = stdout;
  if (stream != stdout.259_44) goto <D.46110>; else goto <D.46111>;
  <D.46110>:
  fclose (stream);
  goto <D.46112>;
  <D.46111>:
  <D.46112>:
  if (ret != 0) goto <D.46113>; else goto <D.46114>;
  <D.46113>:
  syncSetTerminateFlag (1);
  goto <D.46115>;
  <D.46114>:
  <D.46115>:
  D.46001 = ret;
  goto <D.46172>;
  <D.46106>:
  BZ2_bzReadGetUnused (&bzerr, bzf, &unusedTmp, &nUnused);
  bzerr.260_45 = bzerr;
  if (bzerr.260_45 != 0) goto <D.46116>; else goto <D.46117>;
  <D.46116>:
  bzerr.261_46 = bzerr;
  handle_error (1, 3, "pbzip2: *ERROR: Unexpected error [bzerr=%d]. Aborting!\n", bzerr.261_46);
  D.46001 = 3;
  goto <D.46172>;
  <D.46117>:
  i = 0;
  <D.46118>:
  nUnused.262_47 = nUnused;
  if (i >= nUnused.262_47) goto <D.39510>; else goto <D.46119>;
  <D.46119>:
  unusedTmp.263_48 = unusedTmp;
  _49 = (sizetype) i;
  _50 = unusedTmp.263_48 + _49;
  _51 = *_50;
  unused[i] = _51;
  i = i + 1;
  goto <D.46118>;
  <D.39510>:
  BZ2_bzReadClose (&bzerr, bzf);
  bzerr.264_52 = bzerr;
  if (bzerr.264_52 != 0) goto <D.46120>; else goto <D.46121>;
  <D.46120>:
  bzerr.265_53 = bzerr;
  handle_error (1, 3, "pbzip2: *ERROR: Unexpected error [bzerr=%d]. Aborting!\n", bzerr.265_53);
  D.46001 = 3;
  goto <D.46172>;
  <D.46121>:
  nUnused.266_54 = nUnused;
  if (nUnused.266_54 == 0) goto <D.46122>; else goto <D.46123>;
  <D.46122>:
  D.46167 = fgetc (zStream);
  c = D.46167;
  if (c == -1) goto <D.39504>; else goto <D.46124>;
  <D.46124>:
  ungetc (c, zStream);
  goto <D.46125>;
  <D.46123>:
  <D.46125>:
  goto <D.46020>;
  <D.39504>:
  closeok:
  _55 = ferror (zStream);
  retval.267 = _55 != 0;
  if (retval.267 != 0) goto <D.46127>; else goto <D.46128>;
  <D.46127>:
  stdin.268_56 = stdin;
  if (zStream != stdin.268_56) goto <D.46129>; else goto <D.46130>;
  <D.46129>:
  fclose (zStream);
  goto <D.46131>;
  <D.46130>:
  <D.46131>:
  stdout.269_57 = stdout;
  if (stream != stdout.269_57) goto <D.46132>; else goto <D.46133>;
  <D.46132>:
  fclose (stream);
  goto <D.46134>;
  <D.46133>:
  <D.46134>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with input stream of file [%s]!  Skipping...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46128>:
  D.46168 = do_fclose (zStream);
  ret = D.46168;
  if (ret == -1) goto <D.46135>; else goto <D.46136>;
  <D.46135>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46136>:
  _58 = ferror (stream);
  retval.270 = _58 != 0;
  if (retval.270 != 0) goto <D.46138>; else goto <D.46139>;
  <D.46138>:
  stdout.271_59 = stdout;
  if (stream != stdout.271_59) goto <D.46140>; else goto <D.46141>;
  <D.46140>:
  fclose (stream);
  goto <D.46142>;
  <D.46141>:
  <D.46142>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with output stream of file [%s]!  Skipping...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46139>:
  D.46169 = do_fflush (stream);
  ret = D.46169;
  if (ret != 0) goto <D.46143>; else goto <D.46144>;
  <D.46143>:
  stdout.272_60 = stdout;
  if (stream != stdout.272_60) goto <D.46145>; else goto <D.46146>;
  <D.46145>:
  fclose (stream);
  goto <D.46147>;
  <D.46146>:
  <D.46147>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with output stream of file [%s]!  Skipping...\n", InFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46144>:
  stdout.273_61 = stdout;
  if (stream != stdout.273_61) goto <D.46148>; else goto <D.46149>;
  <D.46148>:
  D.46170 = do_fclose (stream);
  ret = D.46170;
  if (ret == -1) goto <D.46150>; else goto <D.46151>;
  <D.46150>:
  handle_error (1, -1, "pbzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", OutFilename);
  D.46001 = -1;
  goto <D.46172>;
  <D.46151>:
  goto <D.46152>;
  <D.46149>:
  <D.46152>:
  syncSetProducerDone (1);
  D.46001 = 0;
  goto <D.46172>;
  <D.46172>:
  obuf = {CLOBBER};
  unused = {CLOBBER};
  unusedTmp = {CLOBBER};
  bzerr = {CLOBBER};
  nUnused = {CLOBBER};
  goto <D.46153>;
  <D.46153>:
  return D.46001;
  <D.46171>:
  obuf = {CLOBBER};
  unused = {CLOBBER};
  unusedTmp = {CLOBBER};
  bzerr = {CLOBBER};
  nUnused = {CLOBBER};
  resx 1
}



;; Function size_t fread(void*, size_t, size_t, FILE*) (fread, funcdef_no=940, decl_uid=18045, cgraph_uid=274, symbol_order=276)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
size_t fread(void*, size_t, size_t, FILE*) (void * restrict __ptr, size_t __size, size_t __n, struct FILE * restrict __stream)
{
  size_t D.46186;
  size_t D.46185;
  size_t D.46184;
  size_t D.46179;

  _1 = __builtin_object_size (__ptr, 0);
  if (_1 != 18446744073709551615) goto <D.46173>; else goto <D.46174>;
  <D.46173>:
  _2 = __builtin_constant_p (__size);
  if (_2 == 0) goto <D.46175>; else goto <D.46177>;
  <D.46177>:
  _3 = __builtin_constant_p (__n);
  if (_3 == 0) goto <D.46175>; else goto <D.46178>;
  <D.46178>:
  _4 = __size | __n;
  if (_4 > 4294967295) goto <D.46175>; else goto <D.46176>;
  <D.46175>:
  _5 = __builtin_object_size (__ptr, 0);
  D.46184 = __fread_chk (__ptr, _5, __size, __n, __stream);
  D.46179 = D.46184;
  goto <D.46183>;
  <D.46176>:
  _6 = __size * __n;
  _7 = __builtin_object_size (__ptr, 0);
  if (_6 > _7) goto <D.46180>; else goto <D.46181>;
  <D.46180>:
  _8 = __builtin_object_size (__ptr, 0);
  D.46185 = __fread_chk_warn (__ptr, _8, __size, __n, __stream);
  D.46179 = D.46185;
  goto <D.46183>;
  <D.46181>:
  goto <D.46182>;
  <D.46174>:
  <D.46182>:
  D.46186 = __fread_alias (__ptr, __size, __n, __stream);
  D.46179 = D.46186;
  goto <D.46183>;
  <D.46183>:
  return D.46179;
}



;; Function int do_fflush(FILE*) (_Z9do_fflushP8_IO_FILE, funcdef_no=1951, decl_uid=39231, cgraph_uid=692, symbol_order=740)

int do_fflush(FILE*) (struct FILE * file)
{
  struct ErrorContext * D.46193;
  int D.46192;
  int ret;
  int D.46190;

  D.46192 = fflush (file);
  ret = D.46192;
  if (ret == -1) goto <D.46187>; else goto <D.46188>;
  <D.46187>:
  D.46193 = pbzip2::ErrorContext::getInstance ();
  _1 = D.46193;
  pbzip2::ErrorContext::saveError (_1);
  goto <D.46189>;
  <D.46188>:
  <D.46189>:
  D.46190 = ret;
  goto <D.46191>;
  <D.46191>:
  return D.46190;
}



;; Function int do_fclose(FILE*) (_Z9do_fcloseP8_IO_FILE, funcdef_no=1950, decl_uid=39227, cgraph_uid=691, symbol_order=739)

int do_fclose(FILE*) (struct FILE * file)
{
  struct ErrorContext * D.46200;
  int D.46199;
  int ret;
  int D.46197;

  D.46199 = fclose (file);
  ret = D.46199;
  if (ret == -1) goto <D.46194>; else goto <D.46195>;
  <D.46194>:
  D.46200 = pbzip2::ErrorContext::getInstance ();
  _1 = D.46200;
  pbzip2::ErrorContext::saveError (_1);
  goto <D.46196>;
  <D.46195>:
  <D.46196>:
  D.46197 = ret;
  goto <D.46198>;
  <D.46198>:
  return D.46197;
}



;; Function ssize_t bufread(int, char*, size_t) (_Z7bufreadiPcm, funcdef_no=1987, decl_uid=39048, cgraph_uid=727, symbol_order=775)

ssize_t bufread(int, char*, size_t) (int hf, char * buf, size_t bsize)
{
  long int D.46210;
  int rsize;
  int ret;
  size_t bufr;
  ssize_t D.46204;

  bufr = 0;
  rsize = (int) bsize;
  <D.46201>:
  _1 = (long unsigned int) rsize;
  D.46210 = read (hf, buf, _1);
  _2 = D.46210;
  ret = (int) _2;
  if (ret < 0) goto <D.46202>; else goto <D.46203>;
  <D.46202>:
  D.46204 = (ssize_t) ret;
  goto <D.46209>;
  <D.46203>:
  if (ret == 0) goto <D.46205>; else goto <D.46206>;
  <D.46205>:
  D.46204 = (ssize_t) bufr;
  goto <D.46209>;
  <D.46206>:
  _3 = (long unsigned int) ret;
  bufr = bufr + _3;
  if (bufr == bsize) goto <D.46207>; else goto <D.46208>;
  <D.46207>:
  D.46204 = (ssize_t) bsize;
  goto <D.46209>;
  <D.46208>:
  rsize = rsize - ret;
  _4 = (sizetype) ret;
  buf = buf + _4;
  goto <D.46201>;
  <D.46209>:
  return D.46204;
}



;; Function int producer(int, int, queue*) (_Z8produceriiP5queue, funcdef_no=1988, decl_uid=39018, cgraph_uid=728, symbol_order=776)

int producer(int, int, queue*) (int hInfile, int blockSize, struct queue * fifo)
{
  int D.46252;
  long int D.46251;
  int D.46250;
  struct outBuff * queueElement;
  int ret;
  size_t inSize;
  char * FileData;
  struct outBuff * iftmp.277;
  void * D.39531;
  bool retval.276;
  long unsigned int inSize.275;
  int D.46215;
  bool retval.274;

  FileData = 0B;
  inSize = 0;
  ret = 0;
  safe_mutex_lock (&ProgressIndicatorsMutex);
  NumBlocks = 0;
  InBytesProduced = 0;
  safe_mutex_unlock (&ProgressIndicatorsMutex);
  <D.46211>:
  D.46250 = syncGetTerminateFlag ();
  _1 = D.46250;
  retval.274 = _1 != 0;
  if (retval.274 != 0) goto <D.46213>; else goto <D.46214>;
  <D.46213>:
  close (hInfile);
  D.46215 = -1;
  goto <D.46249>;
  <D.46214>:
  inSize = (size_t) blockSize;
  FileData = 0B;
  inSize.275 = inSize;
  FileData = operator new [] (inSize.275, &nothrow);
  if (FileData == 0B) goto <D.46217>; else goto <D.46218>;
  <D.46217>:
  close (hInfile);
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (FileData)!  Aborting...\n");
  D.46215 = -1;
  goto <D.46249>;
  <D.46218>:
  D.46251 = bufread (hInfile, FileData, inSize);
  _2 = D.46251;
  ret = (int) _2;
  if (ret == 0) goto <D.46219>; else goto <D.46220>;
  <D.46219>:
  if (FileData != 0B) goto <D.46221>; else goto <D.46222>;
  <D.46221>:
  if (FileData != 0B) goto <D.46223>; else goto <D.46224>;
  <D.46223>:
  operator delete [] (FileData);
  goto <D.46225>;
  <D.46224>:
  <D.46225>:
  goto <D.46226>;
  <D.46222>:
  <D.46226>:
  goto <D.39532>;
  <D.46220>:
  if (ret < 0) goto <D.46227>; else goto <D.46228>;
  <D.46227>:
  close (hInfile);
  if (FileData != 0B) goto <D.46229>; else goto <D.46230>;
  <D.46229>:
  if (FileData != 0B) goto <D.46231>; else goto <D.46232>;
  <D.46231>:
  operator delete [] (FileData);
  goto <D.46233>;
  <D.46232>:
  <D.46233>:
  goto <D.46234>;
  <D.46230>:
  <D.46234>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not read from file!  Aborting...\n");
  D.46215 = -1;
  goto <D.46249>;
  <D.46228>:
  _3 = (long unsigned int) ret;
  if (inSize != _3) goto <D.46235>; else goto <D.46236>;
  <D.46235>:
  inSize = (size_t) ret;
  goto <D.46237>;
  <D.46236>:
  <D.46237>:
  _4 = fifo->mut;
  safe_mutex_lock (_4);
  <D.46238>:
  _5 = fifo->full;
  if (_5 == 0) goto <D.39534>; else goto <D.46239>;
  <D.46239>:
  _6 = fifo->mut;
  _7 = fifo->notFull;
  safe_cond_wait (_7, _6);
  D.46252 = syncGetTerminateFlag ();
  _8 = D.46252;
  retval.276 = _8 != 0;
  if (retval.276 != 0) goto <D.46241>; else goto <D.46242>;
  <D.46241>:
  _9 = fifo->mut;
  pthread_mutex_unlock (_9);
  close (hInfile);
  D.46215 = -1;
  goto <D.46249>;
  <D.46242>:
  goto <D.46238>;
  <D.39534>:
  D.39531 = operator new (40, &nothrow);
  if (D.39531 != 0B) goto <D.46244>; else goto <D.46245>;
  <D.46244>:
  NumBlocks.278_10 = NumBlocks;
  _11 = (unsigned int) inSize;
  outBuff::outBuff (D.39531, FileData, _11, NumBlocks.278_10, 0, 0, 1, 0B);
  iftmp.277 = D.39531;
  goto <D.46246>;
  <D.46245>:
  iftmp.277 = D.39531;
  <D.46246>:
  queueElement = iftmp.277;
  if (queueElement == 0B) goto <D.46247>; else goto <D.46248>;
  <D.46247>:
  close (hInfile);
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (queueElement)!  Aborting...\n");
  D.46215 = -1;
  goto <D.46249>;
  <D.46248>:
  queue::add (fifo, queueElement);
  _12 = fifo->notEmpty;
  safe_cond_signal (_12);
  safe_mutex_lock (&ProgressIndicatorsMutex);
  NumBlocks.279_13 = NumBlocks;
  _14 = NumBlocks.279_13 + 1;
  NumBlocks = _14;
  InBytesProduced.280_15 = InBytesProduced;
  InBytesProduced.281_16 = (long unsigned int) InBytesProduced.280_15;
  _17 = inSize + InBytesProduced.281_16;
  _18 = (long int) _17;
  InBytesProduced = _18;
  safe_mutex_unlock (&ProgressIndicatorsMutex);
  _19 = fifo->mut;
  safe_mutex_unlock (_19);
  goto <D.46211>;
  <D.39532>:
  close (hInfile);
  syncSetProducerDone (1);
  _20 = fifo->mut;
  safe_mutex_lock (_20);
  _21 = fifo->notEmpty;
  safe_cond_broadcast (_21);
  _22 = fifo->mut;
  safe_mutex_unlock (_22);
  D.46215 = 0;
  goto <D.46249>;
  <D.46249>:
  return D.46215;
}



;; Function void* consumer(void*) (consumer, funcdef_no=1989, decl_uid=39094, cgraph_uid=729, symbol_order=777)

void* consumer(void*) (void * q)
{
  struct outBuff * D.46288;
  int D.46287;
  int D.46286;
  int D.46285;
  int D.46284;
  struct outBuff outBlock;
  int ret;
  unsigned int outSize;
  char * CompressedData;
  struct outBuff * fileData;
  struct queue * fifo;
  bool retval.297;
  sizetype D.46275;
  bool iftmp.286;
  bool retval.285;
  bool iftmp.284;
  bool retval.283;
  void * D.46257;
  bool retval.282;

  fileData = 0B;
  CompressedData = 0B;
  outSize = 0;
  ret = -1;
  fifo = q;
  <D.46253>:
  D.46284 = syncGetTerminateFlag ();
  _1 = D.46284;
  retval.282 = _1 != 0;
  if (retval.282 != 0) goto <D.46255>; else goto <D.46256>;
  <D.46255>:
  D.46257 = 0B;
  goto <D.46291>;
  <D.46256>:
  _2 = fifo->mut;
  safe_mutex_lock (_2);
  <D.46258>:
  _3 = fifo->empty;
  if (_3 == 0) goto <D.46263>; else goto <D.46261>;
  <D.46263>:
  _4 = queue::remove (fifo, &fileData);
  if (_4 == 1) goto <D.46264>; else goto <D.46261>;
  <D.46264>:
  iftmp.284 = 1;
  goto <D.46262>;
  <D.46261>:
  iftmp.284 = 0;
  <D.46262>:
  retval.283 = iftmp.284;
  if (retval.283 != 0) goto <D.39548>; else goto <D.46265>;
  <D.46265>:
  _5 = fifo->empty;
  if (_5 != 0) goto <D.46271>; else goto <D.46268>;
  <D.46271>:
  D.46285 = syncGetProducerDone ();
  _6 = D.46285;
  if (_6 == 1) goto <D.46269>; else goto <D.46272>;
  <D.46272>:
  D.46286 = syncGetTerminateFlag ();
  _7 = D.46286;
  if (_7 != 0) goto <D.46269>; else goto <D.46268>;
  <D.46269>:
  iftmp.286 = 1;
  goto <D.46270>;
  <D.46268>:
  iftmp.286 = 0;
  <D.46270>:
  retval.285 = iftmp.286;
  if (retval.285 != 0) goto <D.46273>; else goto <D.46274>;
  <D.46273>:
  _8 = fifo->mut;
  safe_mutex_unlock (_8);
  D.46257 = 0B;
  goto <D.46291>;
  <D.46274>:
  _9 = fifo->mut;
  _10 = fifo->notEmpty;
  safe_cond_wait (_10, _9);
  goto <D.46258>;
  <D.39548>:
  _11 = fifo->notFull;
  safe_cond_signal (_11);
  _12 = fifo->mut;
  safe_mutex_unlock (_12);
  fileData.287_13 = fileData;
  _14 = fileData.287_13->bufSize;
  _15 = (double) _14;
  _16 = _15 * 1.0100000000000000088817841970012523233890533447265625e+0;
  _17 = _16 + 6.0e+2;
  _18 = (unsigned int) _17;
  outSize = _18;
  outSize.288_19 = outSize;
  D.46275 = (sizetype) outSize.288_19;
  CompressedData = operator new [] (D.46275, &nothrow);
  if (CompressedData == 0B) goto <D.46276>; else goto <D.46277>;
  <D.46276>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (CompressedData)!  Aborting...\n");
  D.46257 = 0B;
  goto <D.46291>;
  <D.46277>:
  Verbosity.289_20 = Verbosity;
  BWTblockSize.290_21 = BWTblockSize;
  fileData.291_22 = fileData;
  _23 = fileData.291_22->bufSize;
  fileData.292_24 = fileData;
  _25 = fileData.292_24->buf;
  D.46287 = BZ2_bzBuffToBuffCompress (CompressedData, &outSize, _25, _23, BWTblockSize.290_21, Verbosity.289_20, 30);
  ret = D.46287;
  if (ret != 0) goto <D.46278>; else goto <D.46279>;
  <D.46278>:
  handle_error (1, -1, "pbzip2: *ERROR during compression: %d!  Aborting...\n", ret);
  D.46257 = 0B;
  goto <D.46291>;
  <D.46279>:
  fileData.293_26 = fileData;
  _27 = &fileData.293_26->buf;
  disposeMemory<char> (_27);
  fileData.294_28 = fileData;
  _29 = fileData.294_28->bufSize;
  fileData.295_30 = fileData;
  _31 = fileData.295_30->blockNumber;
  outSize.296_32 = outSize;
  outBuff::outBuff (&outBlock, CompressedData, outSize.296_32, _31, 0, _29, 1, 0B);
  D.46288 = outputBufferAdd (&outBlock, "consumer");
  _33 = D.46288;
  retval.297 = _33 == 0B;
  if (retval.297 != 0) goto <D.46281>; else goto <D.46282>;
  <D.46281>:
  D.46257 = 0B;
  goto <D.46291>;
  <D.46282>:
  fileData.298_34 = fileData;
  operator delete (fileData.298_34, 40);
  fileData = 0B;
  outBlock = {CLOBBER};
  goto <D.46290>;
  <D.46291>:
  outBlock = {CLOBBER};
  goto <D.46293>;
  <D.46290>:
  goto <D.46253>;
  D.46257 = 0B;
  goto <D.46293>;
  <D.46293>:
  fileData = {CLOBBER};
  outSize = {CLOBBER};
  goto <D.46283>;
  <D.46283>:
  return D.46257;
  <D.46289>:
  outBlock = {CLOBBER};
  resx 2
  <D.46292>:
  fileData = {CLOBBER};
  outSize = {CLOBBER};
  resx 1
}



;; Function int mutexesInit() (_Z11mutexesInitv, funcdef_no=1990, decl_uid=39019, cgraph_uid=730, symbol_order=778)

int mutexesInit() ()
{
  int D.46296;

  _1 = operator new (40, &nothrow);
  OutMutex = _1;
  OutMutex.299_2 = OutMutex;
  if (OutMutex.299_2 == 0B) goto <D.46294>; else goto <D.46295>;
  <D.46294>:
  stderr.300_3 = stderr;
  fprintf (stderr.300_3, "pbzip2: *ERROR: Could not allocate memory (OutMutex)!  Aborting...\n");
  D.46296 = 1;
  goto <D.46299>;
  <D.46295>:
  OutMutex.301_4 = OutMutex;
  pthread_mutex_init (OutMutex.301_4, 0B);
  _5 = operator new (40, &nothrow);
  ProducerDoneMutex = _5;
  ProducerDoneMutex.302_6 = ProducerDoneMutex;
  if (ProducerDoneMutex.302_6 == 0B) goto <D.46297>; else goto <D.46298>;
  <D.46297>:
  stderr.303_7 = stderr;
  fprintf (stderr.303_7, "pbzip2: *ERROR: Could not allocate memory (ProducerDoneMutex)!  Aborting...\n");
  D.46296 = 1;
  goto <D.46299>;
  <D.46298>:
  ProducerDoneMutex.304_8 = ProducerDoneMutex;
  pthread_mutex_init (ProducerDoneMutex.304_8, 0B);
  D.46296 = 0;
  goto <D.46299>;
  <D.46299>:
  return D.46296;
}



;; Function void mutexesDelete() (_Z13mutexesDeletev, funcdef_no=1991, decl_uid=39020, cgraph_uid=731, symbol_order=779)

void mutexesDelete() ()
{
  OutMutex.305_1 = OutMutex;
  if (OutMutex.305_1 != 0B) goto <D.46300>; else goto <D.46301>;
  <D.46300>:
  OutMutex.306_2 = OutMutex;
  pthread_mutex_destroy (OutMutex.306_2);
  OutMutex.307_3 = OutMutex;
  operator delete (OutMutex.307_3, 40);
  OutMutex = 0B;
  goto <D.46302>;
  <D.46301>:
  <D.46302>:
  ProducerDoneMutex.308_4 = ProducerDoneMutex;
  if (ProducerDoneMutex.308_4 != 0B) goto <D.46303>; else goto <D.46304>;
  <D.46303>:
  ProducerDoneMutex.309_5 = ProducerDoneMutex;
  pthread_mutex_destroy (ProducerDoneMutex.309_5);
  ProducerDoneMutex.310_6 = ProducerDoneMutex;
  operator delete (ProducerDoneMutex.310_6, 40);
  ProducerDoneMutex = 0B;
  goto <D.46305>;
  <D.46304>:
  <D.46305>:
  return;
}



;; Function queue* queueInit(int) (_Z9queueIniti, funcdef_no=1992, decl_uid=39022, cgraph_uid=732, symbol_order=780)

queue* queueInit(int) (int queueSize)
{
  struct queue * q;
  sizetype D.46327;
  size_t iftmp.331;
  sizetype D.46314;
  size_t iftmp.313;
  struct queue * D.46312;
  struct queue * iftmp.311;
  void * D.39558;

  QUEUESIZE = queueSize;
  D.39558 = operator new (96, &nothrow);
  if (D.39558 != 0B) goto <D.46307>; else goto <D.46308>;
  <D.46307>:
  queue::queue (D.39558);
  iftmp.311 = D.39558;
  goto <D.46309>;
  <D.46308>:
  iftmp.311 = D.39558;
  <D.46309>:
  q = iftmp.311;
  q.312_1 = q;
  if (q.312_1 == 0B) goto <D.46310>; else goto <D.46311>;
  <D.46310>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46311>:
  D.46314 = (sizetype) queueSize;
  if (D.46314 <= 1152921504606846975) goto <D.46315>; else goto <D.46316>;
  <D.46315>:
  iftmp.313 = D.46314 * 8;
  goto <D.46317>;
  <D.46316>:
  __cxa_throw_bad_array_new_length ();
  <D.46317>:
  q.314_2 = q;
  _3 = operator new [] (iftmp.313, &nothrow);
  q.314_2->qData = _3;
  q.315_4 = q;
  _5 = q.315_4->qData;
  if (_5 == 0B) goto <D.46318>; else goto <D.46319>;
  <D.46318>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46319>:
  q.316_6 = q;
  _7 = (long int) queueSize;
  q.316_6->size = _7;
  q.317_8 = q;
  queue::clear (q.317_8);
  q.318_9 = q;
  q.318_9->mut = 0B;
  q.319_10 = q;
  _11 = operator new (40, &nothrow);
  q.319_10->mut = _11;
  q.320_12 = q;
  _13 = q.320_12->mut;
  if (_13 == 0B) goto <D.46320>; else goto <D.46321>;
  <D.46320>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46321>:
  q.321_14 = q;
  _15 = q.321_14->mut;
  pthread_mutex_init (_15, 0B);
  q.322_16 = q;
  q.322_16->notFull = 0B;
  q.323_17 = q;
  _18 = operator new (48, &nothrow);
  q.323_17->notFull = _18;
  q.324_19 = q;
  _20 = q.324_19->notFull;
  if (_20 == 0B) goto <D.46322>; else goto <D.46323>;
  <D.46322>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46323>:
  q.325_21 = q;
  _22 = q.325_21->notFull;
  pthread_cond_init (_22, 0B);
  q.326_23 = q;
  q.326_23->notEmpty = 0B;
  q.327_24 = q;
  _25 = operator new (48, &nothrow);
  q.327_24->notEmpty = _25;
  q.328_26 = q;
  _27 = q.328_26->notEmpty;
  if (_27 == 0B) goto <D.46324>; else goto <D.46325>;
  <D.46324>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46325>:
  q.329_28 = q;
  _29 = q.329_28->notEmpty;
  pthread_cond_init (_29, 0B);
  q.330_30 = q;
  q.330_30->consumers = 0B;
  D.46327 = (sizetype) queueSize;
  if (D.46327 <= 1152921504606846975) goto <D.46328>; else goto <D.46329>;
  <D.46328>:
  iftmp.331 = D.46327 * 8;
  goto <D.46330>;
  <D.46329>:
  __cxa_throw_bad_array_new_length ();
  <D.46330>:
  q.332_31 = q;
  _32 = operator new [] (iftmp.331, &nothrow);
  q.332_31->consumers = _32;
  q.333_33 = q;
  _34 = q.333_33->consumers;
  if (_34 == 0B) goto <D.46331>; else goto <D.46332>;
  <D.46331>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46332>:
  notTooMuchNumBuffered = 0B;
  _35 = operator new (48, &nothrow);
  notTooMuchNumBuffered = _35;
  notTooMuchNumBuffered.334_36 = notTooMuchNumBuffered;
  if (notTooMuchNumBuffered.334_36 == 0B) goto <D.46333>; else goto <D.46334>;
  <D.46333>:
  D.46312 = 0B;
  goto <D.46337>;
  <D.46334>:
  notTooMuchNumBuffered.335_37 = notTooMuchNumBuffered;
  pthread_cond_init (notTooMuchNumBuffered.335_37, 0B);
  D.46312 = q;
  goto <D.46337>;
  <D.46337>:
  q = {CLOBBER};
  goto <D.46335>;
  <D.46335>:
  return D.46312;
  <D.46336>:
  q = {CLOBBER};
  resx 1
}



;; Function queue::queue() (_ZN5queueC2Ev, funcdef_no=1073, decl_uid=26069, cgraph_uid=361, symbol_order=363)

queue::queue() (struct queue * const this)
{
  MEM[(struct  &)this] = {CLOBBER};
  this->count = 0;
  this->lastElement = 0B;
  return;
}



;; Function void queue::clear() (_ZN5queue5clearEv, funcdef_no=1075, decl_uid=26059, cgraph_uid=363, symbol_order=365)

void queue::clear() (struct queue * const this)
{
  this->empty = 1;
  this->full = 0;
  this->head = 0;
  this->tail = 0;
  this->count = 0;
  this->lastElement = 0B;
  this->topLevelFull = 0;
  this->topLevelEmpty = 1;
  return;
}



;; Function void queueDelete(queue*) (_Z11queueDeleteP5queue, funcdef_no=1993, decl_uid=39024, cgraph_uid=733, symbol_order=781)

void queueDelete(queue*) (struct queue * q)
{
  if (q == 0B) goto <D.46338>; else goto <D.46339>;
  <D.46338>:
  goto <D.46358>;
  <D.46339>:
  _1 = q->mut;
  if (_1 != 0B) goto <D.46340>; else goto <D.46341>;
  <D.46340>:
  _2 = q->mut;
  pthread_mutex_destroy (_2);
  _3 = q->mut;
  operator delete (_3, 40);
  q->mut = 0B;
  goto <D.46342>;
  <D.46341>:
  <D.46342>:
  _4 = q->notFull;
  if (_4 != 0B) goto <D.46343>; else goto <D.46344>;
  <D.46343>:
  _5 = q->notFull;
  pthread_cond_destroy (_5);
  _6 = q->notFull;
  operator delete (_6, 48);
  q->notFull = 0B;
  goto <D.46345>;
  <D.46344>:
  <D.46345>:
  _7 = q->notEmpty;
  if (_7 != 0B) goto <D.46346>; else goto <D.46347>;
  <D.46346>:
  _8 = q->notEmpty;
  pthread_cond_destroy (_8);
  _9 = q->notEmpty;
  operator delete (_9, 48);
  q->notEmpty = 0B;
  goto <D.46348>;
  <D.46347>:
  <D.46348>:
  _10 = q->consumers;
  if (_10 != 0B) goto <D.46349>; else goto <D.46350>;
  <D.46349>:
  _11 = q->consumers;
  operator delete [] (_11);
  goto <D.46351>;
  <D.46350>:
  <D.46351>:
  _12 = q->qData;
  if (_12 != 0B) goto <D.46352>; else goto <D.46353>;
  <D.46352>:
  _13 = q->qData;
  operator delete [] (_13);
  goto <D.46354>;
  <D.46353>:
  <D.46354>:
  operator delete (q, 96);
  q = 0B;
  notTooMuchNumBuffered.336_14 = notTooMuchNumBuffered;
  if (notTooMuchNumBuffered.336_14 != 0B) goto <D.46355>; else goto <D.46356>;
  <D.46355>:
  notTooMuchNumBuffered.337_15 = notTooMuchNumBuffered;
  pthread_cond_destroy (notTooMuchNumBuffered.337_15);
  notTooMuchNumBuffered.338_16 = notTooMuchNumBuffered;
  operator delete (notTooMuchNumBuffered.338_16, 48);
  notTooMuchNumBuffered = 0B;
  goto <D.46357>;
  <D.46356>:
  <D.46357>:
  goto <D.46358>;
  <D.46358>:
  return;
}



;; Function void outputBufferInit(size_t) (_Z16outputBufferInitm, funcdef_no=1994, decl_uid=39026, cgraph_uid=734, symbol_order=782)

void outputBufferInit(size_t) (size_t size)
{
  struct outBuff emptyElement;
  bool retval.340;

  OutMutex.339_1 = OutMutex;
  safe_mutex_lock (OutMutex.339_1);
  NextBlockToWrite = 0;
  OutBufferPosToWrite = 0;
  NumBufferedBlocks = 0;
  NumBufferedTailBlocks = 0;
  outBuff::outBuff (&emptyElement, 0B, 0, 0, 0, 0, 1, 0B);
  emptyElement.buf = 0B;
  emptyElement.bufSize = 0;
  std::vector<outBuff>::assign (&OutputBuffer, size, &emptyElement);
  _2 = std::vector<outBuff>::size (&OutputBuffer);
  retval.340 = size != _2;
  if (retval.340 != 0) goto <D.46360>; else goto <D.46361>;
  <D.46360>:
  stderr.341_3 = stderr;
  fprintf (stderr.341_3, "pbzip2: *ERROR: Could not initialize (OutputBuffer); size=%lu!  Aborting...\n", size);
  OutMutex.342_4 = OutMutex;
  safe_mutex_unlock (OutMutex.342_4);
  exit (1);
  <D.46361>:
  OutMutex.343_5 = OutMutex;
  safe_mutex_unlock (OutMutex.343_5);
  emptyElement = {CLOBBER};
  return;
  <D.46362>:
  emptyElement = {CLOBBER};
  resx 1
}



;; Function void std::vector<_Tp, _Alloc>::assign(std::vector<_Tp, _Alloc>::size_type, const value_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE6assignEmRKS0_, funcdef_no=2256, decl_uid=37998, cgraph_uid=996, symbol_order=1044)

void std::vector<_Tp, _Alloc>::assign(std::vector<_Tp, _Alloc>::size_type, const value_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this, size_type __n, const struct value_type & __val)
{
  std::vector<outBuff>::_M_fill_assign (this, __n, __val);
  return;
}



;; Function void std::vector<_Tp, _Alloc>::_M_fill_assign(std::size_t, const value_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE14_M_fill_assignEmRKS0_, funcdef_no=2396, decl_uid=38291, cgraph_uid=1136, symbol_order=1184)

void std::vector<_Tp, _Alloc>::_M_fill_assign(std::size_t, const value_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this, size_t __n, const struct value_type & __val)
{
  struct outBuff * D.46375;
  struct outBuff * D.46374;
  struct vector __tmp;
  struct __normal_iterator D.46371;
  struct __normal_iterator D.46370;
  bool retval.345;
  bool retval.344;

  _1 = std::vector<outBuff>::capacity (this);
  retval.344 = __n > _1;
  if (retval.344 != 0) goto <D.46364>; else goto <D.46365>;
  <D.46364>:
  _2 = &this->D.38528;
  _3 = std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_get_Tp_allocator (_2);
  std::vector<outBuff>::vector (&__tmp, __n, __val, _3);
  _4 = &this->D.38528._M_impl;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::_M_swap_data (&__tmp.D.38528._M_impl, _4);
  std::vector<outBuff>::~vector (&__tmp);
  __tmp = {CLOBBER};
  goto <D.46366>;
  <D.46365>:
  _5 = std::vector<outBuff>::size (this);
  retval.345 = __n > _5;
  if (retval.345 != 0) goto <D.46368>; else goto <D.46369>;
  <D.46368>:
  D.46370 = std::vector<outBuff>::end (this);
  D.46371 = std::vector<outBuff>::begin (this);
  std::fill<__gnu_cxx::__normal_iterator<outBuff*, std::vector<outBuff> >, outBuff> (D.46371, D.46370, __val);
  _6 = &this->D.38528;
  _7 = std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_get_Tp_allocator (_6);
  _8 = std::vector<outBuff>::size (this);
  _9 = __n - _8;
  _10 = this->D.38528._M_impl._M_finish;
  D.46374 = std::__uninitialized_fill_n_a<outBuff*, long unsigned int, outBuff, outBuff> (_10, _9, __val, _7);
  _11 = D.46374;
  this->D.38528._M_impl._M_finish = _11;
  goto <D.46372>;
  <D.46369>:
  _12 = this->D.38528._M_impl._M_start;
  D.46375 = std::fill_n<outBuff*, long unsigned int, outBuff> (_12, __n, __val);
  _13 = D.46375;
  std::vector<outBuff>::_M_erase_at_end (this, _13);
  <D.46372>:
  <D.46366>:
  return;
  <D.46373>:
  __tmp = {CLOBBER};
  resx 1
}



;; Function std::vector<_Tp, _Alloc>::size_type std::vector<_Tp, _Alloc>::capacity() const [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNKSt6vectorI7outBuffSaIS0_EE8capacityEv, funcdef_no=2461, decl_uid=38059, cgraph_uid=1201, symbol_order=1249)

std::vector<_Tp, _Alloc>::size_type std::vector<_Tp, _Alloc>::capacity() const [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (const struct vector * const this)
{
  size_type D.46376;

  _1 = this->D.38528._M_impl._M_end_of_storage;
  _2 = (long int) _1;
  _3 = this->D.38528._M_impl._M_start;
  _4 = (long int) _3;
  _5 = _2 - _4;
  _6 = _5 /[ex] 40;
  D.46376 = (size_type) _6;
  goto <D.46377>;
  <D.46377>:
  return D.46376;
}



;; Function std::vector<_Tp, _Alloc>::vector(std::vector<_Tp, _Alloc>::size_type, const value_type&, const allocator_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EEC2EmRKS0_RKS1_, funcdef_no=2464, decl_uid=38499, cgraph_uid=1204, symbol_order=1252)

std::vector<_Tp, _Alloc>::vector(std::vector<_Tp, _Alloc>::size_type, const value_type&, const allocator_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this, size_type __n, const struct value_type & __value, const struct allocator_type & __a)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->D.38528;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_base (_1, __n, __a);
  std::vector<outBuff>::_M_fill_initialize (this, __n, __value);
  return;
  <D.46378>:
  _2 = &this->D.38528;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::~_Vector_base (_2);
  resx 1
}



;; Function std::_Vector_base<_Tp, _Alloc>::_Vector_base(std::size_t, const allocator_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EEC2EmRKS1_, funcdef_no=2530, decl_uid=37860, cgraph_uid=1270, symbol_order=1318)

std::_Vector_base<_Tp, _Alloc>::_Vector_base(std::size_t, const allocator_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this, size_t __n, const struct allocator_type & __a)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->_M_impl;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::_Vector_impl (_1, __a);
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_create_storage (this, __n);
  return;
  <D.46379>:
  _2 = &this->_M_impl;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::~_Vector_impl (_2);
  resx 1
}



;; Function std::_Vector_base<_Tp, _Alloc>::_Vector_impl::_Vector_impl(const _Tp_alloc_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE12_Vector_implC2ERKS1_, funcdef_no=2595, decl_uid=37816, cgraph_uid=1335, symbol_order=1383)

std::_Vector_base<_Tp, _Alloc>::_Vector_impl::_Vector_impl(const _Tp_alloc_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_impl * const this, const struct _Tp_alloc_type & __a)
{
  MEM[(struct  &)this] = {CLOBBER};
  std::allocator<outBuff>::allocator (this, __a);
  this->_M_start = 0B;
  this->_M_finish = 0B;
  this->_M_end_of_storage = 0B;
  return;
}



;; Function std::allocator< <template-parameter-1-1> >::allocator(const std::allocator< <template-parameter-1-1> >&) [with _Tp = outBuff] (_ZNSaI7outBuffEC2ERKS0_, funcdef_no=2631, decl_uid=37779, cgraph_uid=1371, symbol_order=1419)

std::allocator< <template-parameter-1-1> >::allocator(const std::allocator< <template-parameter-1-1> >&) [with _Tp = outBuff] (struct allocator * const this, const struct allocator & __a)
{
  __gnu_cxx::new_allocator<outBuff>::new_allocator (this, __a);
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::new_allocator(const __gnu_cxx::new_allocator<_Tp>&) [with _Tp = outBuff] (_ZN9__gnu_cxx13new_allocatorI7outBuffEC2ERKS2_, funcdef_no=2647, decl_uid=37714, cgraph_uid=1387, symbol_order=1435)

__gnu_cxx::new_allocator<_Tp>::new_allocator(const __gnu_cxx::new_allocator<_Tp>&) [with _Tp = outBuff] (struct new_allocator * const this, const struct new_allocator & D.37716)
{
  return;
}



;; Function void std::_Vector_base<_Tp, _Alloc>::_M_create_storage(std::size_t) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE17_M_create_storageEm, funcdef_no=2597, decl_uid=37833, cgraph_uid=1337, symbol_order=1385)

void std::_Vector_base<_Tp, _Alloc>::_M_create_storage(std::size_t) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this, size_t __n)
{
  struct outBuff * D.46380;

  D.46380 = std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_allocate (this, __n);
  _1 = D.46380;
  this->_M_impl._M_start = _1;
  _2 = this->_M_impl._M_start;
  this->_M_impl._M_finish = _2;
  _3 = this->_M_impl._M_start;
  _4 = __n * 40;
  _5 = _3 + _4;
  this->_M_impl._M_end_of_storage = _5;
  return;
}



;; Function std::_Vector_base<_Tp, _Alloc>::pointer std::_Vector_base<_Tp, _Alloc>::_M_allocate(std::size_t) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE11_M_allocateEm, funcdef_no=2633, decl_uid=37826, cgraph_uid=1373, symbol_order=1421)

std::_Vector_base<_Tp, _Alloc>::pointer std::_Vector_base<_Tp, _Alloc>::_M_allocate(std::size_t) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this, size_t __n)
{
  struct outBuff * D.46387;
  struct outBuff * iftmp.346;
  struct outBuff * D.46381;

  if (__n != 0) goto <D.46383>; else goto <D.46384>;
  <D.46383>:
  _1 = &this->_M_impl;
  D.46387 = std::allocator_traits<std::allocator<outBuff> >::allocate (_1, __n);
  iftmp.346 = D.46387;
  goto <D.46385>;
  <D.46384>:
  iftmp.346 = 0B;
  <D.46385>:
  D.46381 = iftmp.346;
  goto <D.46386>;
  <D.46386>:
  return D.46381;
}



;; Function static _Tp* std::allocator_traits<std::allocator<_CharT> >::allocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = outBuff] (_ZNSt16allocator_traitsISaI7outBuffEE8allocateERS1_m, funcdef_no=2649, decl_uid=37467, cgraph_uid=1389, symbol_order=1437)

static _Tp* std::allocator_traits<std::allocator<_CharT> >::allocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = outBuff] (struct allocator_type & __a, size_type __n)
{
  struct outBuff * D.46390;
  struct outBuff * D.46388;

  D.46390 = __gnu_cxx::new_allocator<outBuff>::allocate (__a, __n, 0B);
  D.46388 = D.46390;
  goto <D.46389>;
  <D.46389>:
  return D.46388;
}



;; Function _Tp* __gnu_cxx::new_allocator<_Tp>::allocate(__gnu_cxx::new_allocator<_Tp>::size_type, const void*) [with _Tp = outBuff] (_ZN9__gnu_cxx13new_allocatorI7outBuffE8allocateEmPKv, funcdef_no=2652, decl_uid=37669, cgraph_uid=1392, symbol_order=1440)

_Tp* __gnu_cxx::new_allocator<_Tp>::allocate(__gnu_cxx::new_allocator<_Tp>::size_type, const void*) [with _Tp = outBuff] (struct new_allocator * const this, size_type __n, const void * D.37672)
{
  struct outBuff * D.46396;
  struct outBuff * D.46394;
  bool retval.347;

  _1 = __gnu_cxx::new_allocator<outBuff>::max_size (this);
  retval.347 = __n > _1;
  if (retval.347 != 0) goto <D.46392>; else goto <D.46393>;
  <D.46392>:
  std::__throw_bad_alloc ();
  <D.46393>:
  _2 = __n * 40;
  D.46396 = operator new (_2);
  D.46394 = D.46396;
  goto <D.46395>;
  <D.46395>:
  return D.46394;
}



;; Function __gnu_cxx::new_allocator<_Tp>::size_type __gnu_cxx::new_allocator<_Tp>::max_size() const [with _Tp = outBuff] (_ZNK9__gnu_cxx13new_allocatorI7outBuffE8max_sizeEv, funcdef_no=2653, decl_uid=37677, cgraph_uid=1393, symbol_order=1441)

__gnu_cxx::new_allocator<_Tp>::size_type __gnu_cxx::new_allocator<_Tp>::max_size() const [with _Tp = outBuff] (const struct new_allocator * const this)
{
  size_type D.46397;

  D.46397 = 461168601842738790;
  goto <D.46398>;
  <D.46398>:
  return D.46397;
}



;; Function std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::~_Vector_impl() (_ZNSt12_Vector_baseI7outBuffSaIS0_EE12_Vector_implD2Ev, funcdef_no=2389, decl_uid=41775, cgraph_uid=1128, symbol_order=1176)

std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::~_Vector_impl() (struct _Vector_impl * const this)
{
  std::allocator<outBuff>::~allocator (this);
  MEM[(struct  &)this] = {CLOBBER};
  <D.42299>:
  return;
}



;; Function std::allocator< <template-parameter-1-1> >::~allocator() [with _Tp = outBuff] (_ZNSaI7outBuffED2Ev, funcdef_no=2458, decl_uid=37789, cgraph_uid=1198, symbol_order=1246)

std::allocator< <template-parameter-1-1> >::~allocator() [with _Tp = outBuff] (struct allocator * const this)
{
  __gnu_cxx::new_allocator<outBuff>::~new_allocator (this);
  <D.42981>:
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::~new_allocator() [with _Tp = outBuff] (_ZN9__gnu_cxx13new_allocatorI7outBuffED2Ev, funcdef_no=2526, decl_uid=37724, cgraph_uid=1266, symbol_order=1314)

__gnu_cxx::new_allocator<_Tp>::~new_allocator() [with _Tp = outBuff] (struct new_allocator * const this)
{
  <D.44592>:
  return;
}



;; Function void std::vector<_Tp, _Alloc>::_M_fill_initialize(std::vector<_Tp, _Alloc>::size_type, const value_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE18_M_fill_initializeEmRKS0_, funcdef_no=2532, decl_uid=38232, cgraph_uid=1272, symbol_order=1320)

void std::vector<_Tp, _Alloc>::_M_fill_initialize(std::vector<_Tp, _Alloc>::size_type, const value_type&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this, size_type __n, const struct value_type & __value)
{
  struct outBuff * D.46399;

  _1 = &this->D.38528;
  _2 = std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_get_Tp_allocator (_1);
  _3 = this->D.38528._M_impl._M_start;
  D.46399 = std::__uninitialized_fill_n_a<outBuff*, long unsigned int, outBuff, outBuff> (_3, __n, __value, _2);
  _4 = D.46399;
  this->D.38528._M_impl._M_finish = _4;
  return;
}



;; Function std::_Vector_base<_Tp, _Alloc>::~_Vector_base() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EED2Ev, funcdef_no=2394, decl_uid=37883, cgraph_uid=1134, symbol_order=1182)

std::_Vector_base<_Tp, _Alloc>::~_Vector_base() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this)
{
  _1 = this->_M_impl._M_end_of_storage;
  _2 = (long int) _1;
  _3 = this->_M_impl._M_start;
  _4 = (long int) _3;
  _5 = _2 - _4;
  _6 = _5 /[ex] 40;
  _7 = (long unsigned int) _6;
  _8 = this->_M_impl._M_start;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_deallocate (this, _8, _7);
  _9 = &this->_M_impl;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::~_Vector_impl (_9);
  MEM[(struct  &)this] = {CLOBBER};
  <D.42309>:
  return;
}



;; Function void std::_Vector_base<_Tp, _Alloc>::_M_deallocate(std::_Vector_base<_Tp, _Alloc>::pointer, std::size_t) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE13_M_deallocateEPS0_m, funcdef_no=2460, decl_uid=37829, cgraph_uid=1200, symbol_order=1248)

void std::_Vector_base<_Tp, _Alloc>::_M_deallocate(std::_Vector_base<_Tp, _Alloc>::pointer, std::size_t) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this, struct outBuff * __p, size_t __n)
{
  if (__p != 0B) goto <D.46400>; else goto <D.46401>;
  <D.46400>:
  _1 = &this->_M_impl;
  std::allocator_traits<std::allocator<outBuff> >::deallocate (_1, __p, __n);
  goto <D.46402>;
  <D.46401>:
  <D.46402>:
  return;
}



;; Function static void std::allocator_traits<std::allocator<_CharT> >::deallocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::pointer, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = outBuff] (_ZNSt16allocator_traitsISaI7outBuffEE10deallocateERS1_PS0_m, funcdef_no=2528, decl_uid=37474, cgraph_uid=1268, symbol_order=1316)

static void std::allocator_traits<std::allocator<_CharT> >::deallocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::pointer, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = outBuff] (struct allocator_type & __a, struct outBuff * __p, size_type __n)
{
  __gnu_cxx::new_allocator<outBuff>::deallocate (__a, __p, __n);
  return;
}



;; Function void __gnu_cxx::new_allocator<_Tp>::deallocate(__gnu_cxx::new_allocator<_Tp>::pointer, __gnu_cxx::new_allocator<_Tp>::size_type) [with _Tp = outBuff] (_ZN9__gnu_cxx13new_allocatorI7outBuffE10deallocateEPS1_m, funcdef_no=2593, decl_uid=37673, cgraph_uid=1333, symbol_order=1381)

void __gnu_cxx::new_allocator<_Tp>::deallocate(__gnu_cxx::new_allocator<_Tp>::pointer, __gnu_cxx::new_allocator<_Tp>::size_type) [with _Tp = outBuff] (struct new_allocator * const this, struct outBuff * __p, size_type D.37676)
{
  operator delete (__p);
  return;
}



;; Function void std::_Vector_base<_Tp, _Alloc>::_Vector_impl::_M_swap_data(std::_Vector_base<_Tp, _Alloc>::_Vector_impl&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE12_Vector_impl12_M_swap_dataERS3_, funcdef_no=2469, decl_uid=37804, cgraph_uid=1209, symbol_order=1257)

void std::_Vector_base<_Tp, _Alloc>::_Vector_impl::_M_swap_data(std::_Vector_base<_Tp, _Alloc>::_Vector_impl&) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_impl * const this, struct _Vector_impl & __x)
{
  _1 = &__x->_M_start;
  _2 = &this->_M_start;
  std::swap<outBuff*> (_2, _1);
  _3 = &__x->_M_finish;
  _4 = &this->_M_finish;
  std::swap<outBuff*> (_4, _3);
  _5 = &__x->_M_end_of_storage;
  _6 = &this->_M_end_of_storage;
  std::swap<outBuff*> (_6, _5);
  return;
}



;; Function typename std::enable_if<std::__and_<std::__not_<std::__is_tuple_like<_Tp> >, std::is_move_constructible<_Tp>, std::is_move_assignable<_Tp> >::value>::type std::swap(_Tp&, _Tp&) [with _Tp = outBuff*] (_ZSt4swapIP7outBuffENSt9enable_ifIXsrSt6__and_IJSt6__not_ISt15__is_tuple_likeIT_EESt21is_move_constructibleIS6_ESt18is_move_assignableIS6_EEE5valueEvE4typeERS6_SG_, funcdef_no=2533, decl_uid=43314, cgraph_uid=1274, symbol_order=1322)

typename std::enable_if<std::__and_<std::__not_<std::__is_tuple_like<_Tp> >, std::is_move_constructible<_Tp>, std::is_move_assignable<_Tp> >::value>::type std::swap(_Tp&, _Tp&) [with _Tp = outBuff*] (struct outBuff * & __a, struct outBuff * & __b)
{
  struct outBuff * __tmp;

  _1 = std::move<outBuff*&> (__a);
  _2 = *_1;
  __tmp = _2;
  _3 = std::move<outBuff*&> (__b);
  _4 = *_3;
  *__a = _4;
  _5 = std::move<outBuff*&> (&__tmp);
  _6 = *_5;
  *__b = _6;
  __tmp = {CLOBBER};
  return;
}



;; Function constexpr typename std::remove_reference< <template-parameter-1-1> >::type&& std::move(_Tp&&) [with _Tp = outBuff*&] (_ZSt4moveIRP7outBuffEONSt16remove_referenceIT_E4typeEOS4_, funcdef_no=2534, decl_uid=44606, cgraph_uid=1273, symbol_order=1321)

constexpr typename std::remove_reference< <template-parameter-1-1> >::type&& std::move(_Tp&&) [with _Tp = outBuff*&] (struct outBuff * & __t)
{
  struct outBuff * & D.46403;

  D.46403 = __t;
  goto <D.46404>;
  <D.46404>:
  return D.46403;
}



;; Function std::vector<_Tp, _Alloc>::~vector() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EED2Ev, funcdef_no=2467, decl_uid=38525, cgraph_uid=1207, symbol_order=1255)

std::vector<_Tp, _Alloc>::~vector() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this)
{
  _1 = &this->D.38528;
  _2 = std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_get_Tp_allocator (_1);
  _3 = this->D.38528._M_impl._M_finish;
  _4 = this->D.38528._M_impl._M_start;
  std::_Destroy<outBuff*, outBuff> (_4, _3, _2);
  _5 = &this->D.38528;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::~_Vector_base (_5);
  MEM[(struct  &)this] = {CLOBBER};
  <D.42995>:
  return;
}



;; Function void std::_Destroy(_ForwardIterator, _ForwardIterator, std::allocator<_T2>&) [with _ForwardIterator = outBuff*; _Tp = outBuff] (_ZSt8_DestroyIP7outBuffS0_EvT_S2_RSaIT0_E, funcdef_no=2480, decl_uid=42508, cgraph_uid=1220, symbol_order=1268)

void std::_Destroy(_ForwardIterator, _ForwardIterator, std::allocator<_T2>&) [with _ForwardIterator = outBuff*; _Tp = outBuff] (struct outBuff * __first, struct outBuff * __last, struct allocator & D.42511)
{
  std::_Destroy<outBuff*> (__first, __last);
  return;
}



;; Function void std::_Destroy(_ForwardIterator, _ForwardIterator) [with _ForwardIterator = outBuff*] (_ZSt8_DestroyIP7outBuffEvT_S2_, funcdef_no=2552, decl_uid=43587, cgraph_uid=1292, symbol_order=1340)

void std::_Destroy(_ForwardIterator, _ForwardIterator) [with _ForwardIterator = outBuff*] (struct outBuff * __first, struct outBuff * __last)
{
  std::_Destroy_aux<true>::__destroy<outBuff*> (__first, __last);
  return;
}



;; Function static void std::_Destroy_aux<true>::__destroy(_ForwardIterator, _ForwardIterator) [with _ForwardIterator = outBuff*] (_ZNSt12_Destroy_auxILb1EE9__destroyIP7outBuffEEvT_S4_, funcdef_no=2610, decl_uid=44983, cgraph_uid=1350, symbol_order=1398)

static void std::_Destroy_aux<true>::__destroy(_ForwardIterator, _ForwardIterator) [with _ForwardIterator = outBuff*] (struct outBuff * D.44984, struct outBuff * D.44985)
{
  GIMPLE_NOP
  return;
}



;; Function std::vector<_Tp, _Alloc>::iterator std::vector<_Tp, _Alloc>::end() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE3endEv, funcdef_no=2471, decl_uid=38026, cgraph_uid=1211, symbol_order=1259)

std::vector<_Tp, _Alloc>::iterator std::vector<_Tp, _Alloc>::end() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this)
{
  struct iterator D.43354;
  struct iterator D.46405;

  _1 = &this->D.38528._M_impl._M_finish;
  __gnu_cxx::__normal_iterator<outBuff*, std::vector<outBuff> >::__normal_iterator (&D.43354, _1);
  D.46405 = D.43354;
  goto <D.46407>;
  <D.46407>:
  D.43354 = {CLOBBER};
  goto <D.46406>;
  <D.46406>:
  return D.46405;
}



;; Function __gnu_cxx::__normal_iterator<_Iterator, _Container>::__normal_iterator(const _Iterator&) [with _Iterator = outBuff*; _Container = std::vector<outBuff>] (_ZN9__gnu_cxx17__normal_iteratorIP7outBuffSt6vectorIS1_SaIS1_EEEC2ERKS2_, funcdef_no=2536, decl_uid=42399, cgraph_uid=1276, symbol_order=1324)

__gnu_cxx::__normal_iterator<_Iterator, _Container>::__normal_iterator(const _Iterator&) [with _Iterator = outBuff*; _Container = std::vector<outBuff>] (struct __normal_iterator * const this, struct outBuff * const & __i)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = *__i;
  this->_M_current = _1;
  return;
}



;; Function std::vector<_Tp, _Alloc>::iterator std::vector<_Tp, _Alloc>::begin() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE5beginEv, funcdef_no=2470, decl_uid=38022, cgraph_uid=1210, symbol_order=1258)

std::vector<_Tp, _Alloc>::iterator std::vector<_Tp, _Alloc>::begin() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this)
{
  struct iterator D.43352;
  struct iterator D.46408;

  _1 = &this->D.38528._M_impl._M_start;
  __gnu_cxx::__normal_iterator<outBuff*, std::vector<outBuff> >::__normal_iterator (&D.43352, _1);
  D.46408 = D.43352;
  goto <D.46410>;
  <D.46410>:
  D.43352 = {CLOBBER};
  goto <D.46409>;
  <D.46409>:
  return D.46408;
}



;; Function void std::fill(_ForwardIterator, _ForwardIterator, const _Tp&) [with _ForwardIterator = __gnu_cxx::__normal_iterator<outBuff*, std::vector<outBuff> >; _Tp = outBuff] (_ZSt4fillIN9__gnu_cxx17__normal_iteratorIP7outBuffSt6vectorIS2_SaIS2_EEEES2_EvT_S8_RKT0_, funcdef_no=2472, decl_uid=42418, cgraph_uid=1212, symbol_order=1260)

void std::fill(_ForwardIterator, _ForwardIterator, const _Tp&) [with _ForwardIterator = __gnu_cxx::__normal_iterator<outBuff*, std::vector<outBuff> >; _Tp = outBuff] (struct __normal_iterator __first, struct __normal_iterator __last, const struct outBuff & __value)
{
  _1 = std::__niter_base<outBuff*, std::vector<outBuff> > (__last);
  _2 = std::__niter_base<outBuff*, std::vector<outBuff> > (__first);
  std::__fill_a<outBuff*, outBuff> (_2, _1, __value);
  return;
}



;; Function _Iterator std::__niter_base(__gnu_cxx::__normal_iterator<_Iterator, _Container>) [with _Iterator = outBuff*; _Container = std::vector<outBuff>] (_ZSt12__niter_baseIP7outBuffSt6vectorIS0_SaIS0_EEET_N9__gnu_cxx17__normal_iteratorIS5_T0_EE, funcdef_no=2538, decl_uid=43358, cgraph_uid=1278, symbol_order=1326)

_Iterator std::__niter_base(__gnu_cxx::__normal_iterator<_Iterator, _Container>) [with _Iterator = outBuff*; _Container = std::vector<outBuff>] (struct __normal_iterator __it)
{
  struct outBuff * D.46411;

  _1 = __gnu_cxx::__normal_iterator<outBuff*, std::vector<outBuff> >::base (&__it);
  D.46411 = *_1;
  goto <D.46412>;
  <D.46412>:
  return D.46411;
}



;; Function const _Iterator& __gnu_cxx::__normal_iterator<_Iterator, _Container>::base() const [with _Iterator = outBuff*; _Container = std::vector<outBuff>] (_ZNK9__gnu_cxx17__normal_iteratorIP7outBuffSt6vectorIS1_SaIS1_EEE4baseEv, funcdef_no=2598, decl_uid=42386, cgraph_uid=1338, symbol_order=1386)

const _Iterator& __gnu_cxx::__normal_iterator<_Iterator, _Container>::base() const [with _Iterator = outBuff*; _Container = std::vector<outBuff>] (const struct __normal_iterator * const this)
{
  struct outBuff * const & D.46413;

  D.46413 = &this->_M_current;
  goto <D.46414>;
  <D.46414>:
  return D.46413;
}



;; Function typename __gnu_cxx::__enable_if<(! std::__is_scalar<_Tp>::__value), void>::__type std::__fill_a(_ForwardIterator, _ForwardIterator, const _Tp&) [with _ForwardIterator = outBuff*; _Tp = outBuff] (_ZSt8__fill_aIP7outBuffS0_EN9__gnu_cxx11__enable_ifIXntsrSt11__is_scalarIT0_E7__valueEvE6__typeET_S9_RKS5_, funcdef_no=2539, decl_uid=43406, cgraph_uid=1279, symbol_order=1327)

typename __gnu_cxx::__enable_if<(! std::__is_scalar<_Tp>::__value), void>::__type std::__fill_a(_ForwardIterator, _ForwardIterator, const _Tp&) [with _ForwardIterator = outBuff*; _Tp = outBuff] (struct outBuff * __first, struct outBuff * __last, const struct outBuff & __value)
{
  <D.46415>:
  if (__first == __last) goto <D.44614>; else goto <D.46416>;
  <D.46416>:
  *__first = *__value;
  __first = __first + 40;
  goto <D.46415>;
  <D.44614>:
  return;
}



;; Function std::_Vector_base<_Tp, _Alloc>::_Tp_alloc_type& std::_Vector_base<_Tp, _Alloc>::_M_get_Tp_allocator() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE19_M_get_Tp_allocatorEv, funcdef_no=2462, decl_uid=37590, cgraph_uid=1202, symbol_order=1250)

std::_Vector_base<_Tp, _Alloc>::_Tp_alloc_type& std::_Vector_base<_Tp, _Alloc>::_M_get_Tp_allocator() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this)
{
  struct _Tp_alloc_type & D.46417;

  D.46417 = &this->_M_impl;
  goto <D.46418>;
  <D.46418>:
  return D.46417;
}



;; Function _ForwardIterator std::__uninitialized_fill_n_a(_ForwardIterator, _Size, const _Tp&, std::allocator<_Tp2>&) [with _ForwardIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff; _Tp2 = outBuff] (_ZSt24__uninitialized_fill_n_aIP7outBuffmS0_S0_ET_S2_T0_RKT1_RSaIT2_E, funcdef_no=2473, decl_uid=42459, cgraph_uid=1213, symbol_order=1261)

_ForwardIterator std::__uninitialized_fill_n_a(_ForwardIterator, _Size, const _Tp&, std::allocator<_Tp2>&) [with _ForwardIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff; _Tp2 = outBuff] (struct outBuff * __first, long unsigned int __n, const struct outBuff & __x, struct allocator & D.42463)
{
  struct outBuff * D.46421;
  struct outBuff * D.46419;

  D.46421 = std::uninitialized_fill_n<outBuff*, long unsigned int, outBuff> (__first, __n, __x);
  D.46419 = D.46421;
  goto <D.46420>;
  <D.46420>:
  return D.46419;
}



;; Function _ForwardIterator std::uninitialized_fill_n(_ForwardIterator, _Size, const _Tp&) [with _ForwardIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff] (_ZSt20uninitialized_fill_nIP7outBuffmS0_ET_S2_T0_RKT1_, funcdef_no=2540, decl_uid=43411, cgraph_uid=1280, symbol_order=1328)

_ForwardIterator std::uninitialized_fill_n(_ForwardIterator, _Size, const _Tp&) [with _ForwardIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff] (struct outBuff * __first, long unsigned int __n, const struct outBuff & __x)
{
  struct outBuff * D.46424;
  const bool __assignable;
  struct outBuff * D.46422;

  __assignable = 1;
  D.46424 = std::__uninitialized_fill_n<false>::__uninit_fill_n<outBuff*, long unsigned int, outBuff> (__first, __n, __x);
  D.46422 = D.46424;
  goto <D.46423>;
  <D.46423>:
  return D.46422;
}



;; Function static _ForwardIterator std::__uninitialized_fill_n<_TrivialValueType>::__uninit_fill_n(_ForwardIterator, _Size, const _Tp&) [with _ForwardIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff; bool _TrivialValueType = false] (_ZNSt22__uninitialized_fill_nILb0EE15__uninit_fill_nIP7outBuffmS2_EET_S4_T0_RKT1_, funcdef_no=2599, decl_uid=44822, cgraph_uid=1339, symbol_order=1387)

static _ForwardIterator std::__uninitialized_fill_n<_TrivialValueType>::__uninit_fill_n(_ForwardIterator, _Size, const _Tp&) [with _ForwardIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff; bool _TrivialValueType = false] (struct outBuff * __first, long unsigned int __n, const struct outBuff & __x)
{
  struct outBuff * __cur;
  struct outBuff * D.46427;

  __cur = __first;
  <D.46425>:
  if (__n == 0) goto <D.45129>; else goto <D.46426>;
  <D.46426>:
  _1 = std::__addressof<outBuff> (__cur);
  std::_Construct<outBuff, const outBuff&> (_1, __x);
  __n = __n + 18446744073709551615;
  __cur = __cur + 40;
  goto <D.46425>;
  <D.45129>:
  D.46427 = __cur;
  goto <D.46428>;
  <D.46428>:
  return D.46427;
}



;; Function constexpr _Tp* std::__addressof(_Tp&) [with _Tp = outBuff] (_ZSt11__addressofI7outBuffEPT_RS1_, funcdef_no=2634, decl_uid=45124, cgraph_uid=1374, symbol_order=1422)

constexpr _Tp* std::__addressof(_Tp&) [with _Tp = outBuff] (struct outBuff & __r)
{
  struct outBuff * D.46429;

  D.46429 = __r;
  goto <D.46430>;
  <D.46430>:
  return D.46429;
}



;; Function void std::_Construct(_T1*, _Args&& ...) [with _T1 = outBuff; _Args = {const outBuff&}] (_ZSt10_ConstructI7outBuffJRKS0_EEvPT_DpOT0_, funcdef_no=2635, decl_uid=45126, cgraph_uid=1376, symbol_order=1424)

void std::_Construct(_T1*, _Args&& ...) [with _T1 = outBuff; _Args = {const outBuff&}] (struct outBuff * __p, const struct outBuff & __args#0)
{
  struct outBuff * iftmp.348;
  void * D.45256;
  void * D.45255;
  const struct outBuff * D.45258;

  D.45258 = std::forward<const outBuff&> (__args#0);
  D.45255 = __p;
  D.45256 = operator new (40, D.45255);
  if (D.45256 != 0B) goto <D.46432>; else goto <D.46433>;
  <D.46432>:
  MEM[(struct outBuff *)D.45256] = *D.45258;
  iftmp.348 = D.45256;
  goto <D.46434>;
  <D.46433>:
  iftmp.348 = D.45256;
  <D.46434>:
  return;
}



;; Function constexpr _Tp&& std::forward(typename std::remove_reference<_From>::type&) [with _Tp = const outBuff&] (_ZSt7forwardIRK7outBuffEOT_RNSt16remove_referenceIS3_E4typeE, funcdef_no=2636, decl_uid=45235, cgraph_uid=1375, symbol_order=1423)

constexpr _Tp&& std::forward(typename std::remove_reference<_From>::type&) [with _Tp = const outBuff&] (const struct type & __t)
{
  const struct outBuff & D.46435;

  D.46435 = __t;
  goto <D.46436>;
  <D.46436>:
  return D.46435;
}



;; Function void* operator new(std::size_t, void*) (_ZnwmPv, funcdef_no=465, decl_uid=11344, cgraph_uid=144, symbol_order=145)

void* operator new(std::size_t, void*) (size_t D.11342, void * __p)
{
  void * D.46437;

  D.46437 = __p;
  goto <D.46438>;
  <D.46438>:
  return D.46437;
}



;; Function _OI std::fill_n(_OI, _Size, const _Tp&) [with _OI = outBuff*; _Size = long unsigned int; _Tp = outBuff] (_ZSt6fill_nIP7outBuffmS0_ET_S2_T0_RKT1_, funcdef_no=2474, decl_uid=42469, cgraph_uid=1214, symbol_order=1262)

_OI std::fill_n(_OI, _Size, const _Tp&) [with _OI = outBuff*; _Size = long unsigned int; _Tp = outBuff] (struct outBuff * __first, long unsigned int __n, const struct outBuff & __value)
{
  struct outBuff * D.46439;

  _1 = std::__niter_base<outBuff*> (__first);
  D.46439 = std::__fill_n_a<outBuff*, long unsigned int, outBuff> (_1, __n, __value);
  goto <D.46440>;
  <D.46440>:
  return D.46439;
}



;; Function _Iterator std::__niter_base(_Iterator) [with _Iterator = outBuff*] (_ZSt12__niter_baseIP7outBuffET_S2_, funcdef_no=2541, decl_uid=43416, cgraph_uid=1281, symbol_order=1329)

_Iterator std::__niter_base(_Iterator) [with _Iterator = outBuff*] (struct outBuff * __it)
{
  struct outBuff * D.46441;

  D.46441 = __it;
  goto <D.46442>;
  <D.46442>:
  return D.46441;
}



;; Function typename __gnu_cxx::__enable_if<(! std::__is_scalar<_Tp>::__value), _OutputIterator>::__type std::__fill_n_a(_OutputIterator, _Size, const _Tp&) [with _OutputIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff] (_ZSt10__fill_n_aIP7outBuffmS0_EN9__gnu_cxx11__enable_ifIXntsrSt11__is_scalarIT1_E7__valueET_E6__typeES7_T0_RKS5_, funcdef_no=2542, decl_uid=43425, cgraph_uid=1282, symbol_order=1330)

typename __gnu_cxx::__enable_if<(! std::__is_scalar<_Tp>::__value), _OutputIterator>::__type std::__fill_n_a(_OutputIterator, _Size, const _Tp&) [with _OutputIterator = outBuff*; _Size = long unsigned int; _Tp = outBuff] (struct outBuff * __first, long unsigned int __n, const struct outBuff & __value)
{
  long unsigned int __niter;
  struct outBuff * D.46445;

  __niter = __n;
  <D.46443>:
  if (__niter == 0) goto <D.44829>; else goto <D.46444>;
  <D.46444>:
  *__first = *__value;
  __niter = __niter + 18446744073709551615;
  __first = __first + 40;
  goto <D.46443>;
  <D.44829>:
  D.46445 = __first;
  goto <D.46446>;
  <D.46446>:
  return D.46445;
}



;; Function void std::vector<_Tp, _Alloc>::_M_erase_at_end(std::vector<_Tp, _Alloc>::pointer) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE15_M_erase_at_endEPS0_, funcdef_no=2409, decl_uid=38407, cgraph_uid=1149, symbol_order=1197)

void std::vector<_Tp, _Alloc>::_M_erase_at_end(std::vector<_Tp, _Alloc>::pointer) [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this, struct outBuff * __pos)
{
  _1 = &this->D.38528;
  _2 = std::_Vector_base<outBuff, std::allocator<outBuff> >::_M_get_Tp_allocator (_1);
  _3 = this->D.38528._M_impl._M_finish;
  std::_Destroy<outBuff*, outBuff> (__pos, _3, _2);
  this->D.38528._M_impl._M_finish = __pos;
  return;
}



;; Function std::vector<_Tp, _Alloc>::size_type std::vector<_Tp, _Alloc>::size() const [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNKSt6vectorI7outBuffSaIS0_EE4sizeEv, funcdef_no=2257, decl_uid=38046, cgraph_uid=997, symbol_order=1045)

std::vector<_Tp, _Alloc>::size_type std::vector<_Tp, _Alloc>::size() const [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (const struct vector * const this)
{
  size_type D.46447;

  _1 = this->D.38528._M_impl._M_finish;
  _2 = (long int) _1;
  _3 = this->D.38528._M_impl._M_start;
  _4 = (long int) _3;
  _5 = _2 - _4;
  _6 = _5 /[ex] 40;
  D.46447 = (size_type) _6;
  goto <D.46448>;
  <D.46448>:
  return D.46447;
}



;; Function outBuff* outputBufferSeqAddNext(outBuff*, outBuff*) (_Z22outputBufferSeqAddNextP7outBuffS0_, funcdef_no=1996, decl_uid=39032, cgraph_uid=736, symbol_order=784)

outBuff* outputBufferSeqAddNext(outBuff*, outBuff*) (struct outBuff * preveElement, struct outBuff * newElement)
{
  int D.46464;
  struct outBuff * D.46455;
  bool retval.352;

  OutMutex.349_1 = OutMutex;
  safe_mutex_lock (OutMutex.349_1);
  <D.46449>:
  NumBufferedTailBlocks.350_2 = NumBufferedTailBlocks;
  NumBufferedBlocksMax.351_3 = NumBufferedBlocksMax;
  if (NumBufferedTailBlocks.350_2 < NumBufferedBlocksMax.351_3) goto <D.39602>; else goto <D.46450>;
  <D.46450>:
  _4 = preveElement->buf;
  if (_4 == 0B) goto <D.39602>; else goto <D.46451>;
  <D.46451>:
  D.46464 = syncGetTerminateFlag ();
  _5 = D.46464;
  retval.352 = _5 != 0;
  if (retval.352 != 0) goto <D.46453>; else goto <D.46454>;
  <D.46453>:
  OutMutex.353_6 = OutMutex;
  pthread_mutex_unlock (OutMutex.353_6);
  D.46455 = 0B;
  goto <D.46463>;
  <D.46454>:
  LastGoodBlock.354_7 = LastGoodBlock;
  if (LastGoodBlock.354_7 != -1) goto <D.46456>; else goto <D.46457>;
  <D.46456>:
  _8 = newElement->blockNumber;
  LastGoodBlock.355_9 = LastGoodBlock;
  if (_8 > LastGoodBlock.355_9) goto <D.46458>; else goto <D.46459>;
  <D.46458>:
  OutMutex.356_10 = OutMutex;
  pthread_mutex_unlock (OutMutex.356_10);
  D.46455 = 0B;
  goto <D.46463>;
  <D.46459>:
  <D.46457>:
  OutMutex.357_11 = OutMutex;
  notTooMuchNumBuffered.358_12 = notTooMuchNumBuffered;
  safe_cond_wait (notTooMuchNumBuffered.358_12, OutMutex.357_11);
  goto <D.46449>;
  <D.39602>:
  preveElement->next = newElement;
  NumBufferedTailBlocks.359_13 = NumBufferedTailBlocks;
  _14 = NumBufferedTailBlocks.359_13 + 1;
  NumBufferedTailBlocks = _14;
  _15 = preveElement->buf;
  if (_15 == 0B) goto <D.46460>; else goto <D.46461>;
  <D.46460>:
  safe_cond_signal (&OutBufferHeadNotEmpty);
  goto <D.46462>;
  <D.46461>:
  <D.46462>:
  OutMutex.360_16 = OutMutex;
  safe_mutex_unlock (OutMutex.360_16);
  D.46455 = newElement;
  goto <D.46463>;
  <D.46463>:
  return D.46455;
}



;; Function outBuff* outputBufferAdd(const outBuff&, const char*) (_Z15outputBufferAddRK7outBuffPKc, funcdef_no=1997, decl_uid=39029, cgraph_uid=737, symbol_order=785)

outBuff* outputBufferAdd(const outBuff&, const char*) (const struct outBuff & element, const char * caller)
{
  int D.46480;
  size_t outBuffPos;
  int dist;
  struct value_type & D.46475;
  struct outBuff * D.46470;
  bool retval.364;

  OutMutex.361_1 = OutMutex;
  safe_mutex_lock (OutMutex.361_1);
  _2 = element->blockNumber;
  _3 = (unsigned int) _2;
  NumBufferedBlocksMax.362_4 = NumBufferedBlocksMax;
  _5 = (unsigned int) NumBufferedBlocksMax.362_4;
  _6 = _3 - _5;
  dist = (int) _6;
  <D.46465>:
  NextBlockToWrite.363_7 = NextBlockToWrite;
  if (dist < NextBlockToWrite.363_7) goto <D.39616>; else goto <D.46466>;
  <D.46466>:
  D.46480 = syncGetTerminateFlag ();
  _8 = D.46480;
  retval.364 = _8 != 0;
  if (retval.364 != 0) goto <D.46468>; else goto <D.46469>;
  <D.46468>:
  OutMutex.365_9 = OutMutex;
  pthread_mutex_unlock (OutMutex.365_9);
  D.46470 = 0B;
  goto <D.46479>;
  <D.46469>:
  LastGoodBlock.366_10 = LastGoodBlock;
  if (LastGoodBlock.366_10 != -1) goto <D.46471>; else goto <D.46472>;
  <D.46471>:
  _11 = element->blockNumber;
  LastGoodBlock.367_12 = LastGoodBlock;
  if (_11 > LastGoodBlock.367_12) goto <D.46473>; else goto <D.46474>;
  <D.46473>:
  OutMutex.368_13 = OutMutex;
  pthread_mutex_unlock (OutMutex.368_13);
  D.46470 = 0B;
  goto <D.46479>;
  <D.46474>:
  <D.46472>:
  OutMutex.369_14 = OutMutex;
  notTooMuchNumBuffered.370_15 = notTooMuchNumBuffered;
  safe_cond_wait (notTooMuchNumBuffered.370_15, OutMutex.369_14);
  goto <D.46465>;
  <D.39616>:
  _16 = element->blockNumber;
  outBuffPos = getOutputBufferPos (_16);
  D.46475 = std::vector<outBuff>::operator[] (&OutputBuffer, outBuffPos);
  *D.46475 = MEM[(const struct outBuff &)element];
  NumBufferedBlocks.371_17 = NumBufferedBlocks;
  _18 = NumBufferedBlocks.371_17 + 1;
  NumBufferedBlocks = _18;
  _19 = element->blockNumber;
  NextBlockToWrite.372_20 = NextBlockToWrite;
  if (_19 == NextBlockToWrite.372_20) goto <D.46476>; else goto <D.46477>;
  <D.46476>:
  safe_cond_signal (&OutBufferHeadNotEmpty);
  goto <D.46478>;
  <D.46477>:
  <D.46478>:
  OutMutex.373_21 = OutMutex;
  safe_mutex_unlock (OutMutex.373_21);
  D.46470 = std::vector<outBuff>::operator[] (&OutputBuffer, outBuffPos);
  goto <D.46479>;
  <D.46479>:
  return D.46470;
}



;; Function size_t getOutputBufferPos(int) (_Z18getOutputBufferPosi, funcdef_no=1995, decl_uid=39034, cgraph_uid=735, symbol_order=783)

size_t getOutputBufferPos(int) (int blockNum)
{
  size_t outBuffPos;
  size_t D.46484;

  _1 = (long unsigned int) blockNum;
  OutBufferPosToWrite.374_2 = OutBufferPosToWrite;
  _3 = _1 + OutBufferPosToWrite.374_2;
  NextBlockToWrite.375_4 = NextBlockToWrite;
  _5 = (long unsigned int) NextBlockToWrite.375_4;
  outBuffPos = _3 - _5;
  NumBufferedBlocksMax.376_6 = NumBufferedBlocksMax;
  if (outBuffPos >= NumBufferedBlocksMax.376_6) goto <D.46481>; else goto <D.46482>;
  <D.46481>:
  NumBufferedBlocksMax.377_7 = NumBufferedBlocksMax;
  outBuffPos = outBuffPos - NumBufferedBlocksMax.377_7;
  goto <D.46483>;
  <D.46482>:
  <D.46483>:
  D.46484 = outBuffPos;
  goto <D.46485>;
  <D.46485>:
  return D.46484;
}



;; Function int testBZ2ErrorHandling(int, BZFILE*, int) (_Z20testBZ2ErrorHandlingiPvi, funcdef_no=1998, decl_uid=39042, cgraph_uid=738, symbol_order=786)

int testBZ2ErrorHandling(int, BZFILE*, int) (int bzerr, BZFILE * bzf, int streamNo)
{
  int bzerr_dummy;
  int D.46486;

  BZ2_bzReadClose (&bzerr_dummy, bzf);
  switch (bzerr) <default: <D.39630>, case -9: <D.39624>, case -7: <D.39628>, case -6: <D.39625>, case -5: <D.39629>, case -4: <D.39626>, case -3: <D.39627>>
  <D.39624>:
  stderr.378_1 = stderr;
  fprintf (stderr.378_1, "pbzip2: *ERROR: Integers are not the right size for libbzip2. Aborting!\n");
  exit (3);
  <D.39625>:
  stderr.379_2 = stderr;
  fprintf (stderr.379_2, "pbzip2: *ERROR: Integers are not the right size for libbzip2. Aborting!\n");
  D.46486 = 1;
  goto <D.46491>;
  <D.39626>:
  stderr.380_3 = stderr;
  fprintf (stderr.380_3, "pbzip2: *ERROR: Data integrity (CRC) error in data!  Skipping...\n");
  D.46486 = -1;
  goto <D.46491>;
  <D.39627>:
  stderr.381_4 = stderr;
  fprintf (stderr.381_4, "pbzip2: *ERROR: Could NOT allocate enough memory. Aborting!\n");
  D.46486 = 1;
  goto <D.46491>;
  <D.39628>:
  stderr.382_5 = stderr;
  fprintf (stderr.382_5, "pbzip2: *ERROR: File ends unexpectedly!  Skipping...\n");
  D.46486 = -1;
  goto <D.46491>;
  <D.39629>:
  if (streamNo == 1) goto <D.46487>; else goto <D.46488>;
  <D.46487>:
  stderr.383_6 = stderr;
  fprintf (stderr.383_6, "pbzip2: *ERROR: Bad magic number (file not created by bzip2)!  Skipping...\n");
  D.46486 = -1;
  goto <D.46491>;
  <D.46488>:
  stderr.384_7 = stderr;
  fprintf (stderr.384_7, "pbzip2: *WARNING: Trailing garbage after EOF ignored!\n");
  D.46486 = 0;
  goto <D.46491>;
  <D.39630>:
  stderr.385_8 = stderr;
  fprintf (stderr.385_8, "pbzip2: *ERROR: Unexpected error. Aborting!\n");
  exit (3);
  D.46486 = 0;
  goto <D.46491>;
  <D.46491>:
  bzerr_dummy = {CLOBBER};
  goto <D.46489>;
  <D.46489>:
  return D.46486;
  <D.46490>:
  bzerr_dummy = {CLOBBER};
  resx 1
}



;; Function int testCompressedData(char*) (_Z18testCompressedDataPc, funcdef_no=1999, decl_uid=39044, cgraph_uid=739, symbol_order=787)

int testCompressedData(char*) (char * fileName)
{
  int D.46551;
  struct ErrorContext * D.46550;
  int D.46549;
  int D.46548;
  int D.46547;
  int D.46546;
  int D.46545;
  BZFILE * D.46544;
  struct ErrorContext * D.46543;
  struct FILE * D.46542;
  int c;
  int i;
  int nUnused;
  int streamNo;
  int nread;
  int bzerr;
  unsigned char * unusedTmp;
  unsigned char unused[5000];
  unsigned char obuf[5000];
  BZFILE * bzf;
  int ret;
  struct FILE * zStream;
  bool retval.407;
  bool retval.386;
  int D.46496;

  zStream = 0B;
  ret = 0;
  bzf = 0B;
  nUnused = 0;
  streamNo = 0;
  _1 = strcmp (fileName, "-");
  if (_1 != 0) goto <D.46492>; else goto <D.46493>;
  <D.46492>:
  D.46542 = fopen (fileName, "rb");
  zStream = D.46542;
  if (zStream == 0B) goto <D.46494>; else goto <D.46495>;
  <D.46494>:
  D.46543 = pbzip2::ErrorContext::getInstance ();
  _2 = D.46543;
  pbzip2::ErrorContext::saveError (_2);
  handle_error (0, -1, "pbzip2: *ERROR: Could not open input file [%s]!  Skipping...\n", fileName);
  D.46496 = -1;
  goto <D.46553>;
  <D.46495>:
  goto <D.46497>;
  <D.46493>:
  zStream = stdin;
  <D.46497>:
  _3 = ferror (zStream);
  retval.386 = _3 != 0;
  if (retval.386 != 0) goto <D.46499>; else goto <D.46500>;
  <D.46499>:
  handle_error (0, -1, "pbzip2: *ERROR: Problem with stream of file [%s]!  Skipping...\n", fileName);
  stdin.387_4 = stdin;
  if (zStream != stdin.387_4) goto <D.46501>; else goto <D.46502>;
  <D.46501>:
  verbose_fclose (zStream, fileName);
  goto <D.46503>;
  <D.46502>:
  <D.46503>:
  D.46496 = -1;
  goto <D.46553>;
  <D.46500>:
  <D.46504>:
  nUnused.388_5 = nUnused;
  Verbosity.389_6 = Verbosity;
  D.46544 = BZ2_bzReadOpen (&bzerr, zStream, Verbosity.389_6, 0, &unused, nUnused.388_5);
  bzf = D.46544;
  if (bzf == 0B) goto <D.46505>; else goto <D.46507>;
  <D.46507>:
  bzerr.390_7 = bzerr;
  if (bzerr.390_7 != 0) goto <D.46505>; else goto <D.46506>;
  <D.46505>:
  bzerr.391_8 = bzerr;
  D.46545 = testBZ2ErrorHandling (bzerr.391_8, bzf, streamNo);
  ret = D.46545;
  stdin.392_9 = stdin;
  if (zStream != stdin.392_9) goto <D.46508>; else goto <D.46509>;
  <D.46508>:
  verbose_fclose (zStream, fileName);
  goto <D.46510>;
  <D.46509>:
  <D.46510>:
  D.46496 = ret;
  goto <D.46553>;
  <D.46506>:
  streamNo = streamNo + 1;
  <D.46511>:
  bzerr.393_10 = bzerr;
  if (bzerr.393_10 != 0) goto <D.39649>; else goto <D.46512>;
  <D.46512>:
  D.46546 = BZ2_bzRead (&bzerr, bzf, &obuf, 5000);
  nread = D.46546;
  bzerr.394_11 = bzerr;
  if (bzerr.394_11 == -5) goto <D.46513>; else goto <D.46514>;
  <D.46513>:
  bzerr.395_12 = bzerr;
  D.46547 = testBZ2ErrorHandling (bzerr.395_12, bzf, streamNo);
  ret = D.46547;
  stdin.396_13 = stdin;
  if (zStream != stdin.396_13) goto <D.46515>; else goto <D.46516>;
  <D.46515>:
  verbose_fclose (zStream, fileName);
  goto <D.46517>;
  <D.46516>:
  <D.46517>:
  D.46496 = ret;
  goto <D.46553>;
  <D.46514>:
  goto <D.46511>;
  <D.39649>:
  bzerr.397_14 = bzerr;
  if (bzerr.397_14 != 4) goto <D.46518>; else goto <D.46519>;
  <D.46518>:
  bzerr.398_15 = bzerr;
  D.46548 = testBZ2ErrorHandling (bzerr.398_15, bzf, streamNo);
  ret = D.46548;
  stdin.399_16 = stdin;
  if (zStream != stdin.399_16) goto <D.46520>; else goto <D.46521>;
  <D.46520>:
  verbose_fclose (zStream, fileName);
  goto <D.46522>;
  <D.46521>:
  <D.46522>:
  D.46496 = ret;
  goto <D.46553>;
  <D.46519>:
  BZ2_bzReadGetUnused (&bzerr, bzf, &unusedTmp, &nUnused);
  bzerr.400_17 = bzerr;
  if (bzerr.400_17 != 0) goto <D.46523>; else goto <D.46524>;
  <D.46523>:
  stderr.401_18 = stderr;
  fprintf (stderr.401_18, "pbzip2: *ERROR: Unexpected error. Aborting!\n");
  exit (3);
  <D.46524>:
  i = 0;
  <D.46525>:
  nUnused.402_19 = nUnused;
  if (i >= nUnused.402_19) goto <D.39651>; else goto <D.46526>;
  <D.46526>:
  unusedTmp.403_20 = unusedTmp;
  _21 = (sizetype) i;
  _22 = unusedTmp.403_20 + _21;
  _23 = *_22;
  unused[i] = _23;
  i = i + 1;
  goto <D.46525>;
  <D.39651>:
  BZ2_bzReadClose (&bzerr, bzf);
  bzerr.404_24 = bzerr;
  if (bzerr.404_24 != 0) goto <D.46527>; else goto <D.46528>;
  <D.46527>:
  stderr.405_25 = stderr;
  fprintf (stderr.405_25, "pbzip2: *ERROR: Unexpected error. Aborting!\n");
  exit (3);
  <D.46528>:
  nUnused.406_26 = nUnused;
  if (nUnused.406_26 == 0) goto <D.46529>; else goto <D.46530>;
  <D.46529>:
  D.46549 = fgetc (zStream);
  c = D.46549;
  if (c == -1) goto <D.39647>; else goto <D.46531>;
  <D.46531>:
  ungetc (c, zStream);
  goto <D.46532>;
  <D.46530>:
  <D.46532>:
  goto <D.46504>;
  <D.39647>:
  _27 = ferror (zStream);
  retval.407 = _27 != 0;
  if (retval.407 != 0) goto <D.46534>; else goto <D.46535>;
  <D.46534>:
  D.46550 = pbzip2::ErrorContext::getInstance ();
  _28 = D.46550;
  pbzip2::ErrorContext::saveError (_28);
  handle_error (0, -1, "pbzip2: *ERROR: Problem with stream of file [%s]!  Skipping...\n", fileName);
  stdin.408_29 = stdin;
  if (zStream != stdin.408_29) goto <D.46536>; else goto <D.46537>;
  <D.46536>:
  verbose_fclose (zStream, fileName);
  goto <D.46538>;
  <D.46537>:
  <D.46538>:
  D.46496 = -1;
  goto <D.46553>;
  <D.46535>:
  D.46551 = verbose_fclose (zStream, fileName);
  ret = D.46551;
  if (ret == -1) goto <D.46539>; else goto <D.46540>;
  <D.46539>:
  stderr.409_30 = stderr;
  fprintf (stderr.409_30, "pbzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", fileName);
  D.46496 = -1;
  goto <D.46553>;
  <D.46540>:
  D.46496 = 0;
  goto <D.46553>;
  <D.46553>:
  obuf = {CLOBBER};
  unused = {CLOBBER};
  unusedTmp = {CLOBBER};
  bzerr = {CLOBBER};
  nUnused = {CLOBBER};
  goto <D.46541>;
  <D.46541>:
  return D.46496;
  <D.46552>:
  obuf = {CLOBBER};
  unused = {CLOBBER};
  unusedTmp = {CLOBBER};
  bzerr = {CLOBBER};
  nUnused = {CLOBBER};
  resx 1
}



;; Function int verbose_fclose(FILE*, const char*) (_Z14verbose_fcloseP8_IO_FILEPKc, funcdef_no=1952, decl_uid=39236, cgraph_uid=693, symbol_order=741)

int verbose_fclose(FILE*, const char*) (struct FILE * file, const char * fileName)
{
  int D.46560;
  int ret;
  int D.46558;
  bool retval.410;

  D.46560 = fclose (file);
  ret = D.46560;
  retval.410 = ret == -1;
  if (retval.410 != 0) goto <D.46555>; else goto <D.46556>;
  <D.46555>:
  _1 = __errno_location ();
  _2 = *_1;
  stderr.411_3 = stderr;
  pbzip2::ErrorContext::syncPrintErrnoMsg (stderr.411_3, _2);
  stderr.412_4 = stderr;
  fprintf (stderr.412_4, "pbzip2: *ERROR: Failed to close file [%s]!\n", fileName);
  goto <D.46557>;
  <D.46556>:
  <D.46557>:
  D.46558 = ret;
  goto <D.46559>;
  <D.46559>:
  return D.46558;
}



;; Function int getFileMetaData(const char*) (_Z15getFileMetaDataPKc, funcdef_no=2000, decl_uid=39036, cgraph_uid=740, symbol_order=788)

int getFileMetaData(const char*) (const char * fileName)
{
  int D.46561;

  D.46561 = stat (fileName, &fileMetaData);
  goto <D.46562>;
  <D.46562>:
  return D.46561;
}



;; Function int writeFileMetaData(const char*) (_Z17writeFileMetaDataPKc, funcdef_no=2001, decl_uid=39038, cgraph_uid=741, symbol_order=789)

int writeFileMetaData(const char*) (const char * fileName)
{
  struct ErrorContext * D.46577;
  struct ErrorContext * D.46576;
  struct ErrorContext * D.46575;
  struct utimbuf uTimBuf;
  int ret;
  bool retval.413;
  int D.46565;

  ret = 0;
  _1 = fileMetaData.st_atim.tv_sec;
  uTimBuf.actime = _1;
  _2 = fileMetaData.st_mtim.tv_sec;
  uTimBuf.modtime = _2;
  _3 = fileMetaData.st_mode;
  ret = chmod (fileName, _3);
  if (ret != 0) goto <D.46563>; else goto <D.46564>;
  <D.46563>:
  D.46575 = pbzip2::ErrorContext::getInstance ();
  _4 = D.46575;
  pbzip2::ErrorContext::saveError (_4);
  D.46565 = ret;
  goto <D.46579>;
  <D.46564>:
  ret = utime (fileName, &uTimBuf);
  if (ret != 0) goto <D.46566>; else goto <D.46567>;
  <D.46566>:
  D.46576 = pbzip2::ErrorContext::getInstance ();
  _5 = D.46576;
  pbzip2::ErrorContext::saveError (_5);
  D.46565 = ret;
  goto <D.46579>;
  <D.46567>:
  _6 = fileMetaData.st_gid;
  _7 = fileMetaData.st_uid;
  ret = chown (fileName, _7, _6);
  if (ret != 0) goto <D.46568>; else goto <D.46569>;
  <D.46568>:
  D.46577 = pbzip2::ErrorContext::getInstance ();
  _8 = D.46577;
  pbzip2::ErrorContext::saveError (_8);
  _9 = geteuid ();
  retval.413 = _9 == 0;
  if (retval.413 != 0) goto <D.46571>; else goto <D.46572>;
  <D.46571>:
  D.46565 = ret;
  goto <D.46579>;
  <D.46572>:
  goto <D.46573>;
  <D.46569>:
  <D.46573>:
  D.46565 = 0;
  goto <D.46579>;
  <D.46579>:
  uTimBuf = {CLOBBER};
  goto <D.46574>;
  <D.46574>:
  return D.46565;
  <D.46578>:
  uTimBuf = {CLOBBER};
  resx 1
}



;; Function int detectCPUs() (_Z10detectCPUsv, funcdef_no=2002, decl_uid=39049, cgraph_uid=742, symbol_order=790)

int detectCPUs() ()
{
  int ncpu;
  int D.46583;

  ncpu = 1;
  _1 = sysconf (84);
  ncpu = (int) _1;
  if (ncpu <= 0) goto <D.46580>; else goto <D.46581>;
  <D.46580>:
  ncpu = 1;
  goto <D.46582>;
  <D.46581>:
  <D.46582>:
  D.46583 = ncpu;
  goto <D.46584>;
  <D.46584>:
  return D.46583;
}



;; Function void banner() (_Z6bannerv, funcdef_no=2003, decl_uid=39697, cgraph_uid=743, symbol_order=791)

void banner() ()
{
  stderr.414_1 = stderr;
  fprintf (stderr.414_1, "Parallel BZIP2 v1.1.11 [Dec 12, 2014]\n");
  stderr.415_2 = stderr;
  fprintf (stderr.415_2, "By: Jeff Gilchrist [http://compression.ca]\n");
  stderr.416_3 = stderr;
  fprintf (stderr.416_3, "Major contributions: Yavor Nikolov [http://javornikolov.wordpress.com]\n");
  stderr.417_4 = stderr;
  fprintf (stderr.417_4, "Uses libbzip2 by Julian Seward\n");
  goto <D.46585>;
  <D.46585>:
  return;
}



;; Function void usage(char*, const char*) (_Z5usagePcPKc, funcdef_no=2004, decl_uid=39701, cgraph_uid=744, symbol_order=792)

void usage(char*, const char*) (char * progname, const char * reason)
{
  banner ();
  _1 = strncmp (reason, "HELP", 4);
  if (_1 == 0) goto <D.46586>; else goto <D.46587>;
  <D.46586>:
  stderr.418_2 = stderr;
  fprintf (stderr.418_2, "\n");
  goto <D.46588>;
  <D.46587>:
  stderr.419_3 = stderr;
  fprintf (stderr.419_3, "\nInvalid command line: %s.  Aborting...\n\n", reason);
  <D.46588>:
  stderr.420_4 = stderr;
  fprintf (stderr.420_4, "Usage: %s [-1 .. -9] [-b#cdfhklm#p#qrS#tVz] <filename> <filename2> <filenameN>\n", progname);
  stderr.421_5 = stderr;
  fprintf (stderr.421_5, " -1 .. -9        set BWT block size to 100k .. 900k (default 900k)\n");
  stderr.422_6 = stderr;
  fprintf (stderr.422_6, " -b#             Block size in 100k steps (default 9 = 900k)\n");
  stderr.423_7 = stderr;
  fprintf (stderr.423_7, " -c,--stdout     Output to standard out (stdout)\n");
  stderr.424_8 = stderr;
  fprintf (stderr.424_8, " -d,--decompress Decompress file\n");
  stderr.425_9 = stderr;
  fprintf (stderr.425_9, " -f,--force      Overwrite existing output file\n");
  stderr.426_10 = stderr;
  fprintf (stderr.426_10, " -h,--help       Print this help message\n");
  stderr.427_11 = stderr;
  fprintf (stderr.427_11, " -k,--keep       Keep input file, don\'t delete\n");
  stderr.428_12 = stderr;
  fprintf (stderr.428_12, " -l,--loadavg    Load average determines max number processors to use\n");
  stderr.429_13 = stderr;
  fprintf (stderr.429_13, " -m#             Maximum memory usage in 1MB steps (default 100 = 100MB)\n");
  stderr.430_14 = stderr;
  fprintf (stderr.430_14, " -p#             Number of processors to use (default");
  _15 = detectCPUs ();
  stderr.431_16 = stderr;
  fprintf (stderr.431_16, ": autodetect [%d])\n", _15);
  stderr.432_17 = stderr;
  fprintf (stderr.432_17, " -q,--quiet      Quiet mode (default)\n");
  stderr.433_18 = stderr;
  fprintf (stderr.433_18, " -r,--read       Read entire input file into RAM and split between processors\n");
  stderr.434_19 = stderr;
  fprintf (stderr.434_19, " -S#             Child thread stack size in 1KB steps (default stack size if unspecified)\n");
  stderr.435_20 = stderr;
  fprintf (stderr.435_20, " -t,--test       Test compressed file integrity\n");
  stderr.436_21 = stderr;
  fprintf (stderr.436_21, " -v,--verbose    Verbose mode\n");
  stderr.437_22 = stderr;
  fprintf (stderr.437_22, " -V,--version    Display version info for pbzip2 then exit\n");
  stderr.438_23 = stderr;
  fprintf (stderr.438_23, " -z,--compress   Compress file (default)\n");
  stderr.439_24 = stderr;
  fprintf (stderr.439_24, " --ignore-trailing-garbage=# Ignore trailing garbage flag (1 - ignored; 0 - forbidden)\n");
  stderr.440_25 = stderr;
  fprintf (stderr.440_25, "\n");
  stderr.441_26 = stderr;
  fprintf (stderr.441_26, "If no file names are given, pbzip2 compresses or decompresses from standard input to standard output.\n");
  stderr.442_27 = stderr;
  fprintf (stderr.442_27, "\n");
  stderr.443_28 = stderr;
  fprintf (stderr.443_28, "Example: pbzip2 -b15vk myfile.tar\n");
  stderr.444_29 = stderr;
  fprintf (stderr.444_29, "Example: pbzip2 -p4 -r -5 myfile.tar second*.txt\n");
  stderr.445_30 = stderr;
  fprintf (stderr.445_30, "Example: tar cf myfile.tar.bz2 --use-compress-prog=pbzip2 dir_to_compress/\n");
  stderr.446_31 = stderr;
  fprintf (stderr.446_31, "Example: pbzip2 -d -m500 myfile.tar.bz2\n");
  stderr.447_32 = stderr;
  fprintf (stderr.447_32, "Example: pbzip2 -dc myfile.tar.bz2 | tar x\n");
  stderr.448_33 = stderr;
  fprintf (stderr.448_33, "Example: pbzip2 -c < myfile.txt > myfile.txt.bz2 \n");
  stderr.449_34 = stderr;
  fprintf (stderr.449_34, "\n");
  exit (-1);
}



;; Function int main(int, char**) (main, funcdef_no=2005, decl_uid=39705, cgraph_uid=748, symbol_order=796)

int main(int, char**) (int argc, char * * argv)
{
  int D.47187;
  int D.47186;
  int D.47185;
  struct ErrorContext * D.47184;
  int D.47183;
  int D.47182;
  int D.47180;
  int D.47179;
  int D.47178;
  int D.47177;
  int D.47176;
  int D.47175;
  int D.47174;
  struct ErrorContext * D.47173;
  int D.47172;
  int D.47171;
  struct ErrorContext * D.47170;
  int D.47169;
  int D.47168;
  int D.47167;
  int D.47166;
  struct ErrorContext * D.47165;
  int D.47163;
  int D.47162;
  int D.47161;
  long int D.47160;
  int D.47159;
  bool D.47158;
  int D.47154;
  unsigned char * D.47134;
  unsigned char * D.47133;
  unsigned char * D.47132;
  struct ErrorContext * D.47131;
  long int D.47130;
  int D.47129;
  int D.47126;
  struct ErrorContext * D.47125;
  struct queue * D.47124;
  int D.47123;
  struct ErrorContext * D.47122;
  bool removeFlag;
  struct stat statbuf;
  bool removeFlag;
  struct stat statbuf;
  int close_ret;
  struct stat statbuf;
  struct string bz2Tail;
  struct string tbz2Tail;
  struct string bz2Tail;
  size_t size;
  int errLevelCurrentFile;
  int mutexesInitRet;
  bool switchedMtToSt;
  size_t k;
  size_t j;
  size_t i;
  int fileLoop;
  int ret;
  int force;
  int keep;
  int noThreads;
  int errLevel;
  int testFile;
  int compress;
  int decompress;
  int maxMemorySwitch;
  int maxMemory;
  int blockSize;
  int numBlocks;
  int hOutfile;
  int hInfile;
  int zeroByteFile;
  int readEntireFile;
  int cmdLineTempCount;
  double timeStop;
  double timeStart;
  double timeCalc;
  int numCPUidle;
  int numCPUtotal;
  int useLoadAverage;
  double loadAvgArray[3];
  double loadAverage;
  struct timezone tz;
  struct timeval tvStopTime;
  struct timeval tvStartTime;
  char stdinFile[2];
  unsigned char tmpBuff[50];
  char cmdLineTemp[2048];
  struct string outFilename;
  char bz2Header[11];
  char * progNamePos;
  char * progName;
  char * InFilename;
  char * * FileList;
  pthread_t output;
  struct queue * fifo;
  bool retval.667;
  bool iftmp.658;
  bool retval.657;
  bool retval.656;
  bool iftmp.654;
  bool retval.653;
  bool retval.651;
  bool retval.634;
  bool retval.612;
  bool iftmp.610;
  bool retval.605;
  const char * iftmp.587;
  bool iftmp.576;
  bool retval.575;
  bool retval.572;
  struct allocator D.39861;
  struct string D.39862;
  bool retval.560;
  struct allocator D.39859;
  bool retval.559;
  bool retval.558;
  struct allocator D.39853;
  struct allocator D.39850;
  bool retval.553;
  bool retval.552;
  bool retval.549;
  struct allocator D.39832;
  struct string D.39833;
  const char * iftmp.525;
  bool retval.495;
  bool retval.485;
  bool retval.478;
  sizetype D.46615;
  size_t iftmp.451;
  int D.46592;
  bool retval.450;

  FileList = 0B;
  InFilename = 0B;
  progName = 0B;
  progNamePos = 0B;
  bz2Header = "BZh91AY&SY";
  std::__cxx11::basic_string<char>::basic_string (&outFilename);
  stdinFile = "-";
  loadAverage = 0.0;
  useLoadAverage = 0;
  numCPUtotal = 0;
  numCPUidle = 0;
  timeCalc = 0.0;
  timeStart = 0.0;
  timeStop = 0.0;
  cmdLineTempCount = 0;
  readEntireFile = 0;
  zeroByteFile = 0;
  hInfile = -1;
  hOutfile = -1;
  numBlocks = 0;
  blockSize = 900000;
  maxMemory = 100000000;
  maxMemorySwitch = 0;
  decompress = 0;
  compress = 0;
  testFile = 0;
  errLevel = 0;
  noThreads = 0;
  keep = 0;
  force = 0;
  ret = 0;
  switchedMtToSt = 0;
  D.47122 = pbzip2::ErrorContext::getInstance ();
  _1 = D.47122;
  retval.450 = _1 == 0B;
  if (retval.450 != 0) goto <D.46590>; else goto <D.46591>;
  <D.46590>:
  D.46592 = 1;
  goto <D.47189>;
  <D.46591>:
  gettimeofday (&tvStartTime, &tz);
  if (argc <= 1) goto <D.46593>; else goto <D.46594>;
  <D.46593>:
  OutputStdOut = 1;
  keep = 1;
  goto <D.46595>;
  <D.46594>:
  <D.46595>:
  progName = *argv;
  progNamePos = *argv;
  <D.46596>:
  _2 = *progNamePos;
  if (_2 == 0) goto <D.39871>; else goto <D.46597>;
  <D.46597>:
  _3 = *progNamePos;
  if (_3 == 47) goto <D.46598>; else goto <D.46599>;
  <D.46598>:
  progName = progNamePos + 1;
  goto <D.46600>;
  <D.46599>:
  <D.46600>:
  progNamePos = progNamePos + 1;
  goto <D.46596>;
  <D.39871>:
  _4 = strstr (progName, "unzip");
  if (_4 != 0B) goto <D.46601>; else goto <D.46603>;
  <D.46603>:
  _5 = strstr (progName, "UNZIP");
  if (_5 != 0B) goto <D.46601>; else goto <D.46602>;
  <D.46601>:
  decompress = 1;
  <D.46602>:
  _6 = strstr (progName, "zcat");
  if (_6 != 0B) goto <D.46604>; else goto <D.46606>;
  <D.46606>:
  _7 = strstr (progName, "ZCAT");
  if (_7 != 0B) goto <D.46604>; else goto <D.46605>;
  <D.46604>:
  keep = 1;
  OutputStdOut = keep;
  decompress = OutputStdOut;
  <D.46605>:
  _8 = strcmp (progName, "bzip2");
  if (_8 == 0) goto <D.46607>; else goto <D.46609>;
  <D.46609>:
  _9 = strcmp (progName, "BZIP2");
  if (_9 == 0) goto <D.46607>; else goto <D.46610>;
  <D.46610>:
  _10 = strcmp (progName, "bunzip2");
  if (_10 == 0) goto <D.46607>; else goto <D.46611>;
  <D.46611>:
  _11 = strcmp (progName, "BUNZIP2");
  if (_11 == 0) goto <D.46607>; else goto <D.46612>;
  <D.46612>:
  _12 = strcmp (progName, "bzcat");
  if (_12 == 0) goto <D.46607>; else goto <D.46613>;
  <D.46613>:
  _13 = strcmp (progName, "BZCAT");
  if (_13 == 0) goto <D.46607>; else goto <D.46608>;
  <D.46607>:
  IgnoreTrailingGarbageFlag = 1;
  <D.46608>:
  FileListCount = 0;
  D.46615 = (sizetype) argc;
  if (D.46615 <= 1152921504606846975) goto <D.46616>; else goto <D.46617>;
  <D.46616>:
  iftmp.451 = D.46615 * 8;
  goto <D.46618>;
  <D.46617>:
  __cxa_throw_bad_array_new_length ();
  <D.46618>:
  _14 = operator new [] (iftmp.451, &nothrow);
  FileList = _14;
  FileList.452_15 = FileList;
  if (FileList.452_15 == 0B) goto <D.46619>; else goto <D.46620>;
  <D.46619>:
  stderr.453_16 = stderr;
  fprintf (stderr.453_16, "pbzip2: *ERROR: Not enough memory!  Aborting...\n");
  D.46592 = 1;
  goto <D.47189>;
  <D.46620>:
  maxMemory = 100000000;
  NumBufferedBlocksMax = 0;
  _17 = detectCPUs ();
  numCPU = _17;
  numCPUtotal = numCPU;
  i = 1;
  <D.46621>:
  _18 = (int) i;
  if (argc <= _18) goto <D.39873>; else goto <D.46622>;
  <D.46622>:
  _19 = i * 8;
  _20 = argv + _19;
  _21 = *_20;
  _22 = *_21;
  if (_22 == 45) goto <D.46623>; else goto <D.46624>;
  <D.46623>:
  _23 = i * 8;
  _24 = argv + _23;
  _25 = *_24;
  _26 = _25 + 1;
  _27 = *_26;
  if (_27 == 0) goto <D.46625>; else goto <D.46626>;
  <D.46625>:
  _28 = i * 8;
  _29 = argv + _28;
  FileList.454_30 = FileList;
  FileListCount.455_31 = FileListCount;
  _32 = (long unsigned int) FileListCount.455_31;
  _33 = _32 * 8;
  _34 = FileList.454_30 + _33;
  _35 = *_29;
  *_34 = _35;
  FileListCount.456_36 = FileListCount;
  _37 = FileListCount.456_36 + 1;
  FileListCount = _37;
  // predicted unlikely by continue predictor.
  goto <D.39874>;
  <D.46626>:
  _38 = i * 8;
  _39 = argv + _38;
  _40 = *_39;
  _41 = _40 + 1;
  _42 = *_41;
  if (_42 == 45) goto <D.46627>; else goto <D.46628>;
  <D.46627>:
  _43 = i * 8;
  _44 = argv + _43;
  _45 = *_44;
  _46 = strcmp (_45, "--best");
  if (_46 == 0) goto <D.46629>; else goto <D.46630>;
  <D.46629>:
  BWTblockSize = 9;
  goto <D.46631>;
  <D.46630>:
  _47 = i * 8;
  _48 = argv + _47;
  _49 = *_48;
  _50 = strcmp (_49, "--decompress");
  if (_50 == 0) goto <D.46632>; else goto <D.46633>;
  <D.46632>:
  decompress = 1;
  goto <D.46634>;
  <D.46633>:
  _51 = i * 8;
  _52 = argv + _51;
  _53 = *_52;
  _54 = strcmp (_53, "--compress");
  if (_54 == 0) goto <D.46635>; else goto <D.46636>;
  <D.46635>:
  compress = 1;
  goto <D.46637>;
  <D.46636>:
  _55 = i * 8;
  _56 = argv + _55;
  _57 = *_56;
  _58 = strcmp (_57, "--fast");
  if (_58 == 0) goto <D.46638>; else goto <D.46639>;
  <D.46638>:
  BWTblockSize = 1;
  goto <D.46640>;
  <D.46639>:
  _59 = i * 8;
  _60 = argv + _59;
  _61 = *_60;
  _62 = strcmp (_61, "--force");
  if (_62 == 0) goto <D.46641>; else goto <D.46642>;
  <D.46641>:
  force = 1;
  ForceOverwrite = 1;
  goto <D.46643>;
  <D.46642>:
  _63 = i * 8;
  _64 = argv + _63;
  _65 = *_64;
  _66 = strcmp (_65, "--help");
  if (_66 == 0) goto <D.46644>; else goto <D.46645>;
  <D.46644>:
  _67 = *argv;
  usage (_67, "HELP");
  goto <D.46646>;
  <D.46645>:
  _68 = i * 8;
  _69 = argv + _68;
  _70 = *_69;
  _71 = strcmp (_70, "--keep");
  if (_71 == 0) goto <D.46647>; else goto <D.46648>;
  <D.46647>:
  keep = 1;
  goto <D.46649>;
  <D.46648>:
  _72 = i * 8;
  _73 = argv + _72;
  _74 = *_73;
  _75 = strcmp (_74, "--license");
  if (_75 == 0) goto <D.46650>; else goto <D.46651>;
  <D.46650>:
  _76 = *argv;
  usage (_76, "HELP");
  goto <D.46652>;
  <D.46651>:
  _77 = i * 8;
  _78 = argv + _77;
  _79 = *_78;
  _80 = strcmp (_79, "--loadavg");
  if (_80 == 0) goto <D.46653>; else goto <D.46654>;
  <D.46653>:
  useLoadAverage = 1;
  goto <D.46655>;
  <D.46654>:
  _81 = i * 8;
  _82 = argv + _81;
  _83 = *_82;
  _84 = strcmp (_83, "--quiet");
  if (_84 == 0) goto <D.46656>; else goto <D.46657>;
  <D.46656>:
  QuietMode = 1;
  goto <D.46658>;
  <D.46657>:
  _85 = i * 8;
  _86 = argv + _85;
  _87 = *_86;
  _88 = strcmp (_87, "--read");
  if (_88 == 0) goto <D.46659>; else goto <D.46660>;
  <D.46659>:
  readEntireFile = 1;
  goto <D.46661>;
  <D.46660>:
  _89 = i * 8;
  _90 = argv + _89;
  _91 = *_90;
  _92 = strcmp (_91, "--stdout");
  if (_92 == 0) goto <D.46662>; else goto <D.46663>;
  <D.46662>:
  OutputStdOut = 1;
  keep = 1;
  goto <D.46664>;
  <D.46663>:
  _93 = i * 8;
  _94 = argv + _93;
  _95 = *_94;
  _96 = strcmp (_95, "--test");
  if (_96 == 0) goto <D.46665>; else goto <D.46666>;
  <D.46665>:
  testFile = 1;
  goto <D.46667>;
  <D.46666>:
  _97 = i * 8;
  _98 = argv + _97;
  _99 = *_98;
  _100 = strcmp (_99, "--verbose");
  if (_100 == 0) goto <D.46668>; else goto <D.46669>;
  <D.46668>:
  QuietMode = 0;
  goto <D.46670>;
  <D.46669>:
  _101 = i * 8;
  _102 = argv + _101;
  _103 = *_102;
  _104 = strcmp (_103, "--version");
  if (_104 == 0) goto <D.46671>; else goto <D.46672>;
  <D.46671>:
  banner ();
  exit (0);
  <D.46672>:
  _105 = i * 8;
  _106 = argv + _105;
  _107 = *_106;
  _108 = strcmp (_107, "--ignore-trailing-garbage");
  if (_108 == 0) goto <D.46673>; else goto <D.46674>;
  <D.46673>:
  IgnoreTrailingGarbageFlag = 1;
  goto <D.46675>;
  <D.46674>:
  _109 = i * 8;
  _110 = argv + _109;
  _111 = *_110;
  _112 = strcmp (_111, "--ignore-trailing-garbage=1");
  if (_112 == 0) goto <D.46676>; else goto <D.46677>;
  <D.46676>:
  IgnoreTrailingGarbageFlag = 1;
  goto <D.46678>;
  <D.46677>:
  _113 = i * 8;
  _114 = argv + _113;
  _115 = *_114;
  _116 = strcmp (_115, "--ignore-trailing-garbage=0");
  if (_116 == 0) goto <D.46679>; else goto <D.46680>;
  <D.46679>:
  IgnoreTrailingGarbageFlag = 0;
  goto <D.46681>;
  <D.46680>:
  <D.46681>:
  <D.46678>:
  <D.46675>:
  <D.46670>:
  <D.46667>:
  <D.46664>:
  <D.46661>:
  <D.46658>:
  <D.46655>:
  <D.46652>:
  <D.46649>:
  <D.46646>:
  <D.46643>:
  <D.46640>:
  <D.46637>:
  <D.46634>:
  <D.46631>:
  // predicted unlikely by continue predictor.
  goto <D.39874>;
  <D.46628>:
  j = 1;
  <D.46682>:
  _117 = i * 8;
  _118 = argv + _117;
  _119 = *_118;
  _120 = _119 + j;
  _121 = *_120;
  if (_121 == 0) goto <D.39875>; else goto <D.46683>;
  <D.46683>:
  _122 = i * 8;
  _123 = argv + _122;
  _124 = *_123;
  _125 = _124 + j;
  _126 = *_125;
  _127 = (int) _126;
  switch (_127) <default: <D.46726>, case 49: <D.39820>, case 50: <D.39821>, case 51: <D.39822>, case 52: <D.39823>, case 53: <D.39824>, case 54: <D.39825>, case 55: <D.39826>, case 56: <D.39827>, case 57: <D.39828>, case 76: <D.39813>, case 83: <D.39806>, case 86: <D.39818>, case 98: <D.39804>, case 99: <D.39808>, case 100: <D.39807>, case 102: <D.39809>, case 104: <D.39810>, case 107: <D.39811>, case 108: <D.39812>, case 109: <D.39805>, case 112: <D.39803>, case 113: <D.39814>, case 114: <D.39815>, case 116: <D.39816>, case 118: <D.39817>, case 122: <D.39819>>
  <D.39803>:
  k = j + 1;
  cmdLineTempCount = 0;
  strcpy (&cmdLineTemp, "2");
  <D.46684>:
  _128 = i * 8;
  _129 = argv + _128;
  _130 = *_129;
  _131 = _130 + k;
  _132 = *_131;
  if (_132 == 0) goto <D.39878>; else goto <D.46685>;
  <D.46685>:
  if (k > 2047) goto <D.39878>; else goto <D.46686>;
  <D.46686>:
  _133 = i * 8;
  _134 = argv + _133;
  _135 = *_134;
  _136 = _135 + k;
  _137 = *_136;
  _138 = (unsigned char) _137;
  _139 = _138 + 208;
  if (_139 > 9) goto <D.39878>; else goto <D.46687>;
  <D.46687>:
  k = k + 1;
  cmdLineTempCount = cmdLineTempCount + 1;
  goto <D.46684>;
  <D.39878>:
  if (cmdLineTempCount == 0) goto <D.46688>; else goto <D.46689>;
  <D.46688>:
  _140 = *argv;
  usage (_140, "Cannot parse -p argument");
  goto <D.46690>;
  <D.46689>:
  <D.46690>:
  _141 = (long unsigned int) cmdLineTempCount;
  _142 = i * 8;
  _143 = argv + _142;
  _144 = *_143;
  _145 = j + 1;
  _146 = _144 + _145;
  strncpy (&cmdLineTemp, _146, _141);
  cmdLineTemp[cmdLineTempCount] = 0;
  _147 = atoi (&cmdLineTemp);
  numCPU = _147;
  numCPU.457_148 = numCPU;
  if (numCPU.457_148 > 4096) goto <D.46691>; else goto <D.46692>;
  <D.46691>:
  stderr.458_149 = stderr;
  fprintf (stderr.458_149, "pbzip2: *ERROR: Maximal number of supported processors is 4096!  Aborting...\n");
  D.46592 = 1;
  goto <D.47189>;
  <D.46692>:
  numCPU.459_150 = numCPU;
  if (numCPU.459_150 <= 0) goto <D.46693>; else goto <D.46694>;
  <D.46693>:
  stderr.460_151 = stderr;
  fprintf (stderr.460_151, "pbzip2: *ERROR: Minimum number of supported processors is 1!  Aborting...\n");
  D.46592 = 1;
  goto <D.47189>;
  <D.46694>:
  _152 = (long unsigned int) cmdLineTempCount;
  j = j + _152;
  goto <D.39877>;
  <D.39804>:
  k = j + 1;
  cmdLineTempCount = 0;
  strcpy (&cmdLineTemp, "9");
  blockSize = 900000;
  <D.46695>:
  _153 = i * 8;
  _154 = argv + _153;
  _155 = *_154;
  _156 = _155 + k;
  _157 = *_156;
  if (_157 == 0) goto <D.39880>; else goto <D.46696>;
  <D.46696>:
  if (k > 2047) goto <D.39880>; else goto <D.46697>;
  <D.46697>:
  _158 = i * 8;
  _159 = argv + _158;
  _160 = *_159;
  _161 = _160 + k;
  _162 = *_161;
  _163 = (unsigned char) _162;
  _164 = _163 + 208;
  if (_164 > 9) goto <D.39880>; else goto <D.46698>;
  <D.46698>:
  k = k + 1;
  cmdLineTempCount = cmdLineTempCount + 1;
  goto <D.46695>;
  <D.39880>:
  if (cmdLineTempCount == 0) goto <D.46699>; else goto <D.46700>;
  <D.46699>:
  _165 = *argv;
  usage (_165, "Cannot parse file block size");
  goto <D.46701>;
  <D.46700>:
  <D.46701>:
  _166 = (long unsigned int) cmdLineTempCount;
  _167 = i * 8;
  _168 = argv + _167;
  _169 = *_168;
  _170 = j + 1;
  _171 = _169 + _170;
  strncpy (&cmdLineTemp, _171, _166);
  cmdLineTemp[cmdLineTempCount] = 0;
  _172 = atoi (&cmdLineTemp);
  blockSize = _172 * 100000;
  blockSize.461_173 = (unsigned int) blockSize;
  _174 = blockSize.461_173 + 4294867296;
  if (_174 > 999900000) goto <D.46702>; else goto <D.46703>;
  <D.46702>:
  stderr.462_175 = stderr;
  fprintf (stderr.462_175, "pbzip2: *ERROR: File block size Min: 100k and Max: 1000000k!  Aborting...\n");
  D.46592 = 1;
  goto <D.47189>;
  <D.46703>:
  _176 = (long unsigned int) cmdLineTempCount;
  j = j + _176;
  goto <D.39877>;
  <D.39805>:
  k = j + 1;
  cmdLineTempCount = 0;
  strcpy (&cmdLineTemp, "1");
  maxMemory = 1000000;
  <D.46704>:
  _177 = i * 8;
  _178 = argv + _177;
  _179 = *_178;
  _180 = _179 + k;
  _181 = *_180;
  if (_181 == 0) goto <D.39882>; else goto <D.46705>;
  <D.46705>:
  if (k > 2047) goto <D.39882>; else goto <D.46706>;
  <D.46706>:
  _182 = i * 8;
  _183 = argv + _182;
  _184 = *_183;
  _185 = _184 + k;
  _186 = *_185;
  _187 = (unsigned char) _186;
  _188 = _187 + 208;
  if (_188 > 9) goto <D.39882>; else goto <D.46707>;
  <D.46707>:
  k = k + 1;
  cmdLineTempCount = cmdLineTempCount + 1;
  goto <D.46704>;
  <D.39882>:
  if (cmdLineTempCount == 0) goto <D.46708>; else goto <D.46709>;
  <D.46708>:
  _189 = *argv;
  usage (_189, "Cannot parse -m argument");
  goto <D.46710>;
  <D.46709>:
  <D.46710>:
  _190 = (long unsigned int) cmdLineTempCount;
  _191 = i * 8;
  _192 = argv + _191;
  _193 = *_192;
  _194 = j + 1;
  _195 = _193 + _194;
  strncpy (&cmdLineTemp, _195, _190);
  cmdLineTemp[cmdLineTempCount] = 0;
  _196 = atoi (&cmdLineTemp);
  maxMemory = _196 * 1000000;
  maxMemory.463_197 = (unsigned int) maxMemory;
  _198 = maxMemory.463_197 + 4293967296;
  if (_198 > 1999000000) goto <D.46711>; else goto <D.46712>;
  <D.46711>:
  stderr.464_199 = stderr;
  fprintf (stderr.464_199, "pbzip2: *ERROR: Memory usage size Min: 1MB and Max: 2000MB!  Aborting...\n");
  D.46592 = 1;
  goto <D.47189>;
  <D.46712>:
  maxMemorySwitch = 1;
  _200 = (long unsigned int) cmdLineTempCount;
  j = j + _200;
  goto <D.39877>;
  <D.39806>:
  k = j + 1;
  cmdLineTempCount = 0;
  strcpy (&cmdLineTemp, "0");
  ChildThreadStackSize = -1;
  <D.46713>:
  _201 = i * 8;
  _202 = argv + _201;
  _203 = *_202;
  _204 = _203 + k;
  _205 = *_204;
  if (_205 == 0) goto <D.39884>; else goto <D.46714>;
  <D.46714>:
  if (k > 2047) goto <D.39884>; else goto <D.46715>;
  <D.46715>:
  _206 = i * 8;
  _207 = argv + _206;
  _208 = *_207;
  _209 = _208 + k;
  _210 = *_209;
  _211 = (unsigned char) _210;
  _212 = _211 + 208;
  if (_212 > 9) goto <D.39884>; else goto <D.46716>;
  <D.46716>:
  k = k + 1;
  cmdLineTempCount = cmdLineTempCount + 1;
  goto <D.46713>;
  <D.39884>:
  if (cmdLineTempCount == 0) goto <D.46717>; else goto <D.46718>;
  <D.46717>:
  _213 = *argv;
  usage (_213, "Cannot parse -S argument");
  goto <D.46719>;
  <D.46718>:
  <D.46719>:
  _214 = (long unsigned int) cmdLineTempCount;
  _215 = i * 8;
  _216 = argv + _215;
  _217 = *_216;
  _218 = j + 1;
  _219 = _217 + _218;
  strncpy (&cmdLineTemp, _219, _214);
  cmdLineTemp[cmdLineTempCount] = 0;
  _220 = atoi (&cmdLineTemp);
  _221 = _220 * 1024;
  ChildThreadStackSize = _221;
  ChildThreadStackSize.465_222 = ChildThreadStackSize;
  if (ChildThreadStackSize.465_222 < 0) goto <D.46720>; else goto <D.46721>;
  <D.46720>:
  ChildThreadStackSize.466_223 = ChildThreadStackSize;
  stderr.467_224 = stderr;
  fprintf (stderr.467_224, "pbzip2: *ERROR: Parsing -S: invalid stack size specified [%d]!  Ignoring...\n", ChildThreadStackSize.466_223);
  goto <D.46722>;
  <D.46721>:
  ChildThreadStackSize.468_225 = ChildThreadStackSize;
  if (ChildThreadStackSize.468_225 <= 16383) goto <D.46723>; else goto <D.46724>;
  <D.46723>:
  ChildThreadStackSize.469_226 = ChildThreadStackSize;
  stderr.470_227 = stderr;
  fprintf (stderr.470_227, "pbzip2: *WARNING: Stack size %d bytes less than minumum - adjusting to %d bytes.\n", ChildThreadStackSize.469_226, 16384);
  ChildThreadStackSize = 16384;
  goto <D.46725>;
  <D.46724>:
  <D.46725>:
  <D.46722>:
  _228 = (long unsigned int) cmdLineTempCount;
  j = j + _228;
  goto <D.39877>;
  <D.39807>:
  decompress = 1;
  goto <D.39877>;
  <D.39808>:
  OutputStdOut = 1;
  keep = 1;
  goto <D.39877>;
  <D.39809>:
  force = 1;
  ForceOverwrite = 1;
  goto <D.39877>;
  <D.39810>:
  _229 = *argv;
  usage (_229, "HELP");
  goto <D.39877>;
  <D.39811>:
  keep = 1;
  goto <D.39877>;
  <D.39812>:
  useLoadAverage = 1;
  goto <D.39877>;
  <D.39813>:
  banner ();
  exit (0);
  <D.39814>:
  QuietMode = 1;
  goto <D.39877>;
  <D.39815>:
  readEntireFile = 1;
  goto <D.39877>;
  <D.39816>:
  testFile = 1;
  goto <D.39877>;
  <D.39817>:
  QuietMode = 0;
  goto <D.39877>;
  <D.39818>:
  banner ();
  exit (0);
  <D.39819>:
  compress = 1;
  goto <D.39877>;
  <D.39820>:
  BWTblockSize = 1;
  goto <D.39877>;
  <D.39821>:
  BWTblockSize = 2;
  goto <D.39877>;
  <D.39822>:
  BWTblockSize = 3;
  goto <D.39877>;
  <D.39823>:
  BWTblockSize = 4;
  goto <D.39877>;
  <D.39824>:
  BWTblockSize = 5;
  goto <D.39877>;
  <D.39825>:
  BWTblockSize = 6;
  goto <D.39877>;
  <D.39826>:
  BWTblockSize = 7;
  goto <D.39877>;
  <D.39827>:
  BWTblockSize = 8;
  goto <D.39877>;
  <D.39828>:
  BWTblockSize = 9;
  goto <D.39877>;
  <D.46726>:
  <D.39877>:
  j = j + 1;
  goto <D.46682>;
  <D.39875>:
  goto <D.46727>;
  <D.46624>:
  _230 = i * 8;
  _231 = argv + _230;
  FileList.471_232 = FileList;
  FileListCount.472_233 = FileListCount;
  _234 = (long unsigned int) FileListCount.472_233;
  _235 = _234 * 8;
  _236 = FileList.471_232 + _235;
  _237 = *_231;
  *_236 = _237;
  FileListCount.473_238 = FileListCount;
  _239 = FileListCount.473_238 + 1;
  FileListCount = _239;
  <D.46727>:
  <D.39874>:
  i = i + 1;
  goto <D.46621>;
  <D.39873>:
  BWTblockSize.474_240 = BWTblockSize;
  _241 = (unsigned char) BWTblockSize.474_240;
  _242 = _241 + 48;
  Bz2HeaderZero[3] = _242;
  _243 = Bz2HeaderZero[3];
  _244 = (char) _243;
  bz2Header[3] = _244;
  _245 = compress == 1;
  _246 = decompress == 1;
  _247 = _245 & _246;
  if (_247 != 0) goto <D.46728>; else goto <D.46729>;
  <D.46728>:
  stderr.475_248 = stderr;
  fprintf (stderr.475_248, "pbzip2: *ERROR: Can\'t compress and uncompress data at same time.  Aborting!\n");
  _249 = *argv;
  stderr.476_250 = stderr;
  fprintf (stderr.476_250, "pbzip2: For help type: %s -h\n", _249);
  D.46592 = 1;
  goto <D.47189>;
  <D.46729>:
  FileListCount.477_251 = FileListCount;
  if (FileListCount.477_251 == 0) goto <D.46730>; else goto <D.46731>;
  <D.46730>:
  if (testFile == 1) goto <D.46732>; else goto <D.46733>;
  <D.46732>:
  stdin.479_252 = stdin;
  _253 = fileno (stdin.479_252);
  _254 = isatty (_253);
  retval.478 = _254 != 0;
  if (retval.478 != 0) goto <D.46735>; else goto <D.46736>;
  <D.46735>:
  stderr.480_255 = stderr;
  fprintf (stderr.480_255, "pbzip2: *ERROR: Won\'t read compressed data from terminal.  Aborting!\n");
  _256 = *argv;
  stderr.481_257 = stderr;
  fprintf (stderr.481_257, "pbzip2: For help type: %s -h\n", _256);
  D.46592 = 1;
  goto <D.47189>;
  <D.46736>:
  FileList.482_258 = FileList;
  FileListCount.483_259 = FileListCount;
  _260 = (long unsigned int) FileListCount.483_259;
  _261 = _260 * 8;
  _262 = FileList.482_258 + _261;
  *_262 = &stdinFile;
  FileListCount.484_263 = FileListCount;
  _264 = FileListCount.484_263 + 1;
  FileListCount = _264;
  goto <D.46737>;
  <D.46733>:
  if (decompress == 1) goto <D.46738>; else goto <D.46739>;
  <D.46738>:
  stdin.486_265 = stdin;
  _266 = fileno (stdin.486_265);
  _267 = isatty (_266);
  retval.485 = _267 != 0;
  if (retval.485 != 0) goto <D.46741>; else goto <D.46742>;
  <D.46741>:
  stderr.487_268 = stderr;
  fprintf (stderr.487_268, "pbzip2: *ERROR: Won\'t read compressed data from terminal.  Aborting!\n");
  _269 = *argv;
  stderr.488_270 = stderr;
  fprintf (stderr.488_270, "pbzip2: For help type: %s -h\n", _269);
  D.46592 = 1;
  goto <D.47189>;
  <D.46742>:
  OutputStdOut = 1;
  keep = 1;
  FileList.489_271 = FileList;
  FileListCount.490_272 = FileListCount;
  _273 = (long unsigned int) FileListCount.490_272;
  _274 = _273 * 8;
  _275 = FileList.489_271 + _274;
  *_275 = &stdinFile;
  FileListCount.491_276 = FileListCount;
  _277 = FileListCount.491_276 + 1;
  FileListCount = _277;
  goto <D.46743>;
  <D.46739>:
  OutputStdOut.492_278 = OutputStdOut;
  if (OutputStdOut.492_278 == 0) goto <D.46744>; else goto <D.46745>;
  <D.46744>:
  QuietMode.493_279 = QuietMode;
  if (QuietMode.493_279 != 1) goto <D.46746>; else goto <D.46747>;
  <D.46746>:
  stderr.494_280 = stderr;
  fprintf (stderr.494_280, "pbzip2: Assuming input data coming from stdin...\n\n");
  goto <D.46748>;
  <D.46747>:
  <D.46748>:
  OutputStdOut = 1;
  keep = 1;
  goto <D.46749>;
  <D.46745>:
  <D.46749>:
  stdout.496_281 = stdout;
  _282 = fileno (stdout.496_281);
  _283 = isatty (_282);
  retval.495 = _283 != 0;
  if (retval.495 != 0) goto <D.46751>; else goto <D.46752>;
  <D.46751>:
  stderr.497_284 = stderr;
  fprintf (stderr.497_284, "pbzip2: *ERROR: Won\'t write compressed data to terminal.  Aborting!\n");
  _285 = *argv;
  stderr.498_286 = stderr;
  fprintf (stderr.498_286, "pbzip2: For help type: %s -h\n", _285);
  D.46592 = 1;
  goto <D.47189>;
  <D.46752>:
  FileList.499_287 = FileList;
  FileListCount.500_288 = FileListCount;
  _289 = (long unsigned int) FileListCount.500_288;
  _290 = _289 * 8;
  _291 = FileList.499_287 + _290;
  *_291 = &stdinFile;
  FileListCount.501_292 = FileListCount;
  _293 = FileListCount.501_292 + 1;
  FileListCount = _293;
  <D.46743>:
  <D.46737>:
  goto <D.46753>;
  <D.46731>:
  <D.46753>:
  QuietMode.502_294 = QuietMode;
  if (QuietMode.502_294 != 1) goto <D.46754>; else goto <D.46755>;
  <D.46754>:
  banner ();
  if (0 != 0) goto <D.46756>; else goto <D.46757>;
  <D.46756>:
  stderr.503_295 = stderr;
  fprintf (stderr.503_295, "\npbzip2: *WARNING: off_t variable size only %u bits!\n", 64);
  if (decompress == 1) goto <D.46758>; else goto <D.46759>;
  <D.46758>:
  stderr.504_296 = stderr;
  fprintf (stderr.504_296, " You will only able to uncompress files smaller than 2GB in size.\n\n");
  goto <D.46760>;
  <D.46759>:
  stderr.505_297 = stderr;
  fprintf (stderr.505_297, " You will only able to compress files smaller than 2GB in size.\n\n");
  <D.46760>:
  goto <D.46761>;
  <D.46757>:
  <D.46761>:
  goto <D.46762>;
  <D.46755>:
  <D.46762>:
  if (useLoadAverage == 1) goto <D.46763>; else goto <D.46764>;
  <D.46763>:
  ret = getloadavg (&loadAvgArray, 3);
  if (ret != 3) goto <D.46765>; else goto <D.46766>;
  <D.46765>:
  loadAverage = 0.0;
  useLoadAverage = 0;
  QuietMode.506_298 = QuietMode;
  if (QuietMode.506_298 != 1) goto <D.46767>; else goto <D.46768>;
  <D.46767>:
  stderr.507_299 = stderr;
  fprintf (stderr.507_299, "pbzip2:  *WARNING: Could not get load average!  Using requested processors...\n");
  goto <D.46769>;
  <D.46768>:
  <D.46769>:
  goto <D.46770>;
  <D.46766>:
  loadAverage = loadAvgArray[0];
  _300 = loadAverage + 5.0e-1;
  _301 = (int) _300;
  numCPUidle = numCPUtotal - _301;
  numCPU.508_302 = numCPU;
  if (numCPUidle < numCPU.508_302) goto <D.46771>; else goto <D.46772>;
  <D.46771>:
  numCPU = numCPUidle;
  goto <D.46773>;
  <D.46772>:
  <D.46773>:
  numCPU.509_303 = numCPU;
  if (numCPU.509_303 <= 0) goto <D.46774>; else goto <D.46775>;
  <D.46774>:
  numCPU = 1;
  goto <D.46776>;
  <D.46775>:
  <D.46776>:
  <D.46770>:
  goto <D.46777>;
  <D.46764>:
  <D.46777>:
  initChildThreadAttributes ();
  sigInFilename = 0B;
  sigOutFilename = 0B;
  ret = setupSignalHandling ();
  if (ret != 0) goto <D.46778>; else goto <D.46779>;
  <D.46778>:
  stderr.510_304 = stderr;
  fprintf (stderr.510_304, "pbzip2: *ERROR: Can\'t setup signal handling [%d]. Aborting!\n", ret);
  D.46592 = 1;
  goto <D.47189>;
  <D.46779>:
  ret = setupTerminator ();
  if (ret != 0) goto <D.46780>; else goto <D.46781>;
  <D.46780>:
  stderr.511_305 = stderr;
  fprintf (stderr.511_305, "pbzip2: *ERROR: Can\'t setup terminator thread [%d]. Aborting!\n", ret);
  D.46592 = 1;
  goto <D.47189>;
  <D.46781>:
  numCPU.512_306 = numCPU;
  if (numCPU.512_306 <= 0) goto <D.46782>; else goto <D.46783>;
  <D.46782>:
  numCPU = 1;
  goto <D.46784>;
  <D.46783>:
  <D.46784>:
  QuietMode.513_307 = QuietMode;
  if (QuietMode.513_307 != 1) goto <D.46785>; else goto <D.46786>;
  <D.46785>:
  if (testFile != 1) goto <D.46787>; else goto <D.46788>;
  <D.46787>:
  numCPU.514_308 = numCPU;
  stderr.515_309 = stderr;
  fprintf (stderr.515_309, "\n         # CPUs: %d\n", numCPU.514_308);
  if (useLoadAverage == 1) goto <D.46789>; else goto <D.46790>;
  <D.46789>:
  stderr.516_310 = stderr;
  fprintf (stderr.516_310, "   Load Average: %.2f\n", loadAverage);
  goto <D.46791>;
  <D.46790>:
  <D.46791>:
  if (decompress != 1) goto <D.46792>; else goto <D.46793>;
  <D.46792>:
  BWTblockSize.517_311 = BWTblockSize;
  stderr.518_312 = stderr;
  fprintf (stderr.518_312, " BWT Block Size: %d00 KB\n", BWTblockSize.517_311);
  if (blockSize <= 99999) goto <D.46794>; else goto <D.46795>;
  <D.46794>:
  stderr.519_313 = stderr;
  fprintf (stderr.519_313, "File Block Size: %d bytes\n", blockSize);
  goto <D.46796>;
  <D.46795>:
  _314 = blockSize / 1000;
  stderr.520_315 = stderr;
  fprintf (stderr.520_315, "File Block Size: %d KB\n", _314);
  <D.46796>:
  goto <D.46797>;
  <D.46793>:
  <D.46797>:
  _316 = maxMemory / 1000000;
  stderr.521_317 = stderr;
  fprintf (stderr.521_317, " Maximum Memory: %d MB\n", _316);
  ChildThreadStackSize.522_318 = ChildThreadStackSize;
  if (ChildThreadStackSize.522_318 > 0) goto <D.46798>; else goto <D.46799>;
  <D.46798>:
  ChildThreadStackSize.523_319 = ChildThreadStackSize;
  _320 = ChildThreadStackSize.523_319 / 1024;
  stderr.524_321 = stderr;
  fprintf (stderr.524_321, "     Stack Size: %d KB\n", _320);
  goto <D.46800>;
  <D.46799>:
  <D.46800>:
  if (decompress == 1) goto <D.46801>; else goto <D.46802>;
  <D.46801>:
  IgnoreTrailingGarbageFlag.526_322 = IgnoreTrailingGarbageFlag;
  if (IgnoreTrailingGarbageFlag.526_322 == 1) goto <D.46804>; else goto <D.46805>;
  <D.46804>:
  iftmp.525 = "on";
  goto <D.46806>;
  <D.46805>:
  iftmp.525 = "off";
  <D.46806>:
  stderr.527_323 = stderr;
  fprintf (stderr.527_323, " Ignore Trailing Garbage: %s\n", iftmp.525);
  goto <D.46807>;
  <D.46802>:
  <D.46807>:
  goto <D.46808>;
  <D.46788>:
  <D.46808>:
  stderr.528_324 = stderr;
  fprintf (stderr.528_324, "-------------------------------------------\n");
  goto <D.46809>;
  <D.46786>:
  <D.46809>:
  D.47123 = mutexesInit ();
  mutexesInitRet = D.47123;
  if (mutexesInitRet != 0) goto <D.46810>; else goto <D.46811>;
  <D.46810>:
  D.46592 = mutexesInitRet;
  goto <D.47189>;
  <D.46811>:
  numCPU.529_325 = numCPU;
  D.47124 = queueInit (numCPU.529_325);
  _326 = D.47124;
  FifoQueue = _326;
  fifo = FifoQueue;
  if (fifo == 0B) goto <D.46812>; else goto <D.46813>;
  <D.46812>:
  stderr.530_327 = stderr;
  fprintf (stderr.530_327, "pbzip2: *ERROR: Queue Init failed.  Aborting...\n");
  D.46592 = 1;
  goto <D.47189>;
  <D.46813>:
  fileLoop = 0;
  <D.46814>:
  FileListCount.531_328 = FileListCount;
  if (fileLoop >= FileListCount.531_328) goto <D.39886>; else goto <D.46815>;
  <D.46815>:
  producerDone = 0;
  InFileSize = 0;
  NumBlocks = 0;
  switchedMtToSt = 0;
  errLevelCurrentFile = 0;
  D.47125 = pbzip2::ErrorContext::getInstance ();
  _329 = D.47125;
  pbzip2::ErrorContext::reset (_329);
  FileList.532_330 = FileList;
  _331 = (long unsigned int) fileLoop;
  _332 = _331 * 8;
  _333 = FileList.532_330 + _332;
  InFilename = *_333;
  if (testFile != 0) goto <D.46816>; else goto <D.46817>;
  <D.46816>:
  QuietMode.533_334 = QuietMode;
  if (QuietMode.533_334 != 1) goto <D.46818>; else goto <D.46819>;
  <D.46818>:
  FileListCount.534_335 = FileListCount;
  _336 = fileLoop + 1;
  stderr.535_337 = stderr;
  fprintf (stderr.535_337, "      File #: %d of %d\n", _336, FileListCount.534_335);
  _338 = strcmp (InFilename, "-");
  if (_338 != 0) goto <D.46820>; else goto <D.46821>;
  <D.46820>:
  stderr.536_339 = stderr;
  fprintf (stderr.536_339, "     Testing: %s\n", InFilename);
  goto <D.46822>;
  <D.46821>:
  stderr.537_340 = stderr;
  fprintf (stderr.537_340, "     Testing: <stdin>\n");
  <D.46822>:
  goto <D.46823>;
  <D.46819>:
  <D.46823>:
  D.47126 = testCompressedData (InFilename);
  ret = D.47126;
  if (ret > 0) goto <D.46824>; else goto <D.46825>;
  <D.46824>:
  D.46592 = ret;
  goto <D.47189>;
  <D.46825>:
  if (ret == 0) goto <D.46826>; else goto <D.46827>;
  <D.46826>:
  QuietMode.538_341 = QuietMode;
  if (QuietMode.538_341 != 1) goto <D.46828>; else goto <D.46829>;
  <D.46828>:
  stderr.539_342 = stderr;
  fprintf (stderr.539_342, "        Test: OK\n");
  goto <D.46830>;
  <D.46829>:
  <D.46830>:
  goto <D.46831>;
  <D.46827>:
  errLevel = 2;
  <D.46831>:
  QuietMode.540_343 = QuietMode;
  if (QuietMode.540_343 != 1) goto <D.46832>; else goto <D.46833>;
  <D.46832>:
  stderr.541_344 = stderr;
  fprintf (stderr.541_344, "-------------------------------------------\n");
  goto <D.46834>;
  <D.46833>:
  <D.46834>:
  // predicted unlikely by continue predictor.
  goto <D.39887>;
  <D.46817>:
  std::allocator<char>::allocator (&D.39832);
  FileList.542_345 = FileList;
  _346 = (long unsigned int) fileLoop;
  _347 = _346 * 8;
  _348 = FileList.542_345 + _347;
  _349 = *_348;
  std::__cxx11::basic_string<char>::basic_string (&D.39833, _349, &D.39832);
  std::__cxx11::basic_string<char>::operator= (&outFilename, &D.39833);
  std::__cxx11::basic_string<char>::~basic_string (&D.39833);
  D.39833 = {CLOBBER};
  std::allocator<char>::~allocator (&D.39832);
  D.39832 = {CLOBBER};
  if (decompress == 1) goto <D.46837>; else goto <D.46835>;
  <D.46837>:
  _350 = strcmp (InFilename, "-");
  if (_350 != 0) goto <D.46838>; else goto <D.46835>;
  <D.46838>:
  D.47129 = open (InFilename, 0);
  hInfile = D.47129;
  if (hInfile == -1) goto <D.46839>; else goto <D.46840>;
  <D.46839>:
  _351 = __errno_location ();
  _352 = *_351;
  stderr.543_353 = stderr;
  pbzip2::ErrorContext::printErrnoMsg (stderr.543_353, _352);
  stderr.544_354 = stderr;
  fprintf (stderr.544_354, "pbzip2: *ERROR: File [%s] NOT found!  Skipping...\n", InFilename);
  stderr.545_355 = stderr;
  fprintf (stderr.545_355, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47143>;
  <D.46840>:
  memset (&tmpBuff, 0, 50);
  _356 = strlen (&bz2Header);
  _357 = _356 + 1;
  D.47130 = do_read (hInfile, &tmpBuff, _357);
  _358 = D.47130;
  size = (size_t) _358;
  do_close (hInfile);
  if (size == 18446744073709551615) goto <D.46841>; else goto <D.46844>;
  <D.46844>:
  _359 = strlen (&bz2Header);
  _360 = _359 + 1;
  if (size < _360) goto <D.46841>; else goto <D.46842>;
  <D.46841>:
  D.47131 = pbzip2::ErrorContext::getInstance ();
  _361 = D.47131;
  stderr.546_362 = stderr;
  pbzip2::ErrorContext::printErrorMessages (_361, stderr.546_362);
  stderr.547_363 = stderr;
  fprintf (stderr.547_363, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
  stderr.548_364 = stderr;
  fprintf (stderr.548_364, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47143>;
  goto <D.46843>;
  <D.46842>:
  D.47132 = memstr<unsigned char*, char*> (&tmpBuff, 4, &bz2Header, 3);
  _365 = D.47132;
  retval.549 = _365 == 0B;
  if (retval.549 != 0) goto <D.46846>; else goto <D.46847>;
  <D.46846>:
  stderr.550_366 = stderr;
  fprintf (stderr.550_366, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
  stderr.551_367 = stderr;
  fprintf (stderr.551_367, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47143>;
  <D.46847>:
  _368 = strlen (&bz2Header);
  _369 = (unsigned int) _368;
  _370 = _369 + 4294967292;
  _371 = (int) _370;
  _372 = &bz2Header + 4;
  _373 = (unsigned int) size;
  _374 = _373 + 4294967292;
  _375 = (int) _374;
  _376 = &tmpBuff + 4;
  D.47133 = memstr<unsigned char*, char*> (_376, _375, _372, _371);
  _377 = D.47133;
  retval.552 = _377 == 0B;
  if (retval.552 != 0) goto <D.46849>; else goto <D.46850>;
  <D.46849>:
  _378 = strlen (&bz2Header);
  _379 = (unsigned int) _378;
  _380 = _379 + 4294967292;
  _381 = (int) _380;
  _382 = &Bz2HeaderZero + 4;
  _383 = (unsigned int) size;
  _384 = _383 + 4294967292;
  _385 = (int) _384;
  _386 = &tmpBuff + 4;
  D.47134 = memstr<unsigned char*, unsigned char*> (_386, _385, _382, _381);
  _387 = D.47134;
  retval.553 = _387 == 0B;
  if (retval.553 != 0) goto <D.46852>; else goto <D.46853>;
  <D.46852>:
  stderr.554_388 = stderr;
  fprintf (stderr.554_388, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
  stderr.555_389 = stderr;
  fprintf (stderr.555_389, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47143>;
  <D.46853>:
  goto <D.46854>;
  <D.46850>:
  <D.46854>:
  _390 = tmpBuff[3];
  _391 = _390 + 207;
  if (_391 <= 8) goto <D.46855>; else goto <D.46856>;
  <D.46855>:
  _392 = tmpBuff[3];
  _393 = (char) _392;
  BWTblockSizeChar = _393;
  goto <D.46857>;
  <D.46856>:
  stderr.556_394 = stderr;
  fprintf (stderr.556_394, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename);
  stderr.557_395 = stderr;
  fprintf (stderr.557_395, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47143>;
  <D.46857>:
  <D.46843>:
  std::allocator<char>::allocator (&D.39850);
  std::__cxx11::basic_string<char>::basic_string (&bz2Tail, ".bz2", &D.39850);
  std::allocator<char>::~allocator (&D.39850);
  D.39850 = {CLOBBER};
  std::allocator<char>::allocator (&D.39853);
  std::__cxx11::basic_string<char>::basic_string (&tbz2Tail, ".tbz2", &D.39853);
  std::allocator<char>::~allocator (&D.39853);
  D.39853 = {CLOBBER};
  retval.558 = ends_with_icase<char> (&outFilename, &bz2Tail);
  if (retval.558 != 0) goto <D.46859>; else goto <D.46860>;
  <D.46859>:
  _396 = std::__cxx11::basic_string<char>::size (&outFilename);
  _397 = std::__cxx11::basic_string<char>::size (&bz2Tail);
  _398 = _396 - _397;
  std::__cxx11::basic_string<char>::resize (&outFilename, _398);
  goto <D.46861>;
  <D.46860>:
  retval.559 = ends_with_icase<char> (&outFilename, &tbz2Tail);
  if (retval.559 != 0) goto <D.46863>; else goto <D.46864>;
  <D.46863>:
  _399 = std::__cxx11::basic_string<char>::size (&outFilename);
  _400 = std::__cxx11::basic_string<char>::size (&tbz2Tail);
  _401 = _399 - _400;
  std::__cxx11::basic_string<char>::resize (&outFilename, _401);
  std::__cxx11::basic_string<char>::operator+= (&outFilename, ".tar");
  goto <D.46865>;
  <D.46864>:
  std::__cxx11::basic_string<char>::operator+= (&outFilename, ".out");
  <D.46865>:
  <D.46861>:
  std::__cxx11::basic_string<char>::~basic_string (&tbz2Tail);
  std::__cxx11::basic_string<char>::~basic_string (&bz2Tail);
  bz2Tail = {CLOBBER};
  tbz2Tail = {CLOBBER};
  goto <D.47142>;
  <D.47143>:
  bz2Tail = {CLOBBER};
  tbz2Tail = {CLOBBER};
  goto <D.39887>;
  <D.47142>:
  goto <D.46836>;
  <D.46835>:
  std::allocator<char>::allocator (&D.39859);
  std::__cxx11::basic_string<char>::basic_string (&bz2Tail, ".bz2", &D.39859);
  std::allocator<char>::~allocator (&D.39859);
  D.39859 = {CLOBBER};
  std::allocator<char>::allocator (&D.39861);
  std::__cxx11::basic_string<char>::basic_string (&D.39862, InFilename, &D.39861);
  retval.560 = ends_with_icase<char> (&D.39862, &bz2Tail);
  std::__cxx11::basic_string<char>::~basic_string (&D.39862);
  D.39862 = {CLOBBER};
  std::allocator<char>::~allocator (&D.39861);
  D.39861 = {CLOBBER};
  if (retval.560 != 0) goto <D.46867>; else goto <D.46868>;
  <D.46867>:
  stderr.561_402 = stderr;
  fprintf (stderr.561_402, "pbzip2: *ERROR: Input file [%s] already has a .bz2 extension!  Skipping...\n", InFilename);
  stderr.562_403 = stderr;
  fprintf (stderr.562_403, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47150>;
  <D.46868>:
  std::__cxx11::basic_string<char>::operator+= (&outFilename, &bz2Tail);
  std::__cxx11::basic_string<char>::~basic_string (&bz2Tail);
  goto <D.47149>;
  <D.47150>:
  std::__cxx11::basic_string<char>::~basic_string (&bz2Tail);
  goto <D.47153>;
  <D.47149>:
  bz2Tail = {CLOBBER};
  goto <D.47152>;
  <D.47153>:
  bz2Tail = {CLOBBER};
  goto <D.39887>;
  <D.47152>:
  <D.46836>:
  safe_mutex_lock (&ErrorHandlerMutex);
  sigInFilename = InFilename;
  _404 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  sigOutFilename = _404;
  safe_mutex_unlock (&ErrorHandlerMutex);
  _405 = strcmp (InFilename, "-");
  if (_405 != 0) goto <D.46869>; else goto <D.46870>;
  <D.46869>:
  D.47154 = open (InFilename, 0);
  hInfile = D.47154;
  if (hInfile == -1) goto <D.46871>; else goto <D.46872>;
  <D.46871>:
  stderr.563_406 = stderr;
  fprintf (stderr.563_406, "pbzip2: *ERROR: File [%s] NOT found!  Skipping...\n", InFilename);
  stderr.564_407 = stderr;
  fprintf (stderr.564_407, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47157>;
  <D.46872>:
  fstat (hInfile, &statbuf);
  _408 = statbuf.st_mode;
  _409 = _408 & 61440;
  if (_409 == 16384) goto <D.46873>; else goto <D.46874>;
  <D.46873>:
  stderr.565_410 = stderr;
  fprintf (stderr.565_410, "pbzip2: *ERROR: File [%s] is a directory!  Skipping...\n", InFilename);
  stderr.566_411 = stderr;
  fprintf (stderr.566_411, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47157>;
  <D.46874>:
  _412 = statbuf.st_mode;
  _413 = _412 & 61440;
  if (_413 != 32768) goto <D.46875>; else goto <D.46876>;
  <D.46875>:
  stderr.567_414 = stderr;
  fprintf (stderr.567_414, "pbzip2: *ERROR: File [%s] is not a regular file!  Skipping...\n", InFilename);
  stderr.568_415 = stderr;
  fprintf (stderr.568_415, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47157>;
  <D.46876>:
  _416 = statbuf.st_size;
  InFileSize = _416;
  InFileSize.569_417 = InFileSize;
  if (InFileSize.569_417 == 0) goto <D.46877>; else goto <D.46878>;
  <D.46877>:
  if (decompress == 1) goto <D.46879>; else goto <D.46880>;
  <D.46879>:
  stderr.570_418 = stderr;
  fprintf (stderr.570_418, "pbzip2: *ERROR: File is of size 0 [%s]!  Skipping...\n", InFilename);
  stderr.571_419 = stderr;
  fprintf (stderr.571_419, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47157>;
  <D.46880>:
  zeroByteFile = 1;
  goto <D.46881>;
  <D.46878>:
  zeroByteFile = 0;
  <D.46881>:
  _420 = getFileMetaData (InFilename);
  retval.572 = _420 != 0;
  if (retval.572 != 0) goto <D.46883>; else goto <D.46884>;
  <D.46883>:
  stderr.573_421 = stderr;
  fprintf (stderr.573_421, "pbzip2: *ERROR: Could not get file meta data from [%s]!  Skipping...\n", InFilename);
  stderr.574_422 = stderr;
  fprintf (stderr.574_422, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.47157>;
  <D.46884>:
  statbuf = {CLOBBER};
  goto <D.47156>;
  <D.47157>:
  statbuf = {CLOBBER};
  goto <D.39887>;
  <D.47156>:
  goto <D.46885>;
  <D.46870>:
  hInfile = 0;
  InFileSize = -1;
  <D.46885>:
  OutputStdOut.577_423 = OutputStdOut;
  if (OutputStdOut.577_423 == 0) goto <D.46890>; else goto <D.46888>;
  <D.46890>:
  _424 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  D.47158 = check_file_exists (_424);
  _425 = D.47158;
  if (_425 != 0) goto <D.46891>; else goto <D.46888>;
  <D.46891>:
  iftmp.576 = 1;
  goto <D.46889>;
  <D.46888>:
  iftmp.576 = 0;
  <D.46889>:
  retval.575 = iftmp.576;
  if (retval.575 != 0) goto <D.46892>; else goto <D.46893>;
  <D.46892>:
  if (force != 1) goto <D.46894>; else goto <D.46895>;
  <D.46894>:
  _426 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  stderr.578_427 = stderr;
  fprintf (stderr.578_427, "pbzip2: *ERROR: Output file [%s] already exists!  Use -f to overwrite...\n", _426);
  stderr.579_428 = stderr;
  fprintf (stderr.579_428, "-------------------------------------------\n");
  errLevel = 1;
  // predicted unlikely by continue predictor.
  goto <D.39887>;
  <D.46895>:
  _429 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  remove (_429);
  goto <D.46896>;
  <D.46893>:
  <D.46896>:
  if (readEntireFile == 1) goto <D.46897>; else goto <D.46898>;
  <D.46897>:
  if (hInfile == 0) goto <D.46899>; else goto <D.46900>;
  <D.46899>:
  QuietMode.580_430 = QuietMode;
  if (QuietMode.580_430 != 1) goto <D.46901>; else goto <D.46902>;
  <D.46901>:
  stderr.581_431 = stderr;
  fprintf (stderr.581_431, " *Warning: Ignoring -r switch since input is stdin.\n");
  goto <D.46903>;
  <D.46902>:
  <D.46903>:
  goto <D.46904>;
  <D.46900>:
  InFileSize.582_432 = InFileSize;
  numCPU.583_433 = numCPU;
  _434 = (long int) numCPU.583_433;
  _435 = InFileSize.582_432 / _434;
  blockSize = (int) _435;
  <D.46904>:
  goto <D.46905>;
  <D.46898>:
  <D.46905>:
  QuietMode.584_436 = QuietMode;
  if (QuietMode.584_436 != 1) goto <D.46906>; else goto <D.46907>;
  <D.46906>:
  FileListCount.585_437 = FileListCount;
  _438 = fileLoop + 1;
  stderr.586_439 = stderr;
  fprintf (stderr.586_439, "         File #: %d of %d\n", _438, FileListCount.585_437);
  if (hInfile != 0) goto <D.46909>; else goto <D.46910>;
  <D.46909>:
  iftmp.587 = InFilename;
  goto <D.46911>;
  <D.46910>:
  iftmp.587 = "<stdin>";
  <D.46911>:
  stderr.588_440 = stderr;
  fprintf (stderr.588_440, "     Input Name: %s\n", iftmp.587);
  OutputStdOut.589_441 = OutputStdOut;
  if (OutputStdOut.589_441 == 0) goto <D.46912>; else goto <D.46913>;
  <D.46912>:
  _442 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  stderr.590_443 = stderr;
  fprintf (stderr.590_443, "    Output Name: %s\n\n", _442);
  goto <D.46914>;
  <D.46913>:
  stderr.591_444 = stderr;
  fprintf (stderr.591_444, "    Output Name: <stdout>\n\n");
  <D.46914>:
  if (decompress == 1) goto <D.46915>; else goto <D.46916>;
  <D.46915>:
  BWTblockSizeChar.592_445 = BWTblockSizeChar;
  _446 = (int) BWTblockSizeChar.592_445;
  stderr.593_447 = stderr;
  fprintf (stderr.593_447, " BWT Block Size: %c00k\n", _446);
  goto <D.46917>;
  <D.46916>:
  <D.46917>:
  _448 = strcmp (InFilename, "-");
  if (_448 != 0) goto <D.46918>; else goto <D.46919>;
  <D.46918>:
  InFileSize.594_449 = InFileSize;
  InFileSize.595_450 = (long unsigned int) InFileSize.594_449;
  stderr.596_451 = stderr;
  fprintf (stderr.596_451, "     Input Size: %lu bytes\n", InFileSize.595_450);
  goto <D.46920>;
  <D.46919>:
  <D.46920>:
  goto <D.46921>;
  <D.46907>:
  <D.46921>:
  if (decompress == 1) goto <D.46922>; else goto <D.46923>;
  <D.46922>:
  numBlocks = 0;
  numCPU.597_452 = numCPU;
  if (numCPU.597_452 == 1) goto <D.46924>; else goto <D.46927>;
  <D.46927>:
  InFileSize.598_453 = InFileSize;
  if (InFileSize.598_453 <= 999999) goto <D.46924>; else goto <D.46925>;
  <D.46924>:
  noThreads = 1;
  goto <D.46926>;
  <D.46925>:
  noThreads = 0;
  <D.46926>:
  numCPU.599_454 = numCPU;
  if (numCPU.599_454 > 1) goto <D.46928>; else goto <D.46929>;
  <D.46928>:
  _455 = strcmp (InFilename, "-");
  if (_455 == 0) goto <D.46930>; else goto <D.46931>;
  <D.46930>:
  noThreads = 0;
  goto <D.46932>;
  <D.46931>:
  <D.46932>:
  <D.46929>:
  goto <D.46933>;
  <D.46923>:
  InFileSize.600_456 = InFileSize;
  if (InFileSize.600_456 > 0) goto <D.46934>; else goto <D.46935>;
  <D.46934>:
  _457 = (long int) blockSize;
  InFileSize.601_458 = InFileSize;
  _459 = _457 + InFileSize.601_458;
  _460 = _459 + -1;
  _461 = (long int) blockSize;
  _462 = _460 / _461;
  numBlocks = (int) _462;
  if (numBlocks == 1) goto <D.46936>; else goto <D.46939>;
  <D.46939>:
  numCPU.602_463 = numCPU;
  if (numCPU.602_463 == 1) goto <D.46936>; else goto <D.46937>;
  <D.46936>:
  noThreads = 1;
  goto <D.46938>;
  <D.46937>:
  noThreads = 0;
  <D.46938>:
  goto <D.46940>;
  <D.46935>:
  numBlocks = 10000;
  <D.46940>:
  if (zeroByteFile == 1) goto <D.46941>; else goto <D.46942>;
  <D.46941>:
  hOutfile = 1;
  OutputStdOut.603_464 = OutputStdOut;
  if (OutputStdOut.603_464 == 0) goto <D.46943>; else goto <D.46944>;
  <D.46943>:
  _465 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  D.47159 = safe_open_output (_465);
  hOutfile = D.47159;
  if (hOutfile == -1) goto <D.46945>; else goto <D.46946>;
  <D.46945>:
  _466 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  handle_error (1, 1, "pbzip2: *ERROR: Could not create output file [%s]!\n", _466);
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.39886>;
  <D.46946>:
  goto <D.46947>;
  <D.46944>:
  <D.46947>:
  D.47160 = do_write (hOutfile, &Bz2HeaderZero, 14);
  _467 = D.47160;
  ret = (int) _467;
  close_ret = 0;
  OutputStdOut.604_468 = OutputStdOut;
  if (OutputStdOut.604_468 == 0) goto <D.46948>; else goto <D.46949>;
  <D.46948>:
  D.47161 = do_close (hOutfile);
  close_ret = D.47161;
  _469 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  D.47162 = writeFileMetaData (_469);
  _470 = D.47162;
  retval.605 = _470 != 0;
  if (retval.605 != 0) goto <D.46951>; else goto <D.46952>;
  <D.46951>:
  _471 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  handle_error (0, -1, "pbzip2: *ERROR: Could not write file meta data to [%s]!\n", _471);
  goto <D.46953>;
  <D.46952>:
  <D.46953>:
  goto <D.46954>;
  <D.46949>:
  <D.46954>:
  _472 = ret != 14;
  _473 = close_ret == -1;
  _474 = _472 | _473;
  if (_474 != 0) goto <D.46955>; else goto <D.46956>;
  <D.46955>:
  _475 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  handle_error (1, 1, "pbzip2: *ERROR: Could not write to file [%s]! Aborting...\n", _475);
  stderr.606_476 = stderr;
  fprintf (stderr.606_476, "-------------------------------------------\n");
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.39886>;
  <D.46956>:
  QuietMode.607_477 = QuietMode;
  if (QuietMode.607_477 != 1) goto <D.46957>; else goto <D.46958>;
  <D.46957>:
  stderr.608_478 = stderr;
  fprintf (stderr.608_478, "    Output Size: %u bytes\n", 14);
  stderr.609_479 = stderr;
  fprintf (stderr.609_479, "-------------------------------------------\n");
  goto <D.46959>;
  <D.46958>:
  <D.46959>:
  _480 = keep != 1;
  _481 = errLevelCurrentFile == 0;
  _482 = _480 & _481;
  if (_482 != 0) goto <D.46960>; else goto <D.46961>;
  <D.46960>:
  OutputStdOut.611_483 = OutputStdOut;
  if (OutputStdOut.611_483 != 0) goto <D.46963>; else goto <D.46966>;
  <D.46966>:
  _484 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  _485 = stat (_484, &statbuf);
  if (_485 == 0) goto <D.46963>; else goto <D.46964>;
  <D.46963>:
  iftmp.610 = 1;
  goto <D.46965>;
  <D.46964>:
  iftmp.610 = 0;
  <D.46965>:
  removeFlag = iftmp.610;
  if (removeFlag != 0) goto <D.46967>; else goto <D.46968>;
  <D.46967>:
  D.47163 = do_remove (InFilename);
  _486 = D.47163;
  retval.612 = _486 == -1;
  if (retval.612 != 0) goto <D.46970>; else goto <D.46971>;
  <D.46970>:
  handle_error (0, 1, "Can\'t remove input file [%s]!", InFilename);
  goto <D.46972>;
  <D.46971>:
  <D.46972>:
  goto <D.46973>;
  <D.46968>:
  <D.46973>:
  statbuf = {CLOBBER};
  goto <D.46974>;
  <D.46961>:
  <D.46974>:
  // predicted unlikely by continue predictor.
  goto <D.39887>;
  <D.46942>:
  <D.46933>:
  NumBlocksEstimated = numBlocks;
  _487 = maxMemory / blockSize;
  _488 = (long unsigned int) _487;
  NumBufferedBlocksMax = _488;
  NumBufferedBlocksMax.613_489 = NumBufferedBlocksMax;
  _490 = (int) NumBufferedBlocksMax.613_489;
  numCPU.614_491 = numCPU;
  _492 = numCPU.614_491 * 2;
  _493 = _490 - _492;
  if (_493 <= 0) goto <D.46975>; else goto <D.46976>;
  <D.46975>:
  NumBufferedBlocksMax = 1;
  goto <D.46977>;
  <D.46976>:
  NumBufferedBlocksMax.615_494 = NumBufferedBlocksMax;
  numCPU.616_495 = numCPU;
  _496 = numCPU.616_495 * 2;
  _497 = (long unsigned int) _496;
  _498 = NumBufferedBlocksMax.615_494 - _497;
  NumBufferedBlocksMax = _498;
  <D.46977>:
  numCPU.617_499 = numCPU;
  _500 = (long unsigned int) numCPU.617_499;
  NumBufferedBlocksMax.618_501 = NumBufferedBlocksMax;
  if (_500 > NumBufferedBlocksMax.618_501) goto <D.46978>; else goto <D.46979>;
  <D.46978>:
  if (maxMemorySwitch == 0) goto <D.46980>; else goto <D.46981>;
  <D.46980>:
  numCPU.619_502 = numCPU;
  _503 = (long unsigned int) numCPU.619_502;
  NumBufferedBlocksMax = _503;
  QuietMode.620_504 = QuietMode;
  if (QuietMode.620_504 != 1) goto <D.46982>; else goto <D.46983>;
  <D.46982>:
  numCPU.621_505 = numCPU;
  numCPU.622_506 = numCPU;
  _507 = numCPU.622_506 * 2;
  _508 = (long unsigned int) _507;
  NumBufferedBlocksMax.623_509 = NumBufferedBlocksMax;
  _510 = _508 + NumBufferedBlocksMax.623_509;
  _511 = (long unsigned int) blockSize;
  _512 = _510 * _511;
  _513 = _512 / 1000000;
  stderr.624_514 = stderr;
  fprintf (stderr.624_514, "*Warning* Max memory limit increased to %lu MB to support %d CPUs\n", _513, numCPU.621_505);
  goto <D.46984>;
  <D.46983>:
  <D.46984>:
  goto <D.46985>;
  <D.46981>:
  QuietMode.625_515 = QuietMode;
  if (QuietMode.625_515 != 1) goto <D.46986>; else goto <D.46987>;
  <D.46986>:
  stderr.626_516 = stderr;
  fprintf (stderr.626_516, "*Warning* CPU usage and performance may be suboptimal due to max memory limit.\n");
  goto <D.46988>;
  <D.46987>:
  <D.46988>:
  <D.46985>:
  goto <D.46989>;
  <D.46979>:
  <D.46989>:
  LastGoodBlock = -1;
  MinErrorBlock = -1;
  NumBufferedBlocksMax.627_517 = NumBufferedBlocksMax;
  outputBufferInit (NumBufferedBlocksMax.627_517);
  if (decompress == 1) goto <D.46990>; else goto <D.46991>;
  <D.46990>:
  if (noThreads == 0) goto <D.46992>; else goto <D.46993>;
  <D.46992>:
  QuietMode.628_518 = QuietMode;
  if (QuietMode.628_518 != 1) goto <D.46994>; else goto <D.46995>;
  <D.46994>:
  stderr.629_519 = stderr;
  fprintf (stderr.629_519, "Decompressing data...\n");
  goto <D.46996>;
  <D.46995>:
  <D.46996>:
  i = 0;
  <D.46997>:
  _520 = (int) i;
  numCPU.630_521 = numCPU;
  if (_520 >= numCPU.630_521) goto <D.39888>; else goto <D.46998>;
  <D.46998>:
  _522 = fifo->consumers;
  _523 = i * 8;
  _524 = _522 + _523;
  ret = pthread_create (_524, &ChildThreadAttributes, consumer_decompress, fifo);
  if (ret != 0) goto <D.46999>; else goto <D.47000>;
  <D.46999>:
  D.47165 = pbzip2::ErrorContext::getInstance ();
  _525 = D.47165;
  pbzip2::ErrorContext::saveError (_525);
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create consumer thread #%u (code = %d)  Aborting...\n", i, ret);
  TerminatorThread.631_526 = TerminatorThread;
  D.47166 = pthread_join (TerminatorThread.631_526, 0B);
  ret = D.47166;
  D.46592 = 1;
  goto <D.47189>;
  <D.47000>:
  i = i + 1;
  goto <D.46997>;
  <D.39888>:
  _527 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  ret = pthread_create (&output, &ChildThreadAttributes, fileWriter, _527);
  if (ret != 0) goto <D.47001>; else goto <D.47002>;
  <D.47001>:
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create fileWriter thread (code = %d)  Aborting...\n", ret);
  TerminatorThread.632_528 = TerminatorThread;
  D.47167 = pthread_join (TerminatorThread.632_528, 0B);
  ret = D.47167;
  D.46592 = 1;
  goto <D.47189>;
  <D.47002>:
  InFileSize.633_529 = InFileSize;
  D.47168 = producer_decompress (hInfile, InFileSize.633_529, fifo);
  ret = D.47168;
  if (ret == -99) goto <D.47003>; else goto <D.47004>;
  <D.47003>:
  noThreads = 1;
  switchedMtToSt = 1;
  output.635_530 = output;
  D.47169 = pthread_join (output.635_530, 0B);
  _531 = D.47169;
  retval.634 = _531 != 0;
  if (retval.634 != 0) goto <D.47006>; else goto <D.47007>;
  <D.47006>:
  D.47170 = pbzip2::ErrorContext::getInstance ();
  _532 = D.47170;
  pbzip2::ErrorContext::saveError (_532);
  handle_error (1, 1, "pbzip2: *ERROR: Error joining fileWriter thread (code = %d)  Aborting...\n", ret);
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  TerminatorThread.636_533 = TerminatorThread;
  D.47171 = pthread_join (TerminatorThread.636_533, 0B);
  ret = D.47171;
  D.46592 = 1;
  goto <D.47189>;
  <D.47007>:
  goto <D.47008>;
  <D.47004>:
  if (ret != 0) goto <D.47009>; else goto <D.47010>;
  <D.47009>:
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47011>;
  <D.47010>:
  <D.47011>:
  <D.47008>:
  goto <D.47012>;
  <D.46993>:
  <D.47012>:
  _534 = noThreads == 1;
  _535 = errLevelCurrentFile == 0;
  _536 = _534 & _535;
  if (_536 != 0) goto <D.47013>; else goto <D.47014>;
  <D.47013>:
  QuietMode.637_537 = QuietMode;
  if (QuietMode.637_537 != 1) goto <D.47015>; else goto <D.47016>;
  <D.47015>:
  stderr.638_538 = stderr;
  fprintf (stderr.638_538, "Decompressing data (no threads)...\n");
  goto <D.47017>;
  <D.47016>:
  <D.47017>:
  if (hInfile > 0) goto <D.47018>; else goto <D.47019>;
  <D.47018>:
  close (hInfile);
  goto <D.47020>;
  <D.47019>:
  <D.47020>:
  _539 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  D.47172 = directdecompress (InFilename, _539);
  ret = D.47172;
  if (ret != 0) goto <D.47021>; else goto <D.47022>;
  <D.47021>:
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47023>;
  <D.47022>:
  <D.47023>:
  goto <D.47024>;
  <D.47014>:
  <D.47024>:
  goto <D.47025>;
  <D.46991>:
  if (noThreads == 0) goto <D.47026>; else goto <D.47027>;
  <D.47026>:
  QuietMode.639_540 = QuietMode;
  if (QuietMode.639_540 != 1) goto <D.47028>; else goto <D.47029>;
  <D.47028>:
  stderr.640_541 = stderr;
  fprintf (stderr.640_541, "Compressing data...\n");
  goto <D.47030>;
  <D.47029>:
  <D.47030>:
  i = 0;
  <D.47031>:
  _542 = (int) i;
  numCPU.641_543 = numCPU;
  if (_542 >= numCPU.641_543) goto <D.39890>; else goto <D.47032>;
  <D.47032>:
  _544 = fifo->consumers;
  _545 = i * 8;
  _546 = _544 + _545;
  ret = pthread_create (_546, &ChildThreadAttributes, consumer, fifo);
  if (ret != 0) goto <D.47033>; else goto <D.47034>;
  <D.47033>:
  D.47173 = pbzip2::ErrorContext::getInstance ();
  _547 = D.47173;
  pbzip2::ErrorContext::saveError (_547);
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create consumer thread #%u (code = %d)  Aborting...\n", i, ret);
  TerminatorThread.642_548 = TerminatorThread;
  pthread_join (TerminatorThread.642_548, 0B);
  D.46592 = 1;
  goto <D.47189>;
  <D.47034>:
  i = i + 1;
  goto <D.47031>;
  <D.39890>:
  _549 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  ret = pthread_create (&output, &ChildThreadAttributes, fileWriter, _549);
  if (ret != 0) goto <D.47035>; else goto <D.47036>;
  <D.47035>:
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create fileWriter thread (code = %d)  Aborting...\n", ret);
  TerminatorThread.643_550 = TerminatorThread;
  pthread_join (TerminatorThread.643_550, 0B);
  D.46592 = 1;
  goto <D.47189>;
  <D.47036>:
  D.47174 = producer (hInfile, blockSize, fifo);
  ret = D.47174;
  if (ret != 0) goto <D.47037>; else goto <D.47038>;
  <D.47037>:
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47039>;
  <D.47038>:
  <D.47039>:
  goto <D.47040>;
  <D.47027>:
  QuietMode.644_551 = QuietMode;
  if (QuietMode.644_551 != 1) goto <D.47041>; else goto <D.47042>;
  <D.47041>:
  stderr.645_552 = stderr;
  fprintf (stderr.645_552, "Compressing data (no threads)...\n");
  goto <D.47043>;
  <D.47042>:
  <D.47043>:
  _553 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  InFileSize.646_554 = InFileSize;
  D.47175 = directcompress (hInfile, InFileSize.646_554, blockSize, _553);
  ret = D.47175;
  if (ret != 0) goto <D.47044>; else goto <D.47045>;
  <D.47044>:
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47046>;
  <D.47045>:
  <D.47046>:
  <D.47040>:
  <D.47025>:
  if (noThreads == 0) goto <D.47047>; else goto <D.47048>;
  <D.47047>:
  output.647_555 = output;
  D.47176 = pthread_join (output.647_555, 0B);
  ret = D.47176;
  if (ret != 0) goto <D.47049>; else goto <D.47050>;
  <D.47049>:
  _556 = __errno_location ();
  _557 = *_556;
  stderr.648_558 = stderr;
  pbzip2::ErrorContext::printErrnoMsg (stderr.648_558, _557);
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47051>;
  <D.47050>:
  <D.47051>:
  goto <D.47052>;
  <D.47048>:
  <D.47052>:
  _559 = noThreads == 0;
  _560 = switchedMtToSt | _559;
  if (_560 != 0) goto <D.47053>; else goto <D.47054>;
  <D.47053>:
  i = 0;
  <D.47055>:
  _561 = (int) i;
  numCPU.649_562 = numCPU;
  if (_561 >= numCPU.649_562) goto <D.39892>; else goto <D.47056>;
  <D.47056>:
  _563 = fifo->consumers;
  _564 = i * 8;
  _565 = _563 + _564;
  _566 = *_565;
  D.47177 = pthread_join (_566, 0B);
  ret = D.47177;
  if (ret != 0) goto <D.47057>; else goto <D.47058>;
  <D.47057>:
  _567 = __errno_location ();
  _568 = *_567;
  stderr.650_569 = stderr;
  pbzip2::ErrorContext::printErrnoMsg (stderr.650_569, _568);
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47059>;
  <D.47058>:
  <D.47059>:
  i = i + 1;
  goto <D.47055>;
  <D.39892>:
  goto <D.47060>;
  <D.47054>:
  <D.47060>:
  D.47178 = syncGetTerminateFlag ();
  _570 = D.47178;
  retval.651 = _570 != 0;
  if (retval.651 != 0) goto <D.47062>; else goto <D.47063>;
  <D.47062>:
  errLevel = 1;
  errLevelCurrentFile = errLevel;
  goto <D.47064>;
  <D.47063>:
  <D.47064>:
  OutputStdOut.652_571 = OutputStdOut;
  if (OutputStdOut.652_571 == 0) goto <D.47065>; else goto <D.47066>;
  <D.47065>:
  _572 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  D.47179 = writeFileMetaData (_572);
  _573 = D.47179;
  retval.653 = _573 != 0;
  if (retval.653 != 0) goto <D.47068>; else goto <D.47069>;
  <D.47068>:
  _574 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  handle_error (0, -1, "pbzip2: *ERROR: Could not write file meta data to [%s]!\n", _574);
  goto <D.47070>;
  <D.47069>:
  <D.47070>:
  goto <D.47071>;
  <D.47066>:
  <D.47071>:
  _575 = keep != 1;
  _576 = errLevelCurrentFile == 0;
  _577 = _575 & _576;
  if (_577 != 0) goto <D.47072>; else goto <D.47073>;
  <D.47072>:
  OutputStdOut.655_578 = OutputStdOut;
  if (OutputStdOut.655_578 != 0) goto <D.47075>; else goto <D.47078>;
  <D.47078>:
  _579 = std::__cxx11::basic_string<char>::c_str (&outFilename);
  _580 = stat (_579, &statbuf);
  if (_580 == 0) goto <D.47075>; else goto <D.47076>;
  <D.47075>:
  iftmp.654 = 1;
  goto <D.47077>;
  <D.47076>:
  iftmp.654 = 0;
  <D.47077>:
  removeFlag = iftmp.654;
  if (removeFlag != 0) goto <D.47079>; else goto <D.47080>;
  <D.47079>:
  D.47180 = do_remove (InFilename);
  _581 = D.47180;
  retval.656 = _581 == -1;
  if (retval.656 != 0) goto <D.47082>; else goto <D.47083>;
  <D.47082>:
  handle_error (0, 1, "Can\'t remove input file [%s]!", InFilename);
  goto <D.47084>;
  <D.47083>:
  <D.47084>:
  goto <D.47085>;
  <D.47080>:
  <D.47085>:
  statbuf = {CLOBBER};
  goto <D.47086>;
  <D.47073>:
  <D.47086>:
  std::vector<outBuff>::clear (&OutputBuffer);
  queue::clear (fifo);
  if (errLevelCurrentFile == 0) goto <D.47091>; else goto <D.47089>;
  <D.47091>:
  D.47182 = syncGetTerminateFlag ();
  _582 = D.47182;
  if (_582 == 0) goto <D.47092>; else goto <D.47089>;
  <D.47092>:
  iftmp.658 = 1;
  goto <D.47090>;
  <D.47089>:
  iftmp.658 = 0;
  <D.47090>:
  retval.657 = iftmp.658;
  if (retval.657 != 0) goto <D.47093>; else goto <D.47094>;
  <D.47093>:
  safe_mutex_lock (&ErrorHandlerMutex);
  sigInFilename = 0B;
  sigOutFilename = 0B;
  safe_mutex_unlock (&ErrorHandlerMutex);
  goto <D.47095>;
  <D.47094>:
  <D.47095>:
  if (errLevelCurrentFile == 1) goto <D.47096>; else goto <D.47097>;
  <D.47096>:
  syncSetTerminateFlag (1);
  goto <D.39886>;
  <D.47097>:
  QuietMode.659_583 = QuietMode;
  if (QuietMode.659_583 != 1) goto <D.47098>; else goto <D.47099>;
  <D.47098>:
  stderr.660_584 = stderr;
  fprintf (stderr.660_584, "-------------------------------------------\n");
  goto <D.47100>;
  <D.47099>:
  <D.47100>:
  <D.39887>:
  fileLoop = fileLoop + 1;
  goto <D.46814>;
  <D.39886>:
  OutputStdOut.661_585 = OutputStdOut;
  if (OutputStdOut.661_585 == 1) goto <D.47101>; else goto <D.47102>;
  <D.47101>:
  D.47183 = close (1);
  ret = D.47183;
  if (ret == -1) goto <D.47103>; else goto <D.47104>;
  <D.47103>:
  D.47184 = pbzip2::ErrorContext::getInstance ();
  _586 = D.47184;
  pbzip2::ErrorContext::saveError (_586);
  handle_error (1, 1, "pbzip2: *ERROR: Failed to close STDOUT! Aborting...\n");
  exit (1);
  <D.47104>:
  goto <D.47105>;
  <D.47102>:
  <D.47105>:
  SIG_HANDLER_QUIT_SIGNAL.662_587 = SIG_HANDLER_QUIT_SIGNAL;
  SignalHandlerThread.663_588 = SignalHandlerThread;
  ret = pthread_kill (SignalHandlerThread.663_588, SIG_HANDLER_QUIT_SIGNAL.662_587);
  if (ret != 0) goto <D.47106>; else goto <D.47107>;
  <D.47106>:
  stderr.664_589 = stderr;
  fprintf (stderr.664_589, "Couldn\'t signal signal QUIT to SignalHandlerThread [%d]. Quitting prematurely!\n", ret);
  exit (errLevel);
  <D.47107>:
  SignalHandlerThread.665_590 = SignalHandlerThread;
  D.47185 = pthread_join (SignalHandlerThread.665_590, 0B);
  ret = D.47185;
  if (ret != 0) goto <D.47108>; else goto <D.47109>;
  <D.47108>:
  stderr.666_591 = stderr;
  fprintf (stderr.666_591, "Error on join of SignalHandlerThread [%d]\n", ret);
  goto <D.47110>;
  <D.47109>:
  <D.47110>:
  D.47186 = syncGetTerminateFlag ();
  _592 = D.47186;
  retval.667 = _592 == 0;
  if (retval.667 != 0) goto <D.47112>; else goto <D.47113>;
  <D.47112>:
  syncSetFinishedFlag (1);
  goto <D.47114>;
  <D.47113>:
  <D.47114>:
  TerminatorThread.668_593 = TerminatorThread;
  D.47187 = pthread_join (TerminatorThread.668_593, 0B);
  ret = D.47187;
  if (ret != 0) goto <D.47115>; else goto <D.47116>;
  <D.47115>:
  stderr.669_594 = stderr;
  fprintf (stderr.669_594, "Error on join of TerminatorThread [%d]\n", ret);
  goto <D.47117>;
  <D.47116>:
  <D.47117>:
  queueDelete (fifo);
  mutexesDelete ();
  disposeMemory<char*> (&FileList);
  gettimeofday (&tvStopTime, &tz);
  _595 = tvStartTime.tv_sec;
  _596 = (double) _595;
  _597 = tvStartTime.tv_usec;
  _598 = (double) _597;
  _599 = _598 / 1.0e+6;
  timeStart = _596 + _599;
  _600 = tvStopTime.tv_sec;
  _601 = (double) _600;
  _602 = tvStopTime.tv_usec;
  _603 = (double) _602;
  _604 = _603 / 1.0e+6;
  timeStop = _601 + _604;
  timeCalc = timeStop - timeStart;
  QuietMode.670_605 = QuietMode;
  if (QuietMode.670_605 != 1) goto <D.47118>; else goto <D.47119>;
  <D.47118>:
  stderr.671_606 = stderr;
  fprintf (stderr.671_606, "\n     Wall Clock: %f seconds\n", timeCalc);
  goto <D.47120>;
  <D.47119>:
  <D.47120>:
  exit (errLevel);
  <D.47189>:
  std::__cxx11::basic_string<char>::~basic_string (&outFilename);
  goto <D.47191>;
  <D.47191>:
  output = {CLOBBER};
  FileList = {CLOBBER};
  bz2Header = {CLOBBER};
  outFilename = {CLOBBER};
  cmdLineTemp = {CLOBBER};
  tmpBuff = {CLOBBER};
  stdinFile = {CLOBBER};
  tvStartTime = {CLOBBER};
  tvStopTime = {CLOBBER};
  tz = {CLOBBER};
  loadAvgArray = {CLOBBER};
  goto <D.47121>;
  D.46592 = 0;
  goto <D.47121>;
  <D.47121>:
  return D.46592;
  <D.47127>:
  std::allocator<char>::~allocator (&D.39832);
  resx 4
  <D.47128>:
  D.39832 = {CLOBBER};
  resx 3
  <D.47135>:
  std::allocator<char>::~allocator (&D.39850);
  resx 11
  <D.47136>:
  D.39850 = {CLOBBER};
  resx 10
  <D.47137>:
  std::allocator<char>::~allocator (&D.39853);
  resx 17
  <D.47138>:
  D.39853 = {CLOBBER};
  resx 16
  <D.47139>:
  std::__cxx11::basic_string<char>::~basic_string (&tbz2Tail);
  resx 21
  <D.47140>:
  std::__cxx11::basic_string<char>::~basic_string (&bz2Tail);
  resx 15
  <D.47141>:
  bz2Tail = {CLOBBER};
  tbz2Tail = {CLOBBER};
  resx 9
  <D.47144>:
  std::allocator<char>::~allocator (&D.39859);
  resx 27
  <D.47145>:
  D.39859 = {CLOBBER};
  resx 26
  <D.47146>:
  std::allocator<char>::~allocator (&D.39861);
  resx 33
  <D.47147>:
  D.39861 = {CLOBBER};
  resx 32
  <D.47148>:
  std::__cxx11::basic_string<char>::~basic_string (&bz2Tail);
  resx 31
  <D.47151>:
  bz2Tail = {CLOBBER};
  resx 25
  <D.47155>:
  statbuf = {CLOBBER};
  resx 40
  <D.47164>:
  statbuf = {CLOBBER};
  resx 42
  <D.47181>:
  statbuf = {CLOBBER};
  resx 44
  <D.47188>:
  std::__cxx11::basic_string<char>::~basic_string (&outFilename);
  resx 2
  <D.47190>:
  output = {CLOBBER};
  FileList = {CLOBBER};
  bz2Header = {CLOBBER};
  outFilename = {CLOBBER};
  cmdLineTemp = {CLOBBER};
  tmpBuff = {CLOBBER};
  stdinFile = {CLOBBER};
  tvStartTime = {CLOBBER};
  tvStopTime = {CLOBBER};
  tz = {CLOBBER};
  loadAvgArray = {CLOBBER};
  resx 1
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::basic_string() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2Ev, funcdef_no=2259, decl_uid=19661, cgraph_uid=999, symbol_order=1047)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::basic_string() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this)
{
  char * D.47192;
  struct allocator D.41798;

  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->_M_dataplus;
  std::allocator<char>::allocator (&D.41798);
  D.47192 = std::__cxx11::basic_string<char>::_M_local_data (this);
  _2 = D.47192;
  std::__cxx11::basic_string<char>::_Alloc_hider::_Alloc_hider (_1, _2, &D.41798);
  std::allocator<char>::~allocator (&D.41798);
  D.41798 = {CLOBBER};
  std::__cxx11::basic_string<char>::_M_set_length (this, 0);
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_M_local_dataEv, funcdef_no=2279, decl_uid=18751, cgraph_uid=1019, symbol_order=1067)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this)
{
  char * D.47193;

  _1 = &this->D.18739._M_local_buf;
  D.47193 = std::pointer_traits<char*>::pointer_to (_1);
  goto <D.47194>;
  <D.47194>:
  return D.47193;
}



;; Function static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = char] (_ZNSt14pointer_traitsIPcE10pointer_toERc, funcdef_no=2411, decl_uid=41896, cgraph_uid=1151, symbol_order=1199)

static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = char] (__make_not_void & __r)
{
  char * D.47195;

  D.47195 = std::addressof<char> (__r);
  goto <D.47196>;
  <D.47196>:
  return D.47195;
}



;; Function _Tp* std::addressof(_Tp&) [with _Tp = char] (_ZSt9addressofIcEPT_RS0_, funcdef_no=2482, decl_uid=42533, cgraph_uid=1223, symbol_order=1271)

_Tp* std::addressof(_Tp&) [with _Tp = char] (char & __r)
{
  char * D.47197;

  D.47197 = std::__addressof<char> (__r);
  goto <D.47198>;
  <D.47198>:
  return D.47197;
}



;; Function constexpr _Tp* std::__addressof(_Tp&) [with _Tp = char] (_ZSt11__addressofIcEPT_RS0_, funcdef_no=2483, decl_uid=43606, cgraph_uid=1222, symbol_order=1270)

constexpr _Tp* std::__addressof(_Tp&) [with _Tp = char] (char & __r)
{
  char * D.47199;

  D.47199 = __r;
  goto <D.47200>;
  <D.47200>:
  return D.47199;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_Alloc_hider::_Alloc_hider(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer, _Alloc&&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderC2EPcOS3_, funcdef_no=2283, decl_uid=18722, cgraph_uid=1023, symbol_order=1071)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_Alloc_hider::_Alloc_hider(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer, _Alloc&&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct _Alloc_hider * const this, char * __dat, struct allocator & __a)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = std::move<std::allocator<char>&> (__a);
  std::allocator<char>::allocator (this, _1);
  this->_M_p = __dat;
  return;
}



;; Function constexpr typename std::remove_reference< <template-parameter-1-1> >::type&& std::move(_Tp&&) [with _Tp = std::allocator<char>&] (_ZSt4moveIRSaIcEEONSt16remove_referenceIT_E4typeEOS3_, funcdef_no=2281, decl_uid=40904, cgraph_uid=1021, symbol_order=1069)

constexpr typename std::remove_reference< <template-parameter-1-1> >::type&& std::move(_Tp&&) [with _Tp = std::allocator<char>&] (struct allocator & __t)
{
  struct type & D.47201;

  D.47201 = __t;
  goto <D.47202>;
  <D.47202>:
  return D.47201;
}



;; Function std::allocator< <template-parameter-1-1> >::allocator(const std::allocator< <template-parameter-1-1> >&) [with _Tp = char] (_ZNSaIcEC2ERKS_, funcdef_no=2413, decl_uid=11729, cgraph_uid=1153, symbol_order=1201)

std::allocator< <template-parameter-1-1> >::allocator(const std::allocator< <template-parameter-1-1> >&) [with _Tp = char] (struct allocator * const this, const struct allocator & __a)
{
  __gnu_cxx::new_allocator<char>::new_allocator (this, __a);
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::new_allocator(const __gnu_cxx::new_allocator<_Tp>&) [with _Tp = char] (_ZN9__gnu_cxx13new_allocatorIcEC2ERKS1_, funcdef_no=2485, decl_uid=11664, cgraph_uid=1225, symbol_order=1273)

__gnu_cxx::new_allocator<_Tp>::new_allocator(const __gnu_cxx::new_allocator<_Tp>&) [with _Tp = char] (struct new_allocator * const this, const struct new_allocator & D.11666)
{
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_set_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_M_set_lengthEm, funcdef_no=2289, decl_uid=18758, cgraph_uid=1029, symbol_order=1077)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_set_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, size_type __n)
{
  char * D.47203;
  const char_type D.41909;

  std::__cxx11::basic_string<char>::_M_length (this, __n);
  D.41909 = 0;
  D.47203 = std::__cxx11::basic_string<char>::_M_data (this);
  _1 = D.47203;
  _2 = _1 + __n;
  std::char_traits<char>::assign (_2, &D.41909);
  D.41909 = {CLOBBER};
  return;
  <D.47204>:
  D.41909 = {CLOBBER};
  resx 1
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE9_M_lengthEm, funcdef_no=2288, decl_uid=18746, cgraph_uid=1028, symbol_order=1076)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, size_type __length)
{
  this->_M_string_length = __length;
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEv, funcdef_no=2274, decl_uid=18749, cgraph_uid=1014, symbol_order=1062)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  char * D.47205;

  D.47205 = this->_M_dataplus._M_p;
  goto <D.47206>;
  <D.47206>:
  return D.47205;
}



;; Function static void std::char_traits<char>::assign(std::char_traits<char>::char_type&, const char_type&) (_ZNSt11char_traitsIcE6assignERcRKc, funcdef_no=358, decl_uid=10336, cgraph_uid=47, symbol_order=48)

static void std::char_traits<char>::assign(std::char_traits<char>::char_type&, const char_type&) (char_type & __c1, const char_type & __c2)
{
  _1 = *__c2;
  *__c1 = _1;
  return;
}



;; Function char* strstr(char*, const char*) (*strstr, funcdef_no=1920, decl_uid=37062, cgraph_uid=661, symbol_order=663)

__attribute__((__pure__, __gnu_inline__, __always_inline__))
char* strstr(char*, const char*) (char * __haystack, const char * __needle)
{
  char * D.47207;

  D.47207 = __builtin_strstr (__haystack, __needle);
  goto <D.47208>;
  <D.47208>:
  return D.47207;
}



;; Function char* strcpy(char*, const char*) (strcpy, funcdef_no=1933, decl_uid=1056, cgraph_uid=674, symbol_order=676)

__attribute__((nothrow, __artificial__, __gnu_inline__, __always_inline__, __leaf__))
char* strcpy(char*, const char*) (char * restrict __dest, const char * restrict __src)
{
  char * D.47209;

  _1 = __builtin_object_size (__dest, 1);
  D.47209 = __builtin___strcpy_chk (__dest, __src, _1);
  goto <D.47210>;
  <D.47210>:
  return D.47209;
}



;; Function char* strncpy(char*, const char*, size_t) (strncpy, funcdef_no=1935, decl_uid=1080, cgraph_uid=676, symbol_order=678)

__attribute__((nothrow, __artificial__, __gnu_inline__, __always_inline__, __leaf__))
char* strncpy(char*, const char*, size_t) (char * restrict __dest, const char * restrict __src, size_t __len)
{
  char * D.47211;

  _1 = __builtin_object_size (__dest, 1);
  D.47211 = __builtin___strncpy_chk (__dest, __src, __len, _1);
  goto <D.47212>;
  <D.47212>:
  return D.47211;
}



;; Function int atoi(const char*) (atoi, funcdef_no=888, decl_uid=16946, cgraph_uid=222, symbol_order=224)

__attribute__((__pure__, __leaf__, __gnu_inline__))
int atoi(const char*) (const char * __nptr)
{
  int D.47213;

  _1 = strtol (__nptr, 0B, 10);
  D.47213 = (int) _1;
  goto <D.47214>;
  <D.47214>:
  return D.47213;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator=(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>&&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEaSEOS4_, funcdef_no=2264, decl_uid=19014, cgraph_uid=1004, symbol_order=1052)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator=(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>&&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, struct basic_string & __str)
{
  char * D.47258;
  char * D.47257;
  bool D.47256;
  char * D.47255;
  char * D.47254;
  bool D.47253;
  struct allocator_type & D.47252;
  struct allocator_type & D.47251;
  char * D.47250;
  struct allocator_type & D.47249;
  struct allocator_type & D.47248;
  bool D.47247;
  size_type __capacity;
  char * __data;
  struct basic_string & D.47245;
  bool retval.676;
  bool retval.675;
  bool retval.674;
  bool iftmp.673;
  bool retval.672;

  D.47247 = std::__cxx11::basic_string<char>::_M_is_local (this);
  _1 = D.47247;
  if (0 != 0) goto <D.47219>; else goto <D.47217>;
  <D.47219>:
  D.47248 = std::__cxx11::basic_string<char>::_M_get_allocator (__str);
  _2 = D.47248;
  D.47249 = std::__cxx11::basic_string<char>::_M_get_allocator (this);
  _3 = D.47249;
  _4 = std::operator!=<char> (_3, _2);
  if (_4 != 0) goto <D.47220>; else goto <D.47217>;
  <D.47220>:
  iftmp.673 = 1;
  goto <D.47218>;
  <D.47217>:
  iftmp.673 = 0;
  <D.47218>:
  retval.672 = iftmp.673;
  if (retval.672 != 0) goto <D.47221>; else goto <D.47222>;
  <D.47221>:
  _5 = this->D.18739._M_allocated_capacity;
  std::__cxx11::basic_string<char>::_M_destroy (this, _5);
  D.47250 = std::__cxx11::basic_string<char>::_M_local_data (this);
  _6 = D.47250;
  std::__cxx11::basic_string<char>::_M_data (this, _6);
  std::__cxx11::basic_string<char>::_M_set_length (this, 0);
  goto <D.47223>;
  <D.47222>:
  <D.47223>:
  D.47251 = std::__cxx11::basic_string<char>::_M_get_allocator (__str);
  _7 = D.47251;
  D.47252 = std::__cxx11::basic_string<char>::_M_get_allocator (this);
  _8 = D.47252;
  std::__alloc_on_move<std::allocator<char> > (_8, _7);
  D.47253 = std::__cxx11::basic_string<char>::_M_is_local (__str);
  retval.674 = D.47253;
  if (retval.674 != 0) goto <D.47225>; else goto <D.47226>;
  <D.47225>:
  _9 = std::__cxx11::basic_string<char>::size (__str);
  retval.675 = _9 != 0;
  if (retval.675 != 0) goto <D.47228>; else goto <D.47229>;
  <D.47228>:
  _10 = std::__cxx11::basic_string<char>::size (__str);
  D.47254 = std::__cxx11::basic_string<char>::_M_data (__str);
  _11 = D.47254;
  D.47255 = std::__cxx11::basic_string<char>::_M_data (this);
  _12 = D.47255;
  std::__cxx11::basic_string<char>::_S_copy (_12, _11, _10);
  goto <D.47230>;
  <D.47229>:
  <D.47230>:
  _13 = std::__cxx11::basic_string<char>::size (__str);
  std::__cxx11::basic_string<char>::_M_set_length (this, _13);
  goto <D.47231>;
  <D.47226>:
  if (1 != 0) goto <D.47232>; else goto <D.47233>;
  <D.47232>:
  __data = 0B;
  D.47256 = std::__cxx11::basic_string<char>::_M_is_local (this);
  _14 = D.47256;
  retval.676 = ~_14;
  if (retval.676 != 0) goto <D.47235>; else goto <D.47236>;
  <D.47235>:
  if (1 != 0) goto <D.47237>; else goto <D.47238>;
  <D.47237>:
  D.47257 = std::__cxx11::basic_string<char>::_M_data (this);
  __data = D.47257;
  __capacity = this->D.18739._M_allocated_capacity;
  goto <D.47239>;
  <D.47238>:
  _15 = this->D.18739._M_allocated_capacity;
  std::__cxx11::basic_string<char>::_M_destroy (this, _15);
  <D.47239>:
  goto <D.47240>;
  <D.47236>:
  <D.47240>:
  D.47258 = std::__cxx11::basic_string<char>::_M_data (__str);
  _16 = D.47258;
  std::__cxx11::basic_string<char>::_M_data (this, _16);
  _17 = std::__cxx11::basic_string<char>::length (__str);
  std::__cxx11::basic_string<char>::_M_length (this, _17);
  _18 = __str->D.18739._M_allocated_capacity;
  std::__cxx11::basic_string<char>::_M_capacity (this, _18);
  if (__data != 0B) goto <D.47241>; else goto <D.47242>;
  <D.47241>:
  std::__cxx11::basic_string<char>::_M_data (__str, __data);
  std::__cxx11::basic_string<char>::_M_capacity (__str, __capacity);
  goto <D.47243>;
  <D.47242>:
  _19 = &__str->D.18739._M_local_buf;
  std::__cxx11::basic_string<char>::_M_data (__str, _19);
  <D.47243>:
  goto <D.47244>;
  <D.47233>:
  std::__cxx11::basic_string<char>::assign (this, __str);
  <D.47244>:
  <D.47231>:
  std::__cxx11::basic_string<char>::clear (__str);
  D.47245 = this;
  goto <D.47246>;
  <D.47246>:
  return D.47245;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_destroy(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_destroyEm, funcdef_no=2398, decl_uid=18769, cgraph_uid=1138, symbol_order=1186)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_destroy(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, size_type __size)
{
  struct allocator_type & D.47260;
  char * D.47259;

  _1 = __size + 1;
  D.47259 = std::__cxx11::basic_string<char>::_M_data (this);
  _2 = D.47259;
  D.47260 = std::__cxx11::basic_string<char>::_M_get_allocator (this);
  _3 = D.47260;
  std::allocator_traits<std::allocator<char> >::deallocate (_3, _2, _1);
  return;
  <D.47262>:
  eh_dispatch 1
  resx 1
  <D.47261>:
  _4 = __builtin_eh_pointer (1);
  __cxa_call_unexpected (_4);
}



;; Function static void std::allocator_traits<std::allocator<_CharT> >::deallocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::pointer, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = char] (_ZNSt16allocator_traitsISaIcEE10deallocateERS0_Pcm, funcdef_no=2475, decl_uid=18572, cgraph_uid=1215, symbol_order=1263)

static void std::allocator_traits<std::allocator<_CharT> >::deallocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::pointer, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = char] (struct allocator_type & __a, char * __p, size_type __n)
{
  __gnu_cxx::new_allocator<char>::deallocate (__a, __p, __n);
  return;
}



;; Function void __gnu_cxx::new_allocator<_Tp>::deallocate(__gnu_cxx::new_allocator<_Tp>::pointer, __gnu_cxx::new_allocator<_Tp>::size_type) [with _Tp = char] (_ZN9__gnu_cxx13new_allocatorIcE10deallocateEPcm, funcdef_no=2543, decl_uid=11623, cgraph_uid=1283, symbol_order=1331)

void __gnu_cxx::new_allocator<_Tp>::deallocate(__gnu_cxx::new_allocator<_Tp>::pointer, __gnu_cxx::new_allocator<_Tp>::size_type) [with _Tp = char] (struct new_allocator * const this, char * __p, size_type D.11626)
{
  operator delete (__p);
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE16_M_get_allocatorEv, funcdef_no=2280, decl_uid=18844, cgraph_uid=1020, symbol_order=1068)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this)
{
  struct allocator_type & D.47263;

  D.47263 = &this->_M_dataplus;
  goto <D.47264>;
  <D.47264>:
  return D.47263;
}



;; Function void std::__alloc_on_move(_Alloc&, _Alloc&) [with _Alloc = std::allocator<char>] (_ZSt15__alloc_on_moveISaIcEEvRT_S2_, funcdef_no=2399, decl_uid=41813, cgraph_uid=1139, symbol_order=1187)

void std::__alloc_on_move(_Alloc&, _Alloc&) [with _Alloc = std::allocator<char>] (struct allocator & __one, struct allocator & __two)
{
  struct integral_constant D.47265;
  struct __pocma D.42478;

  std::__do_alloc_on_move<std::allocator<char> > (__one, __two, D.47265);
  D.42478 = {CLOBBER};
  return;
}



;; Function void std::__do_alloc_on_move(_Alloc&, _Alloc&, std::true_type) [with _Alloc = std::allocator<char>] (_ZSt18__do_alloc_on_moveISaIcEEvRT_S2_St17integral_constantIbLb1EE, funcdef_no=2476, decl_uid=42479, cgraph_uid=1216, symbol_order=1264)

void std::__do_alloc_on_move(_Alloc&, _Alloc&, std::true_type) [with _Alloc = std::allocator<char>] (struct allocator & __one, struct allocator & __two, struct true_type D.42482)
{
  std::move<std::allocator<char>&> (__two);
  return;
}



;; Function static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_copy(_CharT*, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_S_copyEPcPKcm, funcdef_no=2400, decl_uid=18864, cgraph_uid=1140, symbol_order=1188)

static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_copy(_CharT*, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (char * __d, const char * __s, size_type __n)
{
  if (__n == 1) goto <D.47266>; else goto <D.47267>;
  <D.47266>:
  std::char_traits<char>::assign (__d, __s);
  goto <D.47268>;
  <D.47267>:
  std::char_traits<char>::copy (__d, __s, __n);
  <D.47268>:
  return;
}



;; Function static std::char_traits<char>::char_type* std::char_traits<char>::copy(std::char_traits<char>::char_type*, const char_type*, std::size_t) (_ZNSt11char_traitsIcE4copyEPcPKcm, funcdef_no=365, decl_uid=10360, cgraph_uid=54, symbol_order=55)

static std::char_traits<char>::char_type* std::char_traits<char>::copy(std::char_traits<char>::char_type*, const char_type*, std::size_t) (char_type * __s1, const char_type * __s2, size_t __n)
{
  char_type * D.47271;

  if (__n == 0) goto <D.47269>; else goto <D.47270>;
  <D.47269>:
  D.47271 = __s1;
  goto <D.47272>;
  <D.47270>:
  D.47271 = __builtin_memcpy (__s1, __s2, __n);
  goto <D.47272>;
  <D.47272>:
  return D.47271;
}



;; Function bool std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_is_local() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE11_M_is_localEv, funcdef_no=2285, decl_uid=18761, cgraph_uid=1025, symbol_order=1073)

bool std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_is_local() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  const char * D.47276;
  char * D.47275;
  bool D.47273;

  D.47275 = std::__cxx11::basic_string<char>::_M_data (this);
  _1 = D.47275;
  D.47276 = std::__cxx11::basic_string<char>::_M_local_data (this);
  _2 = D.47276;
  D.47273 = _1 == _2;
  goto <D.47274>;
  <D.47274>:
  return D.47273;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::const_pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_M_local_dataEv, funcdef_no=2415, decl_uid=18753, cgraph_uid=1155, symbol_order=1203)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::const_pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  const char * D.47277;

  _1 = &this->D.18739._M_local_buf;
  D.47277 = std::pointer_traits<const char*>::pointer_to (_1);
  goto <D.47278>;
  <D.47278>:
  return D.47277;
}



;; Function static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = const char] (_ZNSt14pointer_traitsIPKcE10pointer_toERS0_, funcdef_no=2487, decl_uid=42578, cgraph_uid=1227, symbol_order=1275)

static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = const char] (const __make_not_void & __r)
{
  const char * D.47279;

  D.47279 = std::addressof<const char> (__r);
  goto <D.47280>;
  <D.47280>:
  return D.47279;
}



;; Function _Tp* std::addressof(_Tp&) [with _Tp = const char] (_ZSt9addressofIKcEPT_RS1_, funcdef_no=2554, decl_uid=43613, cgraph_uid=1295, symbol_order=1343)

_Tp* std::addressof(_Tp&) [with _Tp = const char] (const char & __r)
{
  const char * D.47281;

  D.47281 = std::__addressof<const char> (__r);
  goto <D.47282>;
  <D.47282>:
  return D.47281;
}



;; Function constexpr _Tp* std::__addressof(_Tp&) [with _Tp = const char] (_ZSt11__addressofIKcEPT_RS1_, funcdef_no=2555, decl_uid=45000, cgraph_uid=1294, symbol_order=1342)

constexpr _Tp* std::__addressof(_Tp&) [with _Tp = const char] (const char & __r)
{
  const char * D.47283;

  D.47283 = __r;
  goto <D.47284>;
  <D.47284>:
  return D.47283;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::length() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6lengthEv, funcdef_no=2178, decl_uid=19046, cgraph_uid=918, symbol_order=966)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::length() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  size_type D.47285;

  D.47285 = this->_M_string_length;
  goto <D.47286>;
  <D.47286>:
  return D.47285;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_capacity(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE11_M_capacityEm, funcdef_no=2287, decl_uid=18755, cgraph_uid=1027, symbol_order=1075)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_capacity(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, size_type __capacity)
{
  this->D.18739._M_allocated_capacity = __capacity;
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE7_M_dataEPc, funcdef_no=2286, decl_uid=18743, cgraph_uid=1026, symbol_order=1074)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, char * __p)
{
  this->_M_dataplus._M_p = __p;
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::clear() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE5clearEv, funcdef_no=2403, decl_uid=19064, cgraph_uid=1143, symbol_order=1191)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::clear() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this)
{
  std::__cxx11::basic_string<char>::_M_set_length (this, 0);
  return;
}



;; Function void* memset(void*, int, size_t) (memset, funcdef_no=1931, decl_uid=1032, cgraph_uid=672, symbol_order=674)

__attribute__((nothrow, __artificial__, __gnu_inline__, __always_inline__, __leaf__))
void* memset(void*, int, size_t) (void * __dest, int __ch, size_t __len)
{
  void * D.47287;

  _1 = __builtin_object_size (__dest, 0);
  D.47287 = __builtin___memset_chk (__dest, __ch, __len, _1);
  goto <D.47288>;
  <D.47288>:
  return D.47287;
}



;; Function FI1 memstr(FI1, int, FI2, int) [with FI1 = unsigned char*; FI2 = char*] (_Z6memstrIPhPcET_S2_iT0_i, funcdef_no=2265, decl_uid=39838, cgraph_uid=1005, symbol_order=1053)

FI1 memstr(FI1, int, FI2, int) [with FI1 = unsigned char*; FI2 = char*] (unsigned char * searchBuf, int searchBufSize, char * searchString, int searchStringSize)
{
  unsigned char * D.47295;
  unsigned char * s;
  unsigned char * searchBufEnd;
  unsigned char * iftmp.677;
  unsigned char * D.47289;

  _1 = (sizetype) searchBufSize;
  searchBufEnd = searchBuf + _1;
  _2 = (sizetype) searchStringSize;
  _3 = searchString + _2;
  D.47295 = std::search<unsigned char*, char*> (searchBuf, searchBufEnd, searchString, _3);
  s = D.47295;
  if (s != searchBufEnd) goto <D.47291>; else goto <D.47292>;
  <D.47291>:
  iftmp.677 = s;
  goto <D.47293>;
  <D.47292>:
  iftmp.677 = 0B;
  <D.47293>:
  D.47289 = iftmp.677;
  goto <D.47294>;
  <D.47294>:
  return D.47289;
}



;; Function _FIter1 std::search(_FIter1, _FIter1, _FIter2, _FIter2) [with _FIter1 = unsigned char*; _FIter2 = char*] (_ZSt6searchIPhPcET_S2_S2_T0_S3_, funcdef_no=2404, decl_uid=41830, cgraph_uid=1144, symbol_order=1192)

_FIter1 std::search(_FIter1, _FIter1, _FIter2, _FIter2) [with _FIter1 = unsigned char*; _FIter2 = char*] (unsigned char * __first1, unsigned char * __last1, char * __first2, char * __last2)
{
  unsigned char * D.47299;
  struct _Iter_equal_to_iter D.47297;
  struct _Iter_equal_to_iter D.42488;
  unsigned char * D.47296;

  __gnu_cxx::__ops::__iter_equal_to_iter ();
  D.47299 = std::__search<unsigned char*, char*, __gnu_cxx::__ops::_Iter_equal_to_iter> (__first1, __last1, __first2, __last2, D.47297);
  D.47296 = D.47299;
  goto <D.47301>;
  <D.47301>:
  D.42488 = {CLOBBER};
  goto <D.47298>;
  <D.47298>:
  return D.47296;
  <D.47300>:
  D.42488 = {CLOBBER};
  resx 1
}



;; Function __gnu_cxx::__ops::_Iter_equal_to_iter __gnu_cxx::__ops::__iter_equal_to_iter() (_ZN9__gnu_cxx5__ops20__iter_equal_to_iterEv, funcdef_no=205, decl_uid=8057, cgraph_uid=12, symbol_order=13)

__gnu_cxx::__ops::_Iter_equal_to_iter __gnu_cxx::__ops::__iter_equal_to_iter() ()
{
  struct _Iter_equal_to_iter D.47302;

  goto <D.47303>;
  <D.47303>:
  return D.47302;
}



;; Function _ForwardIterator1 std::__search(_ForwardIterator1, _ForwardIterator1, _ForwardIterator2, _ForwardIterator2, _BinaryPredicate) [with _ForwardIterator1 = unsigned char*; _ForwardIterator2 = char*; _BinaryPredicate = __gnu_cxx::__ops::_Iter_equal_to_iter] (_ZSt8__searchIPhPcN9__gnu_cxx5__ops19_Iter_equal_to_iterEET_S5_S5_T0_S6_T1_, funcdef_no=2477, decl_uid=42489, cgraph_uid=1217, symbol_order=1265)

_ForwardIterator1 std::__search(_ForwardIterator1, _ForwardIterator1, _ForwardIterator2, _ForwardIterator2, _BinaryPredicate) [with _ForwardIterator1 = unsigned char*; _ForwardIterator2 = char*; _BinaryPredicate = __gnu_cxx::__ops::_Iter_equal_to_iter] (unsigned char * __first1, unsigned char * __last1, char * __first2, char * __last2, struct _Iter_equal_to_iter __predicate)
{
  unsigned char * D.47330;
  unsigned char * D.47329;
  unsigned char * __current;
  char * __p;
  char * __p1;
  bool retval.682;
  bool retval.681;
  bool retval.680;
  bool retval.679;
  struct _Iter_equals_iter D.47313;
  struct _Iter_equal_to_iter D.43500;
  struct _Iter_equals_iter D.47311;
  struct _Iter_equal_to_iter D.47310;
  struct _Iter_equal_to_iter D.43443;
  bool retval.678;
  unsigned char * D.47306;

  _1 = __first1 == __last1;
  _2 = __first2 == __last2;
  _3 = _1 | _2;
  if (_3 != 0) goto <D.47304>; else goto <D.47305>;
  <D.47304>:
  D.47306 = __first1;
  goto <D.47328>;
  <D.47305>:
  __p1 = __first2;
  __p1 = __p1 + 1;
  retval.678 = __p1 == __last2;
  if (retval.678 != 0) goto <D.47308>; else goto <D.47309>;
  <D.47308>:
  D.47311 = __gnu_cxx::__ops::__iter_comp_iter<char*> (D.47310, __first2);
  D.47329 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<char*> > (__first1, __last1, D.47311);
  D.47306 = D.47329;
  goto <D.47328>;
  <D.47309>:
  __current = __first1;
  <D.47312>:
  D.47313 = __gnu_cxx::__ops::__iter_comp_iter<char*> (D.47310, __first2);
  D.47330 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<char*> > (__first1, __last1, D.47313);
  __first1 = D.47330;
  if (__first1 == __last1) goto <D.47314>; else goto <D.47315>;
  <D.47314>:
  D.47306 = __last1;
  goto <D.47328>;
  <D.47315>:
  __p = __p1;
  __current = __first1;
  __current = __current + 1;
  retval.679 = __current == __last1;
  if (retval.679 != 0) goto <D.47317>; else goto <D.47318>;
  <D.47317>:
  D.47306 = __last1;
  goto <D.47328>;
  <D.47318>:
  <D.47319>:
  retval.680 = __gnu_cxx::__ops::_Iter_equal_to_iter::operator()<unsigned char*, char*> (&__predicate, __current, __p);
  if (retval.680 != 0) goto <D.47321>; else goto <D.43509>;
  <D.47321>:
  __p = __p + 1;
  retval.681 = __p == __last2;
  if (retval.681 != 0) goto <D.47323>; else goto <D.47324>;
  <D.47323>:
  D.47306 = __first1;
  goto <D.47328>;
  <D.47324>:
  __current = __current + 1;
  retval.682 = __current == __last1;
  if (retval.682 != 0) goto <D.47326>; else goto <D.47327>;
  <D.47326>:
  D.47306 = __last1;
  goto <D.47328>;
  <D.47327>:
  goto <D.47319>;
  <D.43509>:
  __first1 = __first1 + 1;
  goto <D.47312>;
  D.47306 = __first1;
  goto <D.47328>;
  <D.47328>:
  return D.47306;
}



;; Function __gnu_cxx::__ops::_Iter_equals_iter<_Iterator> __gnu_cxx::__ops::__iter_comp_iter(__gnu_cxx::__ops::_Iter_equal_to_iter, _Iterator) [with _Iterator = char*] (_ZN9__gnu_cxx5__ops16__iter_comp_iterIPcEENS0_17_Iter_equals_iterIT_EENS0_19_Iter_equal_to_iterES4_, funcdef_no=2544, decl_uid=43440, cgraph_uid=1284, symbol_order=1332)

__gnu_cxx::__ops::_Iter_equals_iter<_Iterator> __gnu_cxx::__ops::__iter_comp_iter(__gnu_cxx::__ops::_Iter_equal_to_iter, _Iterator) [with _Iterator = char*] (struct _Iter_equal_to_iter D.43441, char * __it)
{
  struct _Iter_equals_iter D.44833;
  struct _Iter_equals_iter D.47331;

  __gnu_cxx::__ops::_Iter_equals_iter<char*>::_Iter_equals_iter (&D.44833, __it);
  D.47331 = D.44833;
  goto <D.47333>;
  <D.47333>:
  D.44833 = {CLOBBER};
  goto <D.47332>;
  <D.47332>:
  return D.47331;
}



;; Function __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::_Iter_equals_iter(_Iterator1) [with _Iterator1 = char*] (_ZN9__gnu_cxx5__ops17_Iter_equals_iterIPcEC2ES2_, funcdef_no=2601, decl_uid=43462, cgraph_uid=1341, symbol_order=1389)

__gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::_Iter_equals_iter(_Iterator1) [with _Iterator1 = char*] (struct _Iter_equals_iter * const this, char * __it1)
{
  MEM[(struct  &)this] = {CLOBBER};
  this->_M_it1 = __it1;
  return;
}



;; Function _Iterator std::__find_if(_Iterator, _Iterator, _Predicate) [with _Iterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<char*>] (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterIPcEEET_S6_S6_T0_, funcdef_no=2545, decl_uid=43476, cgraph_uid=1286, symbol_order=1334)

_Iterator std::__find_if(_Iterator, _Iterator, _Predicate) [with _Iterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<char*>] (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred)
{
  unsigned char * D.47337;
  struct random_access_iterator_tag D.47335;
  struct iterator_category D.44845;
  unsigned char * D.47334;

  __first.683_1 = __first;
  D.47337 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<char*> > (__first.683_1, __last, __pred, D.47335);
  D.47334 = D.47337;
  goto <D.47339>;
  <D.47339>:
  D.44845 = {CLOBBER};
  goto <D.47336>;
  <D.47336>:
  return D.47334;
  <D.47338>:
  D.44845 = {CLOBBER};
  resx 1
}



;; Function _RandomAccessIterator std::__find_if(_RandomAccessIterator, _RandomAccessIterator, _Predicate, std::random_access_iterator_tag) [with _RandomAccessIterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<char*>] (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterIPcEEET_S6_S6_T0_St26random_access_iterator_tag, funcdef_no=2603, decl_uid=44846, cgraph_uid=1343, symbol_order=1391)

_RandomAccessIterator std::__find_if(_RandomAccessIterator, _RandomAccessIterator, _Predicate, std::random_access_iterator_tag) [with _RandomAccessIterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<char*>] (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred, struct random_access_iterator_tag D.44850)
{
  difference_type __trip_count;
  bool retval.694;
  bool retval.693;
  bool retval.692;
  bool retval.689;
  bool retval.688;
  bool retval.687;
  unsigned char * D.47345;
  bool retval.686;

  __last.684_1 = (long int) __last;
  __first.685_2 = (long int) __first;
  _3 = __last.684_1 - __first.685_2;
  __trip_count = _3 >> 2;
  <D.47340>:
  if (__trip_count <= 0) goto <D.45144>; else goto <D.47341>;
  <D.47341>:
  retval.686 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.686 != 0) goto <D.47343>; else goto <D.47344>;
  <D.47343>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47344>:
  __first = __first + 1;
  retval.687 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.687 != 0) goto <D.47347>; else goto <D.47348>;
  <D.47347>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47348>:
  __first = __first + 1;
  retval.688 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.688 != 0) goto <D.47350>; else goto <D.47351>;
  <D.47350>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47351>:
  __first = __first + 1;
  retval.689 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.689 != 0) goto <D.47353>; else goto <D.47354>;
  <D.47353>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47354>:
  __first = __first + 1;
  __trip_count = __trip_count + -1;
  goto <D.47340>;
  <D.45144>:
  __last.690_4 = (long int) __last;
  __first.691_5 = (long int) __first;
  _6 = __last.690_4 - __first.691_5;
  switch (_6) <default: <D.45143>, case 0: <D.45142>, case 1: <D.45141>, case 2: <D.45140>, case 3: <D.45139>>
  <D.45139>:
  retval.692 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.692 != 0) goto <D.47356>; else goto <D.47357>;
  <D.47356>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47357>:
  __first = __first + 1;
  <D.45140>:
  retval.693 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.693 != 0) goto <D.47359>; else goto <D.47360>;
  <D.47359>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47360>:
  __first = __first + 1;
  <D.45141>:
  retval.694 = __gnu_cxx::__ops::_Iter_equals_iter<char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.694 != 0) goto <D.47362>; else goto <D.47363>;
  <D.47362>:
  D.47345 = __first;
  goto <D.47364>;
  <D.47363>:
  __first = __first + 1;
  <D.45142>:
  <D.45143>:
  D.47345 = __last;
  goto <D.47364>;
  <D.47364>:
  return D.47345;
}



;; Function bool __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::operator()(_Iterator2) [with _Iterator2 = unsigned char*; _Iterator1 = char*] (_ZN9__gnu_cxx5__ops17_Iter_equals_iterIPcEclIPhEEbT_, funcdef_no=2637, decl_uid=45136, cgraph_uid=1377, symbol_order=1425)

bool __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::operator()(_Iterator2) [with _Iterator2 = unsigned char*; _Iterator1 = char*] (struct _Iter_equals_iter * const this, unsigned char * __it2)
{
  bool D.47365;

  _1 = *__it2;
  _2 = (int) _1;
  _3 = this->_M_it1;
  _4 = *_3;
  _5 = (int) _4;
  D.47365 = _2 == _5;
  goto <D.47366>;
  <D.47366>:
  return D.47365;
}



;; Function bool __gnu_cxx::__ops::_Iter_equal_to_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = unsigned char*; _Iterator2 = char*] (_ZNK9__gnu_cxx5__ops19_Iter_equal_to_iterclIPhPcEEbT_T0_, funcdef_no=2547, decl_uid=43503, cgraph_uid=1287, symbol_order=1335)

bool __gnu_cxx::__ops::_Iter_equal_to_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = unsigned char*; _Iterator2 = char*] (const struct _Iter_equal_to_iter * const this, unsigned char * __it1, char * __it2)
{
  bool D.47367;

  _1 = *__it1;
  _2 = (int) _1;
  _3 = *__it2;
  _4 = (int) _3;
  D.47367 = _2 == _4;
  goto <D.47368>;
  <D.47368>:
  return D.47367;
}



;; Function FI1 memstr(FI1, int, FI2, int) [with FI1 = unsigned char*; FI2 = unsigned char*] (_Z6memstrIPhS0_ET_S1_iT0_i, funcdef_no=2266, decl_uid=39843, cgraph_uid=1006, symbol_order=1054)

FI1 memstr(FI1, int, FI2, int) [with FI1 = unsigned char*; FI2 = unsigned char*] (unsigned char * searchBuf, int searchBufSize, unsigned char * searchString, int searchStringSize)
{
  unsigned char * D.47375;
  unsigned char * s;
  unsigned char * searchBufEnd;
  unsigned char * iftmp.695;
  unsigned char * D.47369;

  _1 = (sizetype) searchBufSize;
  searchBufEnd = searchBuf + _1;
  _2 = (sizetype) searchStringSize;
  _3 = searchString + _2;
  D.47375 = std::search<unsigned char*, unsigned char*> (searchBuf, searchBufEnd, searchString, _3);
  s = D.47375;
  if (s != searchBufEnd) goto <D.47371>; else goto <D.47372>;
  <D.47371>:
  iftmp.695 = s;
  goto <D.47373>;
  <D.47372>:
  iftmp.695 = 0B;
  <D.47373>:
  D.47369 = iftmp.695;
  goto <D.47374>;
  <D.47374>:
  return D.47369;
}



;; Function _FIter1 std::search(_FIter1, _FIter1, _FIter2, _FIter2) [with _FIter1 = unsigned char*; _FIter2 = unsigned char*] (_ZSt6searchIPhS0_ET_S1_S1_T0_S2_, funcdef_no=2405, decl_uid=41838, cgraph_uid=1145, symbol_order=1193)

_FIter1 std::search(_FIter1, _FIter1, _FIter2, _FIter2) [with _FIter1 = unsigned char*; _FIter2 = unsigned char*] (unsigned char * __first1, unsigned char * __last1, unsigned char * __first2, unsigned char * __last2)
{
  unsigned char * D.47379;
  struct _Iter_equal_to_iter D.47377;
  struct _Iter_equal_to_iter D.42496;
  unsigned char * D.47376;

  __gnu_cxx::__ops::__iter_equal_to_iter ();
  D.47379 = std::__search<unsigned char*, unsigned char*, __gnu_cxx::__ops::_Iter_equal_to_iter> (__first1, __last1, __first2, __last2, D.47377);
  D.47376 = D.47379;
  goto <D.47381>;
  <D.47381>:
  D.42496 = {CLOBBER};
  goto <D.47378>;
  <D.47378>:
  return D.47376;
  <D.47380>:
  D.42496 = {CLOBBER};
  resx 1
}



;; Function _ForwardIterator1 std::__search(_ForwardIterator1, _ForwardIterator1, _ForwardIterator2, _ForwardIterator2, _BinaryPredicate) [with _ForwardIterator1 = unsigned char*; _ForwardIterator2 = unsigned char*; _BinaryPredicate = __gnu_cxx::__ops::_Iter_equal_to_iter] (_ZSt8__searchIPhS0_N9__gnu_cxx5__ops19_Iter_equal_to_iterEET_S4_S4_T0_S5_T1_, funcdef_no=2478, decl_uid=42497, cgraph_uid=1218, symbol_order=1266)

_ForwardIterator1 std::__search(_ForwardIterator1, _ForwardIterator1, _ForwardIterator2, _ForwardIterator2, _BinaryPredicate) [with _ForwardIterator1 = unsigned char*; _ForwardIterator2 = unsigned char*; _BinaryPredicate = __gnu_cxx::__ops::_Iter_equal_to_iter] (unsigned char * __first1, unsigned char * __last1, unsigned char * __first2, unsigned char * __last2, struct _Iter_equal_to_iter __predicate)
{
  unsigned char * D.47408;
  unsigned char * D.47407;
  unsigned char * __current;
  unsigned char * __p;
  unsigned char * __p1;
  bool retval.700;
  bool retval.699;
  bool retval.698;
  bool retval.697;
  struct _Iter_equals_iter D.47391;
  struct _Iter_equal_to_iter D.43574;
  struct _Iter_equals_iter D.47389;
  struct _Iter_equal_to_iter D.47388;
  struct _Iter_equal_to_iter D.43517;
  bool retval.696;
  unsigned char * D.47384;

  _1 = __first1 == __last1;
  _2 = __first2 == __last2;
  _3 = _1 | _2;
  if (_3 != 0) goto <D.47382>; else goto <D.47383>;
  <D.47382>:
  D.47384 = __first1;
  goto <D.47406>;
  <D.47383>:
  __p1 = __first2;
  __p1 = __p1 + 1;
  retval.696 = __p1 == __last2;
  if (retval.696 != 0) goto <D.47386>; else goto <D.47387>;
  <D.47386>:
  D.47389 = __gnu_cxx::__ops::__iter_comp_iter<unsigned char*> (D.47388, __first2);
  D.47407 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*> > (__first1, __last1, D.47389);
  D.47384 = D.47407;
  goto <D.47406>;
  <D.47387>:
  __current = __first1;
  <D.47390>:
  D.47391 = __gnu_cxx::__ops::__iter_comp_iter<unsigned char*> (D.47388, __first2);
  D.47408 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*> > (__first1, __last1, D.47391);
  __first1 = D.47408;
  if (__first1 == __last1) goto <D.47392>; else goto <D.47393>;
  <D.47392>:
  D.47384 = __last1;
  goto <D.47406>;
  <D.47393>:
  __p = __p1;
  __current = __first1;
  __current = __current + 1;
  retval.697 = __current == __last1;
  if (retval.697 != 0) goto <D.47395>; else goto <D.47396>;
  <D.47395>:
  D.47384 = __last1;
  goto <D.47406>;
  <D.47396>:
  <D.47397>:
  retval.698 = __gnu_cxx::__ops::_Iter_equal_to_iter::operator()<unsigned char*, unsigned char*> (&__predicate, __current, __p);
  if (retval.698 != 0) goto <D.47399>; else goto <D.43583>;
  <D.47399>:
  __p = __p + 1;
  retval.699 = __p == __last2;
  if (retval.699 != 0) goto <D.47401>; else goto <D.47402>;
  <D.47401>:
  D.47384 = __first1;
  goto <D.47406>;
  <D.47402>:
  __current = __current + 1;
  retval.700 = __current == __last1;
  if (retval.700 != 0) goto <D.47404>; else goto <D.47405>;
  <D.47404>:
  D.47384 = __last1;
  goto <D.47406>;
  <D.47405>:
  goto <D.47397>;
  <D.43583>:
  __first1 = __first1 + 1;
  goto <D.47390>;
  D.47384 = __first1;
  goto <D.47406>;
  <D.47406>:
  return D.47384;
}



;; Function __gnu_cxx::__ops::_Iter_equals_iter<_Iterator> __gnu_cxx::__ops::__iter_comp_iter(__gnu_cxx::__ops::_Iter_equal_to_iter, _Iterator) [with _Iterator = unsigned char*] (_ZN9__gnu_cxx5__ops16__iter_comp_iterIPhEENS0_17_Iter_equals_iterIT_EENS0_19_Iter_equal_to_iterES4_, funcdef_no=2548, decl_uid=43514, cgraph_uid=1288, symbol_order=1336)

__gnu_cxx::__ops::_Iter_equals_iter<_Iterator> __gnu_cxx::__ops::__iter_comp_iter(__gnu_cxx::__ops::_Iter_equal_to_iter, _Iterator) [with _Iterator = unsigned char*] (struct _Iter_equal_to_iter D.43515, unsigned char * __it)
{
  struct _Iter_equals_iter D.44863;
  struct _Iter_equals_iter D.47409;

  __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::_Iter_equals_iter (&D.44863, __it);
  D.47409 = D.44863;
  goto <D.47411>;
  <D.47411>:
  D.44863 = {CLOBBER};
  goto <D.47410>;
  <D.47410>:
  return D.47409;
}



;; Function __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::_Iter_equals_iter(_Iterator1) [with _Iterator1 = unsigned char*] (_ZN9__gnu_cxx5__ops17_Iter_equals_iterIPhEC2ES2_, funcdef_no=2605, decl_uid=43536, cgraph_uid=1345, symbol_order=1393)

__gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::_Iter_equals_iter(_Iterator1) [with _Iterator1 = unsigned char*] (struct _Iter_equals_iter * const this, unsigned char * __it1)
{
  MEM[(struct  &)this] = {CLOBBER};
  this->_M_it1 = __it1;
  return;
}



;; Function _Iterator std::__find_if(_Iterator, _Iterator, _Predicate) [with _Iterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>] (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterIS0_EEET_S5_S5_T0_, funcdef_no=2549, decl_uid=43550, cgraph_uid=1289, symbol_order=1337)

_Iterator std::__find_if(_Iterator, _Iterator, _Predicate) [with _Iterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>] (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred)
{
  unsigned char * D.47415;
  struct random_access_iterator_tag D.47413;
  struct iterator_category D.44865;
  unsigned char * D.47412;

  __first.701_1 = __first;
  D.47415 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*> > (__first.701_1, __last, __pred, D.47413);
  D.47412 = D.47415;
  goto <D.47417>;
  <D.47417>:
  D.44865 = {CLOBBER};
  goto <D.47414>;
  <D.47414>:
  return D.47412;
  <D.47416>:
  D.44865 = {CLOBBER};
  resx 1
}



;; Function _RandomAccessIterator std::__find_if(_RandomAccessIterator, _RandomAccessIterator, _Predicate, std::random_access_iterator_tag) [with _RandomAccessIterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>] (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterIS0_EEET_S5_S5_T0_St26random_access_iterator_tag, funcdef_no=2607, decl_uid=44866, cgraph_uid=1347, symbol_order=1395)

_RandomAccessIterator std::__find_if(_RandomAccessIterator, _RandomAccessIterator, _Predicate, std::random_access_iterator_tag) [with _RandomAccessIterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>] (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred, struct random_access_iterator_tag D.44870)
{
  difference_type __trip_count;
  bool retval.712;
  bool retval.711;
  bool retval.710;
  bool retval.707;
  bool retval.706;
  bool retval.705;
  unsigned char * D.47423;
  bool retval.704;

  __last.702_1 = (long int) __last;
  __first.703_2 = (long int) __first;
  _3 = __last.702_1 - __first.703_2;
  __trip_count = _3 >> 2;
  <D.47418>:
  if (__trip_count <= 0) goto <D.45160>; else goto <D.47419>;
  <D.47419>:
  retval.704 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.704 != 0) goto <D.47421>; else goto <D.47422>;
  <D.47421>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47422>:
  __first = __first + 1;
  retval.705 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.705 != 0) goto <D.47425>; else goto <D.47426>;
  <D.47425>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47426>:
  __first = __first + 1;
  retval.706 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.706 != 0) goto <D.47428>; else goto <D.47429>;
  <D.47428>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47429>:
  __first = __first + 1;
  retval.707 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.707 != 0) goto <D.47431>; else goto <D.47432>;
  <D.47431>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47432>:
  __first = __first + 1;
  __trip_count = __trip_count + -1;
  goto <D.47418>;
  <D.45160>:
  __last.708_4 = (long int) __last;
  __first.709_5 = (long int) __first;
  _6 = __last.708_4 - __first.709_5;
  switch (_6) <default: <D.45159>, case 0: <D.45158>, case 1: <D.45157>, case 2: <D.45156>, case 3: <D.45155>>
  <D.45155>:
  retval.710 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.710 != 0) goto <D.47434>; else goto <D.47435>;
  <D.47434>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47435>:
  __first = __first + 1;
  <D.45156>:
  retval.711 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.711 != 0) goto <D.47437>; else goto <D.47438>;
  <D.47437>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47438>:
  __first = __first + 1;
  <D.45157>:
  retval.712 = __gnu_cxx::__ops::_Iter_equals_iter<unsigned char*>::operator()<unsigned char*> (&__pred, __first);
  if (retval.712 != 0) goto <D.47440>; else goto <D.47441>;
  <D.47440>:
  D.47423 = __first;
  goto <D.47442>;
  <D.47441>:
  __first = __first + 1;
  <D.45158>:
  <D.45159>:
  D.47423 = __last;
  goto <D.47442>;
  <D.47442>:
  return D.47423;
}



;; Function bool __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::operator()(_Iterator2) [with _Iterator2 = unsigned char*; _Iterator1 = unsigned char*] (_ZN9__gnu_cxx5__ops17_Iter_equals_iterIPhEclIS2_EEbT_, funcdef_no=2638, decl_uid=45152, cgraph_uid=1378, symbol_order=1426)

bool __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::operator()(_Iterator2) [with _Iterator2 = unsigned char*; _Iterator1 = unsigned char*] (struct _Iter_equals_iter * const this, unsigned char * __it2)
{
  bool D.47443;

  _1 = *__it2;
  _2 = this->_M_it1;
  _3 = *_2;
  D.47443 = _1 == _3;
  goto <D.47444>;
  <D.47444>:
  return D.47443;
}



;; Function bool __gnu_cxx::__ops::_Iter_equal_to_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = unsigned char*; _Iterator2 = unsigned char*] (_ZNK9__gnu_cxx5__ops19_Iter_equal_to_iterclIPhS3_EEbT_T0_, funcdef_no=2550, decl_uid=43577, cgraph_uid=1290, symbol_order=1338)

bool __gnu_cxx::__ops::_Iter_equal_to_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = unsigned char*; _Iterator2 = unsigned char*] (const struct _Iter_equal_to_iter * const this, unsigned char * __it1, unsigned char * __it2)
{
  bool D.47445;

  _1 = *__it1;
  _2 = *__it2;
  D.47445 = _1 == _2;
  goto <D.47446>;
  <D.47446>:
  return D.47445;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE4sizeEv, funcdef_no=2268, decl_uid=19044, cgraph_uid=1008, symbol_order=1056)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  size_type D.47447;

  D.47447 = this->_M_string_length;
  goto <D.47448>;
  <D.47448>:
  return D.47447;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::resize(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6resizeEm, funcdef_no=2269, decl_uid=19054, cgraph_uid=1009, symbol_order=1057)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::resize(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, size_type __n)
{
  std::__cxx11::basic_string<char>::resize (this, __n, 0);
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator+=(const _CharT*) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEpLEPKc, funcdef_no=2270, decl_uid=19091, cgraph_uid=1010, symbol_order=1058)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator+=(const _CharT*) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const char * __s)
{
  struct basic_string & D.47451;
  struct basic_string & D.47449;

  D.47451 = std::__cxx11::basic_string<char>::append (this, __s);
  D.47449 = D.47451;
  goto <D.47450>;
  <D.47450>:
  return D.47449;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::append(const _CharT*) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendEPKc, funcdef_no=2407, decl_uid=19112, cgraph_uid=1147, symbol_order=1195)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::append(const _CharT*) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const char * __s)
{
  struct basic_string & D.47454;
  const size_type __n;
  struct basic_string & D.47452;

  __n = std::char_traits<char>::length (__s);
  std::__cxx11::basic_string<char>::_M_check_length (this, 0, __n, "basic_string::append");
  D.47454 = std::__cxx11::basic_string<char>::_M_append (this, __s, __n);
  D.47452 = D.47454;
  goto <D.47453>;
  <D.47453>:
  return D.47452;
}



;; Function static std::size_t std::char_traits<char>::length(const char_type*) (_ZNSt11char_traitsIcE6lengthEPKc, funcdef_no=362, decl_uid=10348, cgraph_uid=51, symbol_order=52)

static std::size_t std::char_traits<char>::length(const char_type*) (const char_type * __s)
{
  size_t D.47455;

  D.47455 = __builtin_strlen (__s);
  goto <D.47456>;
  <D.47456>:
  return D.47455;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_check_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const char*) const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE15_M_check_lengthEmmPKc, funcdef_no=2479, decl_uid=18852, cgraph_uid=1219, symbol_order=1267)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_check_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const char*) const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this, size_type __n1, size_type __n2, const char * __s)
{
  bool retval.713;

  _1 = std::__cxx11::basic_string<char>::max_size (this);
  _2 = std::__cxx11::basic_string<char>::size (this);
  _3 = __n1 - _2;
  _4 = _1 + _3;
  retval.713 = __n2 > _4;
  if (retval.713 != 0) goto <D.47458>; else goto <D.47459>;
  <D.47458>:
  __s.714_5 = __s;
  std::__throw_length_error (__s.714_5);
  <D.47459>:
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::max_size() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE8max_sizeEv, funcdef_no=2551, decl_uid=19048, cgraph_uid=1291, symbol_order=1339)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::max_size() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  const struct allocator_type & D.47462;
  size_type D.47460;

  D.47462 = std::__cxx11::basic_string<char>::_M_get_allocator (this);
  _1 = D.47462;
  _2 = std::allocator_traits<std::allocator<char> >::max_size (_1);
  _3 = _2 + 18446744073709551615;
  D.47460 = _3 / 2;
  goto <D.47461>;
  <D.47461>:
  return D.47460;
}



;; Function const allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE16_M_get_allocatorEv, funcdef_no=2609, decl_uid=18846, cgraph_uid=1349, symbol_order=1397)

const allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  const struct allocator_type & D.47463;

  D.47463 = &this->_M_dataplus;
  goto <D.47464>;
  <D.47464>:
  return D.47463;
}



;; Function static std::allocator_traits<std::allocator<_CharT> >::size_type std::allocator_traits<std::allocator<_CharT> >::max_size(const allocator_type&) [with _Tp = char] (_ZNSt16allocator_traitsISaIcEE8max_sizeERKS0_, funcdef_no=2608, decl_uid=18600, cgraph_uid=1348, symbol_order=1396)

static std::allocator_traits<std::allocator<_CharT> >::size_type std::allocator_traits<std::allocator<_CharT> >::max_size(const allocator_type&) [with _Tp = char] (const struct allocator_type & __a)
{
  size_type D.47465;

  D.47465 = __gnu_cxx::new_allocator<char>::max_size (__a);
  goto <D.47466>;
  <D.47466>:
  return D.47465;
}



;; Function __gnu_cxx::new_allocator<_Tp>::size_type __gnu_cxx::new_allocator<_Tp>::max_size() const [with _Tp = char] (_ZNK9__gnu_cxx13new_allocatorIcE8max_sizeEv, funcdef_no=2639, decl_uid=11627, cgraph_uid=1379, symbol_order=1427)

__gnu_cxx::new_allocator<_Tp>::size_type __gnu_cxx::new_allocator<_Tp>::max_size() const [with _Tp = char] (const struct new_allocator * const this)
{
  size_type D.47467;

  D.47467 = 18446744073709551615;
  goto <D.47468>;
  <D.47468>:
  return D.47467;
}



;; Function std::allocator< <template-parameter-1-1> >::allocator() [with _Tp = char] (_ZNSaIcEC2Ev, funcdef_no=2186, decl_uid=11734, cgraph_uid=926, symbol_order=974)

std::allocator< <template-parameter-1-1> >::allocator() [with _Tp = char] (struct allocator * const this)
{
  __gnu_cxx::new_allocator<char>::new_allocator (this);
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::new_allocator() [with _Tp = char] (_ZN9__gnu_cxx13new_allocatorIcEC2Ev, funcdef_no=2309, decl_uid=11669, cgraph_uid=1049, symbol_order=1097)

__gnu_cxx::new_allocator<_Tp>::new_allocator() [with _Tp = char] (struct new_allocator * const this)
{
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::basic_string(const _CharT*, const _Alloc&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEC2EPKcRKS3_, funcdef_no=2262, decl_uid=19601, cgraph_uid=1002, symbol_order=1050)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::basic_string(const _CharT*, const _Alloc&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const char * __s, const struct allocator & __a)
{
  char * D.47473;
  const char * iftmp.715;

  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->_M_dataplus;
  D.47473 = std::__cxx11::basic_string<char>::_M_local_data (this);
  _2 = D.47473;
  std::__cxx11::basic_string<char>::_Alloc_hider::_Alloc_hider (_1, _2, __a);
  if (__s != 0B) goto <D.47470>; else goto <D.47471>;
  <D.47470>:
  _3 = std::char_traits<char>::length (__s);
  iftmp.715 = __s + _3;
  goto <D.47472>;
  <D.47471>:
  iftmp.715 = __s + 18446744073709551615;
  <D.47472>:
  std::__cxx11::basic_string<char>::_M_construct<const char*> (this, __s, iftmp.715);
  return;
  <D.47474>:
  _4 = &this->_M_dataplus;
  std::__cxx11::basic_string<char>::_Alloc_hider::~_Alloc_hider (_4);
  resx 1
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_Alloc_hider::_Alloc_hider(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer, const _Alloc&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderC2EPcRKS3_, funcdef_no=2315, decl_uid=18730, cgraph_uid=1055, symbol_order=1103)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_Alloc_hider::_Alloc_hider(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer, const _Alloc&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct _Alloc_hider * const this, char * __dat, const struct allocator & __a)
{
  MEM[(struct  &)this] = {CLOBBER};
  std::allocator<char>::allocator (this, __a);
  this->_M_p = __dat;
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_construct(_InIterator, _InIterator) [with _InIterator = const char*; _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_, funcdef_no=2317, decl_uid=41494, cgraph_uid=1057, symbol_order=1105)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_construct(_InIterator, _InIterator) [with _InIterator = const char*; _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const char * __beg, const char * __end)
{
  struct __false_type D.47475;
  struct _Integral D.42013;

  std::__cxx11::basic_string<char>::_M_construct_aux<const char*> (this, __beg, __end, D.47475);
  D.42013 = {CLOBBER};
  return;
  <D.47476>:
  D.42013 = {CLOBBER};
  resx 1
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_construct_aux(_InIterator, _InIterator, std::__false_type) [with _InIterator = const char*; _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE16_M_construct_auxIPKcEEvT_S8_St12__false_type, funcdef_no=2423, decl_uid=42047, cgraph_uid=1163, symbol_order=1211)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_construct_aux(_InIterator, _InIterator, std::__false_type) [with _InIterator = const char*; _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const char * __beg, const char * __end, struct __false_type D.42051)
{
  struct forward_iterator_tag D.47477;
  struct _Tag D.42747;
  struct forward_iterator_tag D.42767;

  _1 = &D.42747;
  std::__cxx11::basic_string<char>::_M_construct<const char*> (this, __beg, __end, D.47477);
  D.42747 = {CLOBBER};
  return;
  <D.47478>:
  D.42747 = {CLOBBER};
  resx 1
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_construct(_InIterator, _InIterator, std::forward_iterator_tag) [with _FwdIterator = const char*; _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_M_constructIPKcEEvT_S8_St20forward_iterator_tag, funcdef_no=2496, decl_uid=42757, cgraph_uid=1236, symbol_order=1284)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_construct(_InIterator, _InIterator, std::forward_iterator_tag) [with _FwdIterator = const char*; _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const char * __beg, const char * __end, struct forward_iterator_tag D.42761)
{
  char * D.47492;
  char * D.47491;
  long int D.47490;
  size_type __dnew;
  bool iftmp.717;
  bool retval.716;

  _1 = __gnu_cxx::__is_null_pointer<const char> (__beg);
  if (_1 != 0) goto <D.47483>; else goto <D.47481>;
  <D.47483>:
  if (__beg != __end) goto <D.47484>; else goto <D.47481>;
  <D.47484>:
  iftmp.717 = 1;
  goto <D.47482>;
  <D.47481>:
  iftmp.717 = 0;
  <D.47482>:
  retval.716 = iftmp.717;
  if (retval.716 != 0) goto <D.47485>; else goto <D.47486>;
  <D.47485>:
  std::__throw_logic_error ("basic_string::_M_construct null not valid");
  <D.47486>:
  D.47490 = std::distance<const char*> (__beg, __end);
  _2 = D.47490;
  _3 = (long unsigned int) _2;
  __dnew = _3;
  __dnew.718_4 = __dnew;
  if (__dnew.718_4 > 15) goto <D.47487>; else goto <D.47488>;
  <D.47487>:
  D.47491 = std::__cxx11::basic_string<char>::_M_create (this, &__dnew, 0);
  _5 = D.47491;
  std::__cxx11::basic_string<char>::_M_data (this, _5);
  __dnew.719_6 = __dnew;
  std::__cxx11::basic_string<char>::_M_capacity (this, __dnew.719_6);
  goto <D.47489>;
  <D.47488>:
  <D.47489>:
  D.47492 = std::__cxx11::basic_string<char>::_M_data (this);
  _7 = D.47492;
  std::__cxx11::basic_string<char>::_S_copy_chars (_7, __beg, __end);
  __dnew.720_9 = __dnew;
  std::__cxx11::basic_string<char>::_M_set_length (this, __dnew.720_9);
  __dnew = {CLOBBER};
  return;
  <D.47495>:
  eh_dispatch 2
  resx 2
  <D.47494>:
  _8 = __builtin_eh_pointer (2);
  __cxa_begin_catch (_8);
  std::__cxx11::basic_string<char>::_M_dispose (this);
  __cxa_rethrow ();
  <D.47493>:
  __cxa_end_catch ();
  resx 3
  <D.47496>:
  __dnew = {CLOBBER};
  resx 1
}



;; Function bool __gnu_cxx::__is_null_pointer(_Type*) [with _Type = const char] (_ZN9__gnu_cxx17__is_null_pointerIKcEEbPT_, funcdef_no=2560, decl_uid=43644, cgraph_uid=1300, symbol_order=1348)

bool __gnu_cxx::__is_null_pointer(_Type*) [with _Type = const char] (const char * __ptr)
{
  bool D.47497;

  D.47497 = __ptr == 0B;
  goto <D.47498>;
  <D.47498>:
  return D.47497;
}



;; Function typename std::iterator_traits<_Iterator>::difference_type std::distance(_InputIterator, _InputIterator) [with _InputIterator = const char*] (_ZSt8distanceIPKcENSt15iterator_traitsIT_E15difference_typeES3_S3_, funcdef_no=2561, decl_uid=43647, cgraph_uid=1303, symbol_order=1351)

typename std::iterator_traits<_Iterator>::difference_type std::distance(_InputIterator, _InputIterator) [with _InputIterator = const char*] (const char * __first, const char * __last)
{
  struct random_access_iterator_tag D.47500;
  struct iterator_category D.45025;
  difference_type D.47499;

  __first.721_1 = __first;
  D.47499 = std::__distance<const char*> (__first.721_1, __last, D.47500);
  goto <D.47502>;
  <D.47502>:
  D.45025 = {CLOBBER};
  goto <D.47501>;
  <D.47501>:
  return D.47499;
}



;; Function constexpr typename std::iterator_traits<_Iterator>::difference_type std::__distance(_RandomAccessIterator, _RandomAccessIterator, std::random_access_iterator_tag) [with _RandomAccessIterator = const char*] (_ZSt10__distanceIPKcENSt15iterator_traitsIT_E15difference_typeES3_S3_St26random_access_iterator_tag, funcdef_no=2563, decl_uid=45026, cgraph_uid=1302, symbol_order=1350)

constexpr typename std::iterator_traits<_Iterator>::difference_type std::__distance(_RandomAccessIterator, _RandomAccessIterator, std::random_access_iterator_tag) [with _RandomAccessIterator = const char*] (const char * __first, const char * __last, struct random_access_iterator_tag D.45029)
{
  difference_type D.47503;

  __last.722_1 = (long int) __last;
  __first.723_2 = (long int) __first;
  D.47503 = __last.722_1 - __first.723_2;
  goto <D.47504>;
  <D.47504>:
  return D.47503;
}



;; Function static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_copy_chars(_CharT*, const _CharT*, const _CharT*) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE13_S_copy_charsEPcPKcS7_, funcdef_no=2564, decl_uid=18900, cgraph_uid=1304, symbol_order=1352)

static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_copy_chars(_CharT*, const _CharT*, const _CharT*) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (char * __p, const char * __k1, const char * __k2)
{
  __k2.724_1 = (long int) __k2;
  __k1.725_2 = (long int) __k1;
  _3 = __k2.724_1 - __k1.725_2;
  _4 = (long unsigned int) _3;
  std::__cxx11::basic_string<char>::_S_copy (__p, __k1, _4);
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_dispose() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE10_M_disposeEv, funcdef_no=2278, decl_uid=18767, cgraph_uid=1018, symbol_order=1066)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_dispose() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this)
{
  bool D.47509;
  bool retval.726;

  D.47509 = std::__cxx11::basic_string<char>::_M_is_local (this);
  _1 = D.47509;
  retval.726 = ~_1;
  if (retval.726 != 0) goto <D.47506>; else goto <D.47507>;
  <D.47506>:
  _2 = this->D.18739._M_allocated_capacity;
  std::__cxx11::basic_string<char>::_M_destroy (this, _2);
  goto <D.47508>;
  <D.47507>:
  <D.47508>:
  return;
}



;; Function std::__cxx11::basic_string<char>::_Alloc_hider::~_Alloc_hider() (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderD2Ev, funcdef_no=2085, decl_uid=19763, cgraph_uid=824, symbol_order=872)

std::__cxx11::basic_string<char>::_Alloc_hider::~_Alloc_hider() (struct _Alloc_hider * const this)
{
  std::allocator<char>::~allocator (this);
  MEM[(struct  &)this] = {CLOBBER};
  <D.40891>:
  return;
}



;; Function std::allocator< <template-parameter-1-1> >::~allocator() [with _Tp = char] (_ZNSaIcED2Ev, funcdef_no=2189, decl_uid=11739, cgraph_uid=929, symbol_order=977)

std::allocator< <template-parameter-1-1> >::~allocator() [with _Tp = char] (struct allocator * const this)
{
  __gnu_cxx::new_allocator<char>::~new_allocator (this);
  <D.41490>:
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::~new_allocator() [with _Tp = char] (_ZN9__gnu_cxx13new_allocatorIcED2Ev, funcdef_no=2312, decl_uid=11674, cgraph_uid=1052, symbol_order=1100)

__gnu_cxx::new_allocator<_Tp>::~new_allocator() [with _Tp = char] (struct new_allocator * const this)
{
  <D.41975>:
  return;
}



;; Function bool ends_with_icase(const std::__cxx11::basic_string<_CharT>&, const std::__cxx11::basic_string<_CharT>&) [with charT = char] (_Z15ends_with_icaseIcEbRKNSt7__cxx1112basic_stringIT_St11char_traitsIS2_ESaIS2_EEES8_, funcdef_no=2267, decl_uid=39854, cgraph_uid=1007, symbol_order=1055)

bool ends_with_icase(const std::__cxx11::basic_string<_CharT>&, const std::__cxx11::basic_string<_CharT>&) [with charT = char] (const struct basic_string & str, const struct basic_string & suffix)
{
  size_t si;
  int ti;
  bool retval.728;
  bool retval.727;
  bool D.47512;

  _1 = std::__cxx11::basic_string<char>::size (str);
  _2 = (unsigned int) _1;
  _3 = std::__cxx11::basic_string<char>::size (suffix);
  _4 = (unsigned int) _3;
  _5 = _2 - _4;
  ti = (int) _5;
  if (ti < 0) goto <D.47510>; else goto <D.47511>;
  <D.47510>:
  D.47512 = 0;
  goto <D.47519>;
  <D.47511>:
  si = 0;
  <D.47513>:
  _6 = std::__cxx11::basic_string<char>::size (suffix);
  retval.727 = si < _6;
  if (retval.727 != 0) goto <D.47515>; else goto <D.41846>;
  <D.47515>:
  _7 = (long unsigned int) ti;
  _8 = std::__cxx11::basic_string<char>::operator[] (str, _7);
  _9 = *_8;
  _10 = (int) _9;
  _11 = tolower (_10);
  _12 = std::__cxx11::basic_string<char>::operator[] (suffix, si);
  _13 = *_12;
  _14 = (int) _13;
  _15 = tolower (_14);
  retval.728 = _11 != _15;
  if (retval.728 != 0) goto <D.47517>; else goto <D.47518>;
  <D.47517>:
  D.47512 = 0;
  goto <D.47519>;
  <D.47518>:
  si = si + 1;
  ti = ti + 1;
  goto <D.47513>;
  <D.41846>:
  D.47512 = 1;
  goto <D.47519>;
  <D.47519>:
  return D.47512;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::const_reference std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator[](std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEixEm, funcdef_no=2406, decl_uid=19068, cgraph_uid=1146, symbol_order=1194)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::const_reference std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator[](std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this, size_type __pos)
{
  char * D.47522;
  const value_type & D.47520;

  D.47522 = std::__cxx11::basic_string<char>::_M_data (this);
  _1 = D.47522;
  D.47520 = _1 + __pos;
  goto <D.47521>;
  <D.47521>:
  return D.47520;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator+=(const std::__cxx11::basic_string<_CharT, _Traits, _Alloc>&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEpLERKS4_, funcdef_no=2271, decl_uid=19088, cgraph_uid=1011, symbol_order=1059)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator+=(const std::__cxx11::basic_string<_CharT, _Traits, _Alloc>&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const struct basic_string & __str)
{
  struct basic_string & D.47525;
  struct basic_string & D.47523;

  D.47525 = std::__cxx11::basic_string<char>::append (this, __str);
  D.47523 = D.47525;
  goto <D.47524>;
  <D.47524>:
  return D.47523;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::append(const std::__cxx11::basic_string<_CharT, _Traits, _Alloc>&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE6appendERKS4_, funcdef_no=2408, decl_uid=19100, cgraph_uid=1148, symbol_order=1196)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::append(const std::__cxx11::basic_string<_CharT, _Traits, _Alloc>&) [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this, const struct basic_string & __str)
{
  struct basic_string & D.47529;
  char * D.47528;
  struct basic_string & D.47526;

  _1 = std::__cxx11::basic_string<char>::size (__str);
  D.47528 = std::__cxx11::basic_string<char>::_M_data (__str);
  _2 = D.47528;
  D.47529 = std::__cxx11::basic_string<char>::_M_append (this, _2, _1);
  D.47526 = D.47529;
  goto <D.47527>;
  <D.47527>:
  return D.47526;
}



;; Function int fstat(int, stat*) (*fstat64, funcdef_no=1900, decl_uid=35996, cgraph_uid=641, symbol_order=643)

__attribute__((__leaf__, __gnu_inline__))
int fstat(int, stat*) (int __fd, struct stat * __statbuf)
{
  int D.47530;

  D.47530 = __fxstat (1, __fd, __statbuf);
  goto <D.47531>;
  <D.47531>:
  return D.47530;
}



;; Function int do_close(int) (_Z8do_closei, funcdef_no=1949, decl_uid=39223, cgraph_uid=690, symbol_order=738)

int do_close(int) (int fd)
{
  struct ErrorContext * D.47538;
  int D.47537;
  int ret;
  int D.47535;

  D.47537 = close (fd);
  ret = D.47537;
  if (ret == -1) goto <D.47532>; else goto <D.47533>;
  <D.47532>:
  D.47538 = pbzip2::ErrorContext::getInstance ();
  _1 = D.47538;
  pbzip2::ErrorContext::saveError (_1);
  goto <D.47534>;
  <D.47533>:
  <D.47534>:
  D.47535 = ret;
  goto <D.47536>;
  <D.47536>:
  return D.47535;
}



;; Function const _CharT* std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::c_str() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNKSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE5c_strEv, funcdef_no=2009, decl_uid=19401, cgraph_uid=749, symbol_order=797)

const _CharT* std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::c_str() const [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (const struct basic_string * const this)
{
  const char * D.47541;
  const char * D.47539;

  D.47541 = std::__cxx11::basic_string<char>::_M_data (this);
  D.47539 = D.47541;
  goto <D.47540>;
  <D.47540>:
  return D.47539;
}



;; Function void std::vector<_Tp, _Alloc>::clear() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EE5clearEv, funcdef_no=2272, decl_uid=38165, cgraph_uid=1012, symbol_order=1060)

void std::vector<_Tp, _Alloc>::clear() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this)
{
  _1 = this->D.38528._M_impl._M_start;
  std::vector<outBuff>::_M_erase_at_end (this, _1);
  return;
}



;; Function void syncSetFinishedFlag(int) (_Z19syncSetFinishedFlagi, funcdef_no=1959, decl_uid=38970, cgraph_uid=700, symbol_order=748)

void syncSetFinishedFlag(int) (int newValue)
{
  safe_mutex_lock (&TerminateFlagMutex);
  finishedFlag = newValue;
  finishedFlag.729_1 = finishedFlag;
  if (finishedFlag.729_1 != 0) goto <D.47542>; else goto <D.47543>;
  <D.47542>:
  pthread_cond_signal (&TerminateCond);
  goto <D.47544>;
  <D.47543>:
  <D.47544>:
  safe_mutex_unlock (&TerminateFlagMutex);
  return;
}



;; Function void disposeMemory(C*&) [with C = char*] (_Z13disposeMemoryIPcEvRPT_, funcdef_no=2273, decl_uid=39869, cgraph_uid=1013, symbol_order=1061)

void disposeMemory(C*&) [with C = char*] (char * * & pBuff)
{
  _1 = *pBuff;
  if (_1 != 0B) goto <D.47545>; else goto <D.47546>;
  <D.47545>:
  _2 = *pBuff;
  if (_2 != 0B) goto <D.47547>; else goto <D.47548>;
  <D.47547>:
  _3 = *pBuff;
  operator delete [] (_3);
  goto <D.47549>;
  <D.47548>:
  <D.47549>:
  *pBuff = 0B;
  goto <D.47550>;
  <D.47546>:
  <D.47550>:
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::~basic_string() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev, funcdef_no=2087, decl_uid=19666, cgraph_uid=827, symbol_order=875)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::~basic_string() [with _CharT = char; _Traits = std::char_traits<char>; _Alloc = std::allocator<char>] (struct basic_string * const this)
{
  std::__cxx11::basic_string<char>::_M_dispose (this);
  _1 = &this->_M_dataplus;
  std::__cxx11::basic_string<char>::_Alloc_hider::~_Alloc_hider (_1);
  MEM[(struct  &)this] = {CLOBBER};
  <D.40896>:
  return;
}



;; Function (static initializers for pbzip2.cpp) (_GLOBAL__sub_I__Z20safe_cond_timed_waitP14pthread_cond_tP15pthread_mutex_tiPKc, funcdef_no=2655, decl_uid=45282, cgraph_uid=1395, symbol_order=1959)

(static initializers for pbzip2.cpp) ()
{
  __static_initialization_and_destruction_0 (1, 65535);
  return;
}



;; Function void __static_initialization_and_destruction_0(int, int) (_Z41__static_initialization_and_destruction_0ii, funcdef_no=2654, decl_uid=45276, cgraph_uid=1394, symbol_order=1442)

void __static_initialization_and_destruction_0(int, int) (int __initialize_p, int __priority)
{
  if (__initialize_p == 1) goto <D.47551>; else goto <D.47552>;
  <D.47551>:
  if (__priority == 65535) goto <D.47553>; else goto <D.47554>;
  <D.47553>:
  std::vector<outBuff>::vector (&OutputBuffer);
  __cxa_atexit (__comp_dtor , &OutputBuffer, &__dso_handle);
  goto <D.47555>;
  <D.47554>:
  <D.47555>:
  goto <D.47556>;
  <D.47552>:
  <D.47556>:
  return;
}



;; Function std::vector<_Tp, _Alloc>::vector() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt6vectorI7outBuffSaIS0_EEC2Ev, funcdef_no=2251, decl_uid=38520, cgraph_uid=991, symbol_order=1039)

std::vector<_Tp, _Alloc>::vector() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct vector * const this)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->D.38528;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_base (_1);
  return;
}



;; Function std::_Vector_base<_Tp, _Alloc>::_Vector_base() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EEC2Ev, funcdef_no=2391, decl_uid=37878, cgraph_uid=1131, symbol_order=1179)

std::_Vector_base<_Tp, _Alloc>::_Vector_base() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_base * const this)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->_M_impl;
  std::_Vector_base<outBuff, std::allocator<outBuff> >::_Vector_impl::_Vector_impl (_1);
  return;
}



;; Function std::_Vector_base<_Tp, _Alloc>::_Vector_impl::_Vector_impl() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (_ZNSt12_Vector_baseI7outBuffSaIS0_EE12_Vector_implC2Ev, funcdef_no=2455, decl_uid=37821, cgraph_uid=1195, symbol_order=1243)

std::_Vector_base<_Tp, _Alloc>::_Vector_impl::_Vector_impl() [with _Tp = outBuff; _Alloc = std::allocator<outBuff>] (struct _Vector_impl * const this)
{
  MEM[(struct  &)this] = {CLOBBER};
  std::allocator<outBuff>::allocator (this);
  this->_M_start = 0B;
  this->_M_finish = 0B;
  this->_M_end_of_storage = 0B;
  return;
}



;; Function std::allocator< <template-parameter-1-1> >::allocator() [with _Tp = outBuff] (_ZNSaI7outBuffEC2Ev, funcdef_no=2523, decl_uid=37784, cgraph_uid=1263, symbol_order=1311)

std::allocator< <template-parameter-1-1> >::allocator() [with _Tp = outBuff] (struct allocator * const this)
{
  __gnu_cxx::new_allocator<outBuff>::new_allocator (this);
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::new_allocator() [with _Tp = outBuff] (_ZN9__gnu_cxx13new_allocatorI7outBuffEC2Ev, funcdef_no=2591, decl_uid=37719, cgraph_uid=1331, symbol_order=1379)

__gnu_cxx::new_allocator<_Tp>::new_allocator() [with _Tp = outBuff] (struct new_allocator * const this)
{
  return;
}

