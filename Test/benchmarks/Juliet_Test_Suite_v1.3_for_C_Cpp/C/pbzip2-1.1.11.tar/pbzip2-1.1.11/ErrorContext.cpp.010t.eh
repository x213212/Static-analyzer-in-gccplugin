
;; Function static pbzip2::ErrorContext* pbzip2::ErrorContext::getInstance() (_ZN6pbzip212ErrorContext11getInstanceEv, funcdef_no=1111, decl_uid=26140, cgraph_uid=396, symbol_order=400)

static pbzip2::ErrorContext* pbzip2::ErrorContext::getInstance() ()
{
  struct ErrorContext * D.30394;
  struct ErrorContext * iftmp.1;
  void * D.26670;

  pthread_mutex_lock (&_err_ctx_mutex);
  _instance.0_1 = _instance;
  if (_instance.0_1 == 0B) goto <D.30384>; else goto <D.30385>;
  <D.30384>:
  D.26670 = operator new (8, &nothrow);
  if (D.26670 != 0B) goto <D.30387>; else goto <D.30388>;
  <D.30387>:
  pbzip2::ErrorContext::ErrorContext (D.26670);
  iftmp.1 = D.26670;
  goto <D.30389>;
  <D.30388>:
  iftmp.1 = D.26670;
  <D.30389>:
  _instance = iftmp.1;
  _instance.2_2 = _instance;
  if (_instance.2_2 == 0B) goto <D.30390>; else goto <D.30391>;
  <D.30390>:
  stderr.3_3 = stderr;
  fprintf (stderr.3_3, "pbzip2: *ERROR: Can\'t initialize error context - out of memory!\n");
  goto <D.30392>;
  <D.30391>:
  <D.30392>:
  goto <D.30393>;
  <D.30385>:
  <D.30393>:
  pthread_mutex_unlock (&_err_ctx_mutex);
  D.30394 = _instance;
  goto <D.30395>;
  <D.30395>:
  return D.30394;
}



;; Function pbzip2::ErrorContext::ErrorContext() (_ZN6pbzip212ErrorContextC2Ev, funcdef_no=1082, decl_uid=26162, cgraph_uid=367, symbol_order=369)

pbzip2::ErrorContext::ErrorContext() (struct ErrorContext * const this)
{
  MEM[(struct  &)this] = {CLOBBER};
  this->_first_kernel_err_no = 0;
  this->_last_kernel_err_no = 0;
  return;
}



;; Function int fprintf(FILE*, const char*, ...) (fprintf, funcdef_no=928, decl_uid=1095, cgraph_uid=262, symbol_order=264)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
int fprintf(FILE*, const char*, ...) (struct FILE * restrict __stream, const char * restrict __fmt)
{
  int D.30398;
  int D.30396;

  D.30398 = __fprintf_chk (__stream, 1, __fmt, __builtin_va_arg_pack ());
  D.30396 = D.30398;
  goto <D.30397>;
  <D.30397>:
  return D.30396;
}



;; Function static void pbzip2::ErrorContext::printErrnoMsg(FILE*, int) (_ZN6pbzip212ErrorContext13printErrnoMsgEP8_IO_FILEi, funcdef_no=1112, decl_uid=26150, cgraph_uid=397, symbol_order=401)

static void pbzip2::ErrorContext::printErrnoMsg(FILE*, int) (struct FILE * out, int err)
{
  if (err != 0) goto <D.30399>; else goto <D.30400>;
  <D.30399>:
  _1 = strerror (err);
  fprintf (out, "pbzip2: *ERROR: system call failed with errno=[%d: %s]!\n", err, _1);
  goto <D.30401>;
  <D.30400>:
  <D.30401>:
  return;
}



;; Function static void pbzip2::ErrorContext::syncPrintErrnoMsg(FILE*, int) (_ZN6pbzip212ErrorContext17syncPrintErrnoMsgEP8_IO_FILEi, funcdef_no=1113, decl_uid=26153, cgraph_uid=398, symbol_order=402)

static void pbzip2::ErrorContext::syncPrintErrnoMsg(FILE*, int) (struct FILE * out, int err)
{
  pthread_mutex_lock (&_err_ctx_mutex);
  pbzip2::ErrorContext::printErrnoMsg (out, err);
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;
}



;; Function void pbzip2::ErrorContext::printErrorMessages(FILE*) (_ZN6pbzip212ErrorContext18printErrorMessagesEP8_IO_FILE, funcdef_no=1114, decl_uid=26142, cgraph_uid=399, symbol_order=403)

void pbzip2::ErrorContext::printErrorMessages(FILE*) (struct ErrorContext * const this, struct FILE * out)
{
  pthread_mutex_lock (&_err_ctx_mutex);
  _1 = this->_first_kernel_err_no;
  pbzip2::ErrorContext::printErrnoMsg (out, _1);
  _2 = this->_last_kernel_err_no;
  pbzip2::ErrorContext::printErrnoMsg (out, _2);
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;
}



;; Function void pbzip2::ErrorContext::saveError() (_ZN6pbzip212ErrorContext9saveErrorEv, funcdef_no=1115, decl_uid=26144, cgraph_uid=400, symbol_order=404)

void pbzip2::ErrorContext::saveError() (struct ErrorContext * const this)
{
  int & err_ref;
  int newerr;
  int * iftmp.4;

  _1 = __errno_location ();
  newerr = *_1;
  pthread_mutex_lock (&_err_ctx_mutex);
  if (newerr != 0) goto <D.30402>; else goto <D.30403>;
  <D.30402>:
  _2 = this->_first_kernel_err_no;
  if (_2 == 0) goto <D.30405>; else goto <D.30406>;
  <D.30405>:
  iftmp.4 = &this->_first_kernel_err_no;
  goto <D.30407>;
  <D.30406>:
  iftmp.4 = &this->_last_kernel_err_no;
  <D.30407>:
  err_ref = iftmp.4;
  *err_ref = newerr;
  goto <D.30408>;
  <D.30403>:
  <D.30408>:
  _3 = __errno_location ();
  _4 = *_3;
  this->_last_kernel_err_no = _4;
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;
}



;; Function void pbzip2::ErrorContext::reset() (_ZN6pbzip212ErrorContext5resetEv, funcdef_no=1116, decl_uid=26146, cgraph_uid=401, symbol_order=405)

void pbzip2::ErrorContext::reset() (struct ErrorContext * const this)
{
  pthread_mutex_lock (&_err_ctx_mutex);
  this->_last_kernel_err_no = 0;
  _1 = this->_last_kernel_err_no;
  this->_first_kernel_err_no = _1;
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;
}

