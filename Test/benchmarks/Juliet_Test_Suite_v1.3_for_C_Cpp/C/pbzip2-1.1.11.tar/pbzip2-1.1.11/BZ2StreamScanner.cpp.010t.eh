
;; Function pbzip2::BZ2StreamScanner::BZ2StreamScanner(int, size_t) (_ZN6pbzip216BZ2StreamScannerC2Eim, funcdef_no=1923, decl_uid=30766, cgraph_uid=664, symbol_order=667)

pbzip2::BZ2StreamScanner::BZ2StreamScanner(int, size_t) (struct BZ2StreamScanner * const this, int hInFile, size_t inBuffCapacity)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = &_ZTVN6pbzip216BZ2StreamScannerE + 16;
  this->_vptr.BZ2StreamScanner = _1;
  _2 = &this->_bz2Header;
  std::__cxx11::basic_string<unsigned char>::basic_string (_2);
  _3 = &this->_bz2HeaderZero;
  std::__cxx11::basic_string<unsigned char>::basic_string (_3);
  this->_inBuff = 0B;
  this->_inBuffCapacity = 0;
  _4 = &this->_outBuff;
  outBuff::outBuff (_4, 0B, 0, 0, 0, 0, 1, 0B);
  this->_outBuff.buf = 0B;
  this->_outBuff.bufSize = 0;
  pbzip2::BZ2StreamScanner::init (this, hInFile, inBuffCapacity);
  return;
  <D.41124>:
  _5 = &this->_bz2HeaderZero;
  std::__cxx11::basic_string<unsigned char>::~basic_string (_5);
  resx 2
  <D.41125>:
  _6 = &this->_bz2Header;
  std::__cxx11::basic_string<unsigned char>::~basic_string (_6);
  resx 1
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::basic_string() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEC2Ev, funcdef_no=2178, decl_uid=30729, cgraph_uid=919, symbol_order=922)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::basic_string() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this)
{
  struct allocator D.38463;

  MEM[(struct  &)this] = {CLOBBER};
  _1 = &this->_M_dataplus;
  std::allocator<unsigned char>::allocator (&D.38463);
  _2 = std::__cxx11::basic_string<unsigned char>::_M_local_data (this);
  std::__cxx11::basic_string<unsigned char>::_Alloc_hider::_Alloc_hider (_1, _2, &D.38463);
  std::allocator<unsigned char>::~allocator (&D.38463);
  D.38463 = {CLOBBER};
  std::__cxx11::basic_string<unsigned char>::_M_set_length (this, 0);
  return;
}



;; Function std::allocator< <template-parameter-1-1> >::allocator() [with _Tp = unsigned char] (_ZNSaIhEC2Ev, funcdef_no=2296, decl_uid=29769, cgraph_uid=1037, symbol_order=1040)

std::allocator< <template-parameter-1-1> >::allocator() [with _Tp = unsigned char] (struct allocator * const this)
{
  __gnu_cxx::new_allocator<unsigned char>::new_allocator (this);
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::new_allocator() [with _Tp = unsigned char] (_ZN9__gnu_cxx13new_allocatorIhEC2Ev, funcdef_no=2357, decl_uid=29704, cgraph_uid=1098, symbol_order=1101)

__gnu_cxx::new_allocator<_Tp>::new_allocator() [with _Tp = unsigned char] (struct new_allocator * const this)
{
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE13_M_local_dataEv, funcdef_no=2294, decl_uid=29820, cgraph_uid=1035, symbol_order=1038)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this)
{
  unsigned char * D.41127;

  _1 = &this->D.29808._M_local_buf;
  D.41127 = std::pointer_traits<unsigned char*>::pointer_to (_1);
  goto <D.41128>;
  <D.41128>:
  return D.41127;
}



;; Function static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = unsigned char] (_ZNSt14pointer_traitsIPhE10pointer_toERh, funcdef_no=2355, decl_uid=38923, cgraph_uid=1096, symbol_order=1099)

static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = unsigned char] (__make_not_void & __r)
{
  unsigned char * D.41129;

  D.41129 = std::addressof<unsigned char> (__r);
  goto <D.41130>;
  <D.41130>:
  return D.41129;
}



;; Function _Tp* std::addressof(_Tp&) [with _Tp = unsigned char] (_ZSt9addressofIhEPT_RS0_, funcdef_no=2412, decl_uid=39501, cgraph_uid=1154, symbol_order=1157)

_Tp* std::addressof(_Tp&) [with _Tp = unsigned char] (unsigned char & __r)
{
  unsigned char * D.41131;

  D.41131 = std::__addressof<unsigned char> (__r);
  goto <D.41132>;
  <D.41132>:
  return D.41131;
}



;; Function constexpr _Tp* std::__addressof(_Tp&) [with _Tp = unsigned char] (_ZSt11__addressofIhEPT_RS0_, funcdef_no=2413, decl_uid=40554, cgraph_uid=1153, symbol_order=1156)

constexpr _Tp* std::__addressof(_Tp&) [with _Tp = unsigned char] (unsigned char & __r)
{
  unsigned char * D.41133;

  D.41133 = __r;
  goto <D.41134>;
  <D.41134>:
  return D.41133;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_Alloc_hider::_Alloc_hider(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer, _Alloc&&) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE12_Alloc_hiderC2EPhOS3_, funcdef_no=2303, decl_uid=29791, cgraph_uid=1044, symbol_order=1047)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_Alloc_hider::_Alloc_hider(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer, _Alloc&&) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct _Alloc_hider * const this, unsigned char * __dat, struct allocator & __a)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = std::move<std::allocator<unsigned char>&> (__a);
  std::allocator<unsigned char>::allocator (this, _1);
  this->_M_p = __dat;
  return;
}



;; Function constexpr typename std::remove_reference< <template-parameter-1-1> >::type&& std::move(_Tp&&) [with _Tp = std::allocator<unsigned char>&] (_ZSt4moveIRSaIhEEONSt16remove_referenceIT_E4typeEOS3_, funcdef_no=2302, decl_uid=38941, cgraph_uid=1042, symbol_order=1045)

constexpr typename std::remove_reference< <template-parameter-1-1> >::type&& std::move(_Tp&&) [with _Tp = std::allocator<unsigned char>&] (struct allocator & __t)
{
  struct type & D.41135;

  D.41135 = __t;
  goto <D.41136>;
  <D.41136>:
  return D.41135;
}



;; Function std::allocator< <template-parameter-1-1> >::allocator(const std::allocator< <template-parameter-1-1> >&) [with _Tp = unsigned char] (_ZNSaIhEC2ERKS_, funcdef_no=2363, decl_uid=29764, cgraph_uid=1104, symbol_order=1107)

std::allocator< <template-parameter-1-1> >::allocator(const std::allocator< <template-parameter-1-1> >&) [with _Tp = unsigned char] (struct allocator * const this, const struct allocator & __a)
{
  __gnu_cxx::new_allocator<unsigned char>::new_allocator (this, __a);
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::new_allocator(const __gnu_cxx::new_allocator<_Tp>&) [with _Tp = unsigned char] (_ZN9__gnu_cxx13new_allocatorIhEC2ERKS1_, funcdef_no=2415, decl_uid=29699, cgraph_uid=1156, symbol_order=1159)

__gnu_cxx::new_allocator<_Tp>::new_allocator(const __gnu_cxx::new_allocator<_Tp>&) [with _Tp = unsigned char] (struct new_allocator * const this, const struct new_allocator & D.29701)
{
  return;
}



;; Function std::allocator< <template-parameter-1-1> >::~allocator() [with _Tp = unsigned char] (_ZNSaIhED2Ev, funcdef_no=2299, decl_uid=29774, cgraph_uid=1040, symbol_order=1043)

std::allocator< <template-parameter-1-1> >::~allocator() [with _Tp = unsigned char] (struct allocator * const this)
{
  __gnu_cxx::new_allocator<unsigned char>::~new_allocator (this);
  <D.38933>:
  return;
}



;; Function __gnu_cxx::new_allocator<_Tp>::~new_allocator() [with _Tp = unsigned char] (_ZN9__gnu_cxx13new_allocatorIhED2Ev, funcdef_no=2360, decl_uid=29709, cgraph_uid=1101, symbol_order=1104)

__gnu_cxx::new_allocator<_Tp>::~new_allocator() [with _Tp = unsigned char] (struct new_allocator * const this)
{
  <D.39510>:
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_set_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE13_M_set_lengthEm, funcdef_no=2305, decl_uid=29827, cgraph_uid=1046, symbol_order=1049)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_set_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __n)
{
  const char_type D.38947;

  std::__cxx11::basic_string<unsigned char>::_M_length (this, __n);
  D.38947 = 0;
  _1 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  _2 = _1 + __n;
  __gnu_cxx::char_traits<unsigned char>::assign (_2, &D.38947);
  D.38947 = {CLOBBER};
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE9_M_lengthEm, funcdef_no=2365, decl_uid=29815, cgraph_uid=1106, symbol_order=1109)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __length)
{
  this->_M_string_length = __length;
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE7_M_dataEv, funcdef_no=2316, decl_uid=29818, cgraph_uid=1057, symbol_order=1060)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  unsigned char * D.41137;

  D.41137 = this->_M_dataplus._M_p;
  goto <D.41138>;
  <D.41138>:
  return D.41137;
}



;; Function static constexpr void __gnu_cxx::char_traits<_CharT>::assign(__gnu_cxx::char_traits<_CharT>::char_type&, const char_type&) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE6assignERhRKh, funcdef_no=2366, decl_uid=29545, cgraph_uid=1107, symbol_order=1110)

static constexpr void __gnu_cxx::char_traits<_CharT>::assign(__gnu_cxx::char_traits<_CharT>::char_type&, const char_type&) [with _CharT = unsigned char] (char_type & __c1, const char_type & __c2)
{
  _1 = *__c2;
  *__c1 = _1;
  return;
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



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::~basic_string() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEED2Ev, funcdef_no=2181, decl_uid=30734, cgraph_uid=922, symbol_order=925)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::~basic_string() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this)
{
  std::__cxx11::basic_string<unsigned char>::_M_dispose (this);
  _1 = &this->_M_dataplus;
  std::__cxx11::basic_string<unsigned char>::_Alloc_hider::~_Alloc_hider (_1);
  MEM[(struct  &)this] = {CLOBBER};
  <D.38469>:
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_dispose() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE10_M_disposeEv, funcdef_no=2306, decl_uid=29836, cgraph_uid=1047, symbol_order=1050)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_dispose() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this)
{
  bool D.41143;
  bool retval.0;

  D.41143 = std::__cxx11::basic_string<unsigned char>::_M_is_local (this);
  _1 = D.41143;
  retval.0 = ~_1;
  if (retval.0 != 0) goto <D.41140>; else goto <D.41141>;
  <D.41140>:
  _2 = this->D.29808._M_allocated_capacity;
  std::__cxx11::basic_string<unsigned char>::_M_destroy (this, _2);
  goto <D.41142>;
  <D.41141>:
  <D.41142>:
  return;
}



;; Function bool std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_is_local() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE11_M_is_localEv, funcdef_no=2367, decl_uid=29830, cgraph_uid=1108, symbol_order=1111)

bool std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_is_local() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  bool D.41144;

  _1 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  _2 = std::__cxx11::basic_string<unsigned char>::_M_local_data (this);
  D.41144 = _1 == _2;
  goto <D.41145>;
  <D.41145>:
  return D.41144;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::const_pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE13_M_local_dataEv, funcdef_no=2417, decl_uid=29822, cgraph_uid=1158, symbol_order=1161)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::const_pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_local_data() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  const unsigned char * D.41146;

  _1 = &this->D.29808._M_local_buf;
  D.41146 = std::pointer_traits<const unsigned char*>::pointer_to (_1);
  goto <D.41147>;
  <D.41147>:
  return D.41146;
}



;; Function static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = const unsigned char] (_ZNSt14pointer_traitsIPKhE10pointer_toERS0_, funcdef_no=2469, decl_uid=40591, cgraph_uid=1210, symbol_order=1213)

static _Tp* std::pointer_traits<_Tp*>::pointer_to(std::__make_not_void<_Tp>&) [with _Tp = const unsigned char] (const __make_not_void & __r)
{
  const unsigned char * D.41148;

  D.41148 = std::addressof<const unsigned char> (__r);
  goto <D.41149>;
  <D.41149>:
  return D.41148;
}



;; Function _Tp* std::addressof(_Tp&) [with _Tp = const unsigned char] (_ZSt9addressofIKhEPT_RS1_, funcdef_no=2498, decl_uid=41002, cgraph_uid=1240, symbol_order=1243)

_Tp* std::addressof(_Tp&) [with _Tp = const unsigned char] (const unsigned char & __r)
{
  const unsigned char * D.41150;

  D.41150 = std::__addressof<const unsigned char> (__r);
  goto <D.41151>;
  <D.41151>:
  return D.41150;
}



;; Function constexpr _Tp* std::__addressof(_Tp&) [with _Tp = const unsigned char] (_ZSt11__addressofIKhEPT_RS1_, funcdef_no=2499, decl_uid=41065, cgraph_uid=1239, symbol_order=1242)

constexpr _Tp* std::__addressof(_Tp&) [with _Tp = const unsigned char] (const unsigned char & __r)
{
  const unsigned char * D.41152;

  D.41152 = __r;
  goto <D.41153>;
  <D.41153>:
  return D.41152;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_destroy(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE10_M_destroyEm, funcdef_no=2368, decl_uid=29838, cgraph_uid=1109, symbol_order=1112)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_destroy(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __size)
{
  _1 = __size + 1;
  _2 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  _3 = std::__cxx11::basic_string<unsigned char>::_M_get_allocator (this);
  std::allocator_traits<std::allocator<unsigned char> >::deallocate (_3, _2, _1);
  return;
  <D.41155>:
  eh_dispatch 1
  resx 1
  <D.41154>:
  _4 = __builtin_eh_pointer (1);
  __cxa_call_unexpected (_4);
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE16_M_get_allocatorEv, funcdef_no=2419, decl_uid=29913, cgraph_uid=1160, symbol_order=1163)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this)
{
  struct allocator_type & D.41156;

  D.41156 = &this->_M_dataplus;
  goto <D.41157>;
  <D.41157>:
  return D.41156;
}



;; Function static void std::allocator_traits<std::allocator<_CharT> >::deallocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::pointer, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = unsigned char] (_ZNSt16allocator_traitsISaIhEE10deallocateERS0_Phm, funcdef_no=2418, decl_uid=29415, cgraph_uid=1159, symbol_order=1162)

static void std::allocator_traits<std::allocator<_CharT> >::deallocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::pointer, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = unsigned char] (struct allocator_type & __a, unsigned char * __p, size_type __n)
{
  __gnu_cxx::new_allocator<unsigned char>::deallocate (__a, __p, __n);
  return;
}



;; Function void __gnu_cxx::new_allocator<_Tp>::deallocate(__gnu_cxx::new_allocator<_Tp>::pointer, __gnu_cxx::new_allocator<_Tp>::size_type) [with _Tp = unsigned char] (_ZN9__gnu_cxx13new_allocatorIhE10deallocateEPhm, funcdef_no=2470, decl_uid=29658, cgraph_uid=1211, symbol_order=1214)

void __gnu_cxx::new_allocator<_Tp>::deallocate(__gnu_cxx::new_allocator<_Tp>::pointer, __gnu_cxx::new_allocator<_Tp>::size_type) [with _Tp = unsigned char] (struct new_allocator * const this, unsigned char * __p, size_type D.29661)
{
  operator delete (__p);
  return;
}



;; Function std::__cxx11::basic_string<unsigned char>::_Alloc_hider::~_Alloc_hider() (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE12_Alloc_hiderD2Ev, funcdef_no=2176, decl_uid=30871, cgraph_uid=916, symbol_order=919)

std::__cxx11::basic_string<unsigned char>::_Alloc_hider::~_Alloc_hider() (struct _Alloc_hider * const this)
{
  std::allocator<unsigned char>::~allocator (this);
  MEM[(struct  &)this] = {CLOBBER};
  <D.38459>:
  return;
}



;; Function int pbzip2::BZ2StreamScanner::init(int, size_t) (_ZN6pbzip216BZ2StreamScanner4initEim, funcdef_no=1925, decl_uid=29303, cgraph_uid=666, symbol_order=669)

int pbzip2::BZ2StreamScanner::init(int, size_t) (struct BZ2StreamScanner * const this, int hInFile, size_t inBuffCapacity)
{
  CharType bz2ZeroHeader[11];
  CharType bz2header[11];
  int D.41165;
  long unsigned int inBuffCapacity.2;
  bool retval.1;

  _1 = this->_vptr.BZ2StreamScanner;
  _2 = _1 + 16;
  _3 = *_2;
  OBJ_TYPE_REF(_3;(struct BZ2StreamScanner)this->2) (this);
  bz2header = "BZh91AY&SY";
  bz2ZeroHeader[0] = 66;
  bz2ZeroHeader[1] = 90;
  bz2ZeroHeader[2] = 104;
  bz2ZeroHeader[3] = 57;
  bz2ZeroHeader[4] = 23;
  bz2ZeroHeader[5] = 114;
  bz2ZeroHeader[6] = 69;
  bz2ZeroHeader[7] = 56;
  bz2ZeroHeader[8] = 80;
  bz2ZeroHeader[9] = 144;
  bz2ZeroHeader[10] = 0;
  this->_hInFile = hInFile;
  this->_eof = 0;
  _4 = &this->_bz2Header;
  std::__cxx11::basic_string<unsigned char>::operator= (_4, &bz2header);
  _5 = &this->_bz2HeaderZero;
  std::__cxx11::basic_string<unsigned char>::operator= (_5, &bz2ZeroHeader);
  this->_bz2HeaderFound = 0;
  this->_inBuffCapacity = 0;
  this->_errState = 0;
  this->_searchStatus = 0;
  this->_outBuffCapacityHint = 0;
  this->_outBuffCapacityLimit = 1048576;
  this->_outSequenceNumber = 0;
  this->_streamNumber = 0;
  _6 = &this->_bz2Header;
  _7 = std::__cxx11::basic_string<unsigned char>::size (_6);
  _8 = _7 * 2;
  retval.1 = inBuffCapacity < _8;
  if (retval.1 != 0) goto <D.41159>; else goto <D.41160>;
  <D.41159>:
  _9 = &this->_bz2Header;
  _10 = std::__cxx11::basic_string<unsigned char>::size (_9);
  inBuffCapacity = _10 * 2;
  goto <D.41161>;
  <D.41160>:
  <D.41161>:
  inBuffCapacity.2 = inBuffCapacity;
  _11 = operator new [] (inBuffCapacity.2, &nothrow);
  this->_inBuff = _11;
  _12 = this->_inBuff;
  if (_12 == 0B) goto <D.41163>; else goto <D.41164>;
  <D.41163>:
  _13 = this->_errState;
  _14 = _13 | 1;
  this->_errState = _14;
  this->_inBuffEnd = 0B;
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (FileData)!  Aborting...\n");
  D.41165 = -1;
  goto <D.41168>;
  <D.41164>:
  this->_inBuffCapacity = inBuffCapacity;
  _15 = this->_inBuff;
  this->_inBuffEnd = _15;
  _16 = this->_inBuffEnd;
  this->_inBuffSearchPtr = _16;
  _17 = this->_inBuffSearchPtr;
  this->_inBuffCurrent = _17;
  D.41165 = 0;
  goto <D.41168>;
  <D.41168>:
  bz2header = {CLOBBER};
  bz2ZeroHeader = {CLOBBER};
  goto <D.41166>;
  <D.41166>:
  return D.41165;
  <D.41167>:
  bz2header = {CLOBBER};
  bz2ZeroHeader = {CLOBBER};
  resx 1
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator=(const _CharT*) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEaSEPKh, funcdef_no=2183, decl_uid=30076, cgraph_uid=924, symbol_order=927)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator=(const _CharT*) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, const unsigned char * __s)
{
  struct basic_string & D.41171;
  struct basic_string & D.41169;

  D.41171 = std::__cxx11::basic_string<unsigned char>::assign (this, __s);
  D.41169 = D.41171;
  goto <D.41170>;
  <D.41170>:
  return D.41169;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::assign(const _CharT*) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE6assignEPKh, funcdef_no=2307, decl_uid=30224, cgraph_uid=1050, symbol_order=1053)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::assign(const _CharT*) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, const unsigned char * __s)
{
  struct basic_string & D.41175;
  long unsigned int D.41174;
  struct basic_string & D.41172;

  D.41174 = __gnu_cxx::char_traits<unsigned char>::length (__s);
  _1 = D.41174;
  _2 = std::__cxx11::basic_string<unsigned char>::size (this);
  D.41175 = std::__cxx11::basic_string<unsigned char>::_M_replace (this, 0, _2, __s, _1);
  D.41172 = D.41175;
  goto <D.41173>;
  <D.41173>:
  return D.41172;
}



;; Function static constexpr std::size_t __gnu_cxx::char_traits<_CharT>::length(const char_type*) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE6lengthEPKh, funcdef_no=2308, decl_uid=29558, cgraph_uid=1049, symbol_order=1052)

static constexpr std::size_t __gnu_cxx::char_traits<_CharT>::length(const char_type*) [with _CharT = unsigned char] (const char_type * __p)
{
  size_t __i;
  size_t D.41179;
  const char_type D.38952;
  bool retval.3;

  __i = 0;
  <D.41176>:
  D.38952 = 0;
  _1 = __p + __i;
  _2 = __gnu_cxx::char_traits<unsigned char>::eq (_1, &D.38952);
  retval.3 = ~_2;
  D.38952 = {CLOBBER};
  if (retval.3 != 0) goto <D.41178>; else goto <D.38954>;
  <D.41178>:
  __i = __i + 1;
  goto <D.41176>;
  <D.38954>:
  D.41179 = __i;
  goto <D.41180>;
  <D.41180>:
  return D.41179;
}



;; Function static constexpr bool __gnu_cxx::char_traits<_CharT>::eq(const char_type&, const char_type&) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE2eqERKhS3_, funcdef_no=2309, decl_uid=29548, cgraph_uid=1048, symbol_order=1051)

static constexpr bool __gnu_cxx::char_traits<_CharT>::eq(const char_type&, const char_type&) [with _CharT = unsigned char] (const char_type & __c1, const char_type & __c2)
{
  bool D.41181;

  _1 = *__c1;
  _2 = *__c2;
  D.41181 = _1 == _2;
  goto <D.41182>;
  <D.41182>:
  return D.41181;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_replace(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE10_M_replaceEmmPKhm, funcdef_no=2369, decl_uid=30451, cgraph_uid=1110, symbol_order=1113)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_replace(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __pos, size_type __len1, const unsigned char * __s, const size_type __len2)
{
  const size_type __nleft;
  const size_type __how_much;
  unsigned char * __p;
  const size_type __new_size;
  const size_type __old_size;
  struct basic_string & D.41212;
  bool retval.5;
  bool retval.4;

  std::__cxx11::basic_string<unsigned char>::_M_check_length (this, __len1, __len2, "basic_string::_M_replace");
  __old_size = std::__cxx11::basic_string<unsigned char>::size (this);
  _1 = __old_size + __len2;
  __new_size = _1 - __len1;
  _2 = std::__cxx11::basic_string<unsigned char>::capacity (this);
  retval.4 = __new_size <= _2;
  if (retval.4 != 0) goto <D.41184>; else goto <D.41185>;
  <D.41184>:
  _3 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  __p = _3 + __pos;
  _4 = __old_size - __pos;
  __how_much = _4 - __len1;
  retval.5 = std::__cxx11::basic_string<unsigned char>::_M_disjunct (this, __s);
  if (retval.5 != 0) goto <D.41187>; else goto <D.41188>;
  <D.41187>:
  _5 = __how_much != 0;
  _6 = __len1 != __len2;
  _7 = _5 & _6;
  if (_7 != 0) goto <D.41189>; else goto <D.41190>;
  <D.41189>:
  _8 = __p + __len1;
  _9 = __p + __len2;
  std::__cxx11::basic_string<unsigned char>::_S_move (_9, _8, __how_much);
  goto <D.41191>;
  <D.41190>:
  <D.41191>:
  if (__len2 != 0) goto <D.41192>; else goto <D.41193>;
  <D.41192>:
  std::__cxx11::basic_string<unsigned char>::_S_copy (__p, __s, __len2);
  goto <D.41194>;
  <D.41193>:
  <D.41194>:
  goto <D.41195>;
  <D.41188>:
  _10 = __len2 != 0;
  _11 = __len2 <= __len1;
  _12 = _10 & _11;
  if (_12 != 0) goto <D.41196>; else goto <D.41197>;
  <D.41196>:
  std::__cxx11::basic_string<unsigned char>::_S_move (__p, __s, __len2);
  goto <D.41198>;
  <D.41197>:
  <D.41198>:
  _13 = __how_much != 0;
  _14 = __len1 != __len2;
  _15 = _13 & _14;
  if (_15 != 0) goto <D.41199>; else goto <D.41200>;
  <D.41199>:
  _16 = __p + __len1;
  _17 = __p + __len2;
  std::__cxx11::basic_string<unsigned char>::_S_move (_17, _16, __how_much);
  goto <D.41201>;
  <D.41200>:
  <D.41201>:
  if (__len2 > __len1) goto <D.41202>; else goto <D.41203>;
  <D.41202>:
  _18 = __s + __len2;
  _19 = __p + __len1;
  if (_18 <= _19) goto <D.41204>; else goto <D.41205>;
  <D.41204>:
  std::__cxx11::basic_string<unsigned char>::_S_move (__p, __s, __len2);
  goto <D.41206>;
  <D.41205>:
  _20 = __p + __len1;
  if (__s >= _20) goto <D.41207>; else goto <D.41208>;
  <D.41207>:
  _21 = __len2 - __len1;
  _22 = __s + _21;
  std::__cxx11::basic_string<unsigned char>::_S_copy (__p, _22, __len2);
  goto <D.41209>;
  <D.41208>:
  _23 = __p + __len1;
  _24 = (long int) _23;
  __s.6_25 = (long int) __s;
  _26 = _24 - __s.6_25;
  __nleft = (const size_type) _26;
  std::__cxx11::basic_string<unsigned char>::_S_move (__p, __s, __nleft);
  _27 = __len2 - __nleft;
  _28 = __p + __len2;
  _29 = __p + __nleft;
  std::__cxx11::basic_string<unsigned char>::_S_copy (_29, _28, _27);
  <D.41209>:
  <D.41206>:
  goto <D.41210>;
  <D.41203>:
  <D.41210>:
  <D.41195>:
  goto <D.41211>;
  <D.41185>:
  std::__cxx11::basic_string<unsigned char>::_M_mutate (this, __pos, __len1, __s, __len2);
  <D.41211>:
  std::__cxx11::basic_string<unsigned char>::_M_set_length (this, __new_size);
  D.41212 = this;
  goto <D.41213>;
  <D.41213>:
  return D.41212;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_check_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const char*) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE15_M_check_lengthEmmPKc, funcdef_no=2420, decl_uid=29921, cgraph_uid=1161, symbol_order=1164)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_check_length(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const char*) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this, size_type __n1, size_type __n2, const char * __s)
{
  bool retval.7;

  _1 = std::__cxx11::basic_string<unsigned char>::max_size (this);
  _2 = std::__cxx11::basic_string<unsigned char>::size (this);
  _3 = __n1 - _2;
  _4 = _1 + _3;
  retval.7 = __n2 > _4;
  if (retval.7 != 0) goto <D.41215>; else goto <D.41216>;
  <D.41215>:
  __s.8_5 = __s;
  std::__throw_length_error (__s.8_5);
  <D.41216>:
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::max_size() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE8max_sizeEv, funcdef_no=2471, decl_uid=30116, cgraph_uid=1212, symbol_order=1215)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::max_size() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  size_type D.41217;

  _1 = std::__cxx11::basic_string<unsigned char>::_M_get_allocator (this);
  _2 = std::allocator_traits<std::allocator<unsigned char> >::max_size (_1);
  _3 = _2 + 18446744073709551615;
  D.41217 = _3 / 2;
  goto <D.41218>;
  <D.41218>:
  return D.41217;
}



;; Function const allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE16_M_get_allocatorEv, funcdef_no=2501, decl_uid=29915, cgraph_uid=1242, symbol_order=1245)

const allocator_type& std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_get_allocator() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  const struct allocator_type & D.41219;

  D.41219 = &this->_M_dataplus;
  goto <D.41220>;
  <D.41220>:
  return D.41219;
}



;; Function static std::allocator_traits<std::allocator<_CharT> >::size_type std::allocator_traits<std::allocator<_CharT> >::max_size(const allocator_type&) [with _Tp = unsigned char] (_ZNSt16allocator_traitsISaIhEE8max_sizeERKS0_, funcdef_no=2500, decl_uid=29443, cgraph_uid=1241, symbol_order=1244)

static std::allocator_traits<std::allocator<_CharT> >::size_type std::allocator_traits<std::allocator<_CharT> >::max_size(const allocator_type&) [with _Tp = unsigned char] (const struct allocator_type & __a)
{
  size_type D.41221;

  D.41221 = __gnu_cxx::new_allocator<unsigned char>::max_size (__a);
  goto <D.41222>;
  <D.41222>:
  return D.41221;
}



;; Function __gnu_cxx::new_allocator<_Tp>::size_type __gnu_cxx::new_allocator<_Tp>::max_size() const [with _Tp = unsigned char] (_ZNK9__gnu_cxx13new_allocatorIhE8max_sizeEv, funcdef_no=2510, decl_uid=29662, cgraph_uid=1251, symbol_order=1254)

__gnu_cxx::new_allocator<_Tp>::size_type __gnu_cxx::new_allocator<_Tp>::max_size() const [with _Tp = unsigned char] (const struct new_allocator * const this)
{
  size_type D.41223;

  D.41223 = 18446744073709551615;
  goto <D.41224>;
  <D.41224>:
  return D.41223;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::capacity() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE8capacityEv, funcdef_no=2421, decl_uid=30127, cgraph_uid=1162, symbol_order=1165)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::capacity() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  bool D.41231;
  size_type iftmp.9;
  size_type D.41225;

  D.41231 = std::__cxx11::basic_string<unsigned char>::_M_is_local (this);
  _1 = D.41231;
  if (_1 != 0) goto <D.41227>; else goto <D.41228>;
  <D.41227>:
  iftmp.9 = 15;
  goto <D.41229>;
  <D.41228>:
  iftmp.9 = this->D.29808._M_allocated_capacity;
  <D.41229>:
  D.41225 = iftmp.9;
  goto <D.41230>;
  <D.41230>:
  return D.41225;
}



;; Function bool std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_disjunct(const _CharT*) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE11_M_disjunctEPKh, funcdef_no=2422, decl_uid=29930, cgraph_uid=1163, symbol_order=1166)

bool std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_disjunct(const _CharT*) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this, const unsigned char * __s)
{
  const unsigned char * const D.40680;
  struct less D.40679;
  const unsigned char * const D.40678;
  struct less D.40668;
  bool iftmp.10;
  bool D.41232;

  _1 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  D.40678 = _1;
  _2 = std::less<const unsigned char*>::operator() (&D.40668, &__s, &D.40678);
  if (_2 != 0) goto <D.41234>; else goto <D.41237>;
  <D.41237>:
  _3 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  _4 = std::__cxx11::basic_string<unsigned char>::size (this);
  _5 = _3 + _4;
  D.40680 = _5;
  _6 = std::less<const unsigned char*>::operator() (&D.40679, &D.40680, &__s);
  if (_6 != 0) goto <D.41234>; else goto <D.41235>;
  <D.41234>:
  iftmp.10 = 1;
  goto <D.41236>;
  <D.41235>:
  iftmp.10 = 0;
  <D.41236>:
  D.41232 = iftmp.10;
  goto <D.41239>;
  <D.41239>:
  D.40680 = {CLOBBER};
  goto <D.41240>;
  <D.41240>:
  D.40679 = {CLOBBER};
  goto <D.41241>;
  <D.41241>:
  D.40678 = {CLOBBER};
  goto <D.41242>;
  <D.41242>:
  D.40668 = {CLOBBER};
  goto <D.41238>;
  <D.41238>:
  return D.41232;
}



;; Function constexpr bool std::less<_Tp>::operator()(const _Tp&, const _Tp&) const [with _Tp = const unsigned char*] (_ZNKSt4lessIPKhEclERKS1_S4_, funcdef_no=2472, decl_uid=40607, cgraph_uid=1213, symbol_order=1216)

constexpr bool std::less<_Tp>::operator()(const _Tp&, const _Tp&) const [with _Tp = const unsigned char*] (const struct less * const this, const unsigned char * const & __x, const unsigned char * const & __y)
{
  bool D.41243;

  _1 = *__x;
  _2 = *__y;
  D.41243 = _1 < _2;
  goto <D.41244>;
  <D.41244>:
  return D.41243;
}



;; Function static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_move(_CharT*, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE7_S_moveEPhPKhm, funcdef_no=2423, decl_uid=29937, cgraph_uid=1164, symbol_order=1167)

static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_move(_CharT*, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (unsigned char * __d, const unsigned char * __s, size_type __n)
{
  if (__n == 1) goto <D.41245>; else goto <D.41246>;
  <D.41245>:
  __gnu_cxx::char_traits<unsigned char>::assign (__d, __s);
  goto <D.41247>;
  <D.41246>:
  __gnu_cxx::char_traits<unsigned char>::move (__d, __s, __n);
  <D.41247>:
  return;
}



;; Function static __gnu_cxx::char_traits<_CharT>::char_type* __gnu_cxx::char_traits<_CharT>::move(__gnu_cxx::char_traits<_CharT>::char_type*, const char_type*, std::size_t) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE4moveEPhPKhm, funcdef_no=2473, decl_uid=29564, cgraph_uid=1214, symbol_order=1217)

static __gnu_cxx::char_traits<_CharT>::char_type* __gnu_cxx::char_traits<_CharT>::move(__gnu_cxx::char_traits<_CharT>::char_type*, const char_type*, std::size_t) [with _CharT = unsigned char] (char_type * __s1, const char_type * __s2, size_t __n)
{
  char_type * D.41248;

  D.41248 = __builtin_memmove (__s1, __s2, __n);
  goto <D.41249>;
  <D.41249>:
  return D.41248;
}



;; Function static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_copy(_CharT*, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE7_S_copyEPhPKhm, funcdef_no=2424, decl_uid=29933, cgraph_uid=1165, symbol_order=1168)

static void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_copy(_CharT*, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (unsigned char * __d, const unsigned char * __s, size_type __n)
{
  if (__n == 1) goto <D.41250>; else goto <D.41251>;
  <D.41250>:
  __gnu_cxx::char_traits<unsigned char>::assign (__d, __s);
  goto <D.41252>;
  <D.41251>:
  __gnu_cxx::char_traits<unsigned char>::copy (__d, __s, __n);
  <D.41252>:
  return;
}



;; Function static __gnu_cxx::char_traits<_CharT>::char_type* __gnu_cxx::char_traits<_CharT>::copy(__gnu_cxx::char_traits<_CharT>::char_type*, const char_type*, std::size_t) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE4copyEPhPKhm, funcdef_no=2474, decl_uid=29568, cgraph_uid=1215, symbol_order=1218)

static __gnu_cxx::char_traits<_CharT>::char_type* __gnu_cxx::char_traits<_CharT>::copy(__gnu_cxx::char_traits<_CharT>::char_type*, const char_type*, std::size_t) [with _CharT = unsigned char] (char_type * __s1, const char_type * __s2, size_t __n)
{
  char_type * D.41253;

  _1 = __s2 + __n;
  std::copy<const unsigned char*, unsigned char*> (__s2, _1, __s1);
  D.41253 = __s1;
  goto <D.41254>;
  <D.41254>:
  return D.41253;
}



;; Function _OI std::copy(_II, _II, _OI) [with _II = const unsigned char*; _OI = unsigned char*] (_ZSt4copyIPKhPhET0_T_S4_S3_, funcdef_no=2502, decl_uid=41009, cgraph_uid=1243, symbol_order=1246)

_OI std::copy(_II, _II, _OI) [with _II = const unsigned char*; _OI = unsigned char*] (const unsigned char * __first, const unsigned char * __last, unsigned char * __result)
{
  unsigned char * D.41257;
  unsigned char * D.41255;

  _1 = std::__miter_base<const unsigned char*> (__last);
  _2 = std::__miter_base<const unsigned char*> (__first);
  D.41257 = std::__copy_move_a2<false, const unsigned char*, unsigned char*> (_2, _1, __result);
  D.41255 = D.41257;
  goto <D.41256>;
  <D.41256>:
  return D.41255;
}



;; Function _Iterator std::__miter_base(_Iterator) [with _Iterator = const unsigned char*] (_ZSt12__miter_baseIPKhET_S2_, funcdef_no=2511, decl_uid=41077, cgraph_uid=1252, symbol_order=1255)

_Iterator std::__miter_base(_Iterator) [with _Iterator = const unsigned char*] (const unsigned char * __it)
{
  const unsigned char * D.41258;

  D.41258 = __it;
  goto <D.41259>;
  <D.41259>:
  return D.41258;
}



;; Function _OI std::__copy_move_a2(_II, _II, _OI) [with bool _IsMove = false; _II = const unsigned char*; _OI = unsigned char*] (_ZSt14__copy_move_a2ILb0EPKhPhET1_T0_S4_S3_, funcdef_no=2512, decl_uid=41079, cgraph_uid=1253, symbol_order=1256)

_OI std::__copy_move_a2(_II, _II, _OI) [with bool _IsMove = false; _II = const unsigned char*; _OI = unsigned char*] (const unsigned char * __first, const unsigned char * __last, unsigned char * __result)
{
  unsigned char * D.41260;

  _1 = std::__niter_base<unsigned char*> (__result);
  _2 = std::__niter_base<const unsigned char*> (__last);
  _3 = std::__niter_base<const unsigned char*> (__first);
  D.41260 = std::__copy_move_a<false, const unsigned char*, unsigned char*> (_3, _2, _1);
  goto <D.41261>;
  <D.41261>:
  return D.41260;
}



;; Function _Iterator std::__niter_base(_Iterator) [with _Iterator = unsigned char*] (_ZSt12__niter_baseIPhET_S1_, funcdef_no=2370, decl_uid=38958, cgraph_uid=1111, symbol_order=1114)

_Iterator std::__niter_base(_Iterator) [with _Iterator = unsigned char*] (unsigned char * __it)
{
  unsigned char * D.41262;

  D.41262 = __it;
  goto <D.41263>;
  <D.41263>:
  return D.41262;
}



;; Function _Iterator std::__niter_base(_Iterator) [with _Iterator = const unsigned char*] (_ZSt12__niter_baseIPKhET_S2_, funcdef_no=2516, decl_uid=41093, cgraph_uid=1257, symbol_order=1260)

_Iterator std::__niter_base(_Iterator) [with _Iterator = const unsigned char*] (const unsigned char * __it)
{
  const unsigned char * D.41264;

  D.41264 = __it;
  goto <D.41265>;
  <D.41265>:
  return D.41264;
}



;; Function _OI std::__copy_move_a(_II, _II, _OI) [with bool _IsMove = false; _II = const unsigned char*; _OI = unsigned char*] (_ZSt13__copy_move_aILb0EPKhPhET1_T0_S4_S3_, funcdef_no=2517, decl_uid=41095, cgraph_uid=1258, symbol_order=1261)

_OI std::__copy_move_a(_II, _II, _OI) [with bool _IsMove = false; _II = const unsigned char*; _OI = unsigned char*] (const unsigned char * __first, const unsigned char * __last, unsigned char * __result)
{
  const bool __simple;
  unsigned char * D.41266;

  __simple = 1;
  D.41266 = std::__copy_move<false, true, std::random_access_iterator_tag>::__copy_m<unsigned char> (__first, __last, __result);
  goto <D.41267>;
  <D.41267>:
  return D.41266;
}



;; Function static _Tp* std::__copy_move<_IsMove, true, std::random_access_iterator_tag>::__copy_m(const _Tp*, const _Tp*, _Tp*) [with _Tp = unsigned char; bool _IsMove = false] (_ZNSt11__copy_moveILb0ELb1ESt26random_access_iterator_tagE8__copy_mIhEEPT_PKS3_S6_S4_, funcdef_no=2426, decl_uid=39571, cgraph_uid=1167, symbol_order=1170)

static _Tp* std::__copy_move<_IsMove, true, std::random_access_iterator_tag>::__copy_m(const _Tp*, const _Tp*, _Tp*) [with _Tp = unsigned char; bool _IsMove = false] (const unsigned char * __first, const unsigned char * __last, unsigned char * __result)
{
  const ptrdiff_t _Num;
  unsigned char * D.41271;

  __last.11_1 = (long int) __last;
  __first.12_2 = (long int) __first;
  _Num = __last.11_1 - __first.12_2;
  if (_Num != 0) goto <D.41268>; else goto <D.41269>;
  <D.41268>:
  _Num.13_3 = (long unsigned int) _Num;
  __builtin_memmove (__result, __first, _Num.13_3);
  goto <D.41270>;
  <D.41269>:
  <D.41270>:
  _Num.14_4 = (sizetype) _Num;
  D.41271 = __result + _Num.14_4;
  goto <D.41272>;
  <D.41272>:
  return D.41271;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_mutate(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE9_M_mutateEmmPKhm, funcdef_no=2425, decl_uid=29979, cgraph_uid=1166, symbol_order=1169)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_mutate(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __pos, size_type __len1, const unsigned char * __s, size_type __len2)
{
  unsigned char * D.41282;
  unsigned char * __r;
  size_type __new_capacity;
  const size_type __how_much;

  _1 = std::__cxx11::basic_string<unsigned char>::length (this);
  _2 = _1 - __pos;
  __how_much = _2 - __len1;
  _3 = std::__cxx11::basic_string<unsigned char>::length (this);
  _4 = __len2 + _3;
  _5 = _4 - __len1;
  __new_capacity = _5;
  _6 = std::__cxx11::basic_string<unsigned char>::capacity (this);
  D.41282 = std::__cxx11::basic_string<unsigned char>::_M_create (this, &__new_capacity, _6);
  __r = D.41282;
  if (__pos != 0) goto <D.41273>; else goto <D.41274>;
  <D.41273>:
  _7 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  std::__cxx11::basic_string<unsigned char>::_S_copy (__r, _7, __pos);
  goto <D.41275>;
  <D.41274>:
  <D.41275>:
  _8 = __s != 0B;
  _9 = __len2 != 0;
  _10 = _8 & _9;
  if (_10 != 0) goto <D.41276>; else goto <D.41277>;
  <D.41276>:
  _11 = __r + __pos;
  std::__cxx11::basic_string<unsigned char>::_S_copy (_11, __s, __len2);
  goto <D.41278>;
  <D.41277>:
  <D.41278>:
  if (__how_much != 0) goto <D.41279>; else goto <D.41280>;
  <D.41279>:
  _12 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  _13 = __pos + __len1;
  _14 = _12 + _13;
  _15 = __pos + __len2;
  _16 = __r + _15;
  std::__cxx11::basic_string<unsigned char>::_S_copy (_16, _14, __how_much);
  goto <D.41281>;
  <D.41280>:
  <D.41281>:
  std::__cxx11::basic_string<unsigned char>::_M_dispose (this);
  std::__cxx11::basic_string<unsigned char>::_M_data (this, __r);
  __new_capacity.15_17 = __new_capacity;
  std::__cxx11::basic_string<unsigned char>::_M_capacity (this, __new_capacity.15_17);
  __new_capacity = {CLOBBER};
  return;
  <D.41283>:
  __new_capacity = {CLOBBER};
  resx 1
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::length() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE6lengthEv, funcdef_no=2475, decl_uid=30114, cgraph_uid=1216, symbol_order=1219)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::length() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  size_type D.41284;

  D.41284 = this->_M_string_length;
  goto <D.41285>;
  <D.41285>:
  return D.41284;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_create(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type&, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE9_M_createERmm, funcdef_no=2476, decl_uid=29832, cgraph_uid=1217, symbol_order=1220)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_create(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type&, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type & __capacity, size_type __old_capacity)
{
  unsigned char * D.41300;
  unsigned char * D.41298;
  bool retval.17;
  bool retval.16;

  _1 = *__capacity;
  _2 = std::__cxx11::basic_string<unsigned char>::max_size (this);
  retval.16 = _1 > _2;
  if (retval.16 != 0) goto <D.41287>; else goto <D.41288>;
  <D.41287>:
  std::__throw_length_error ("basic_string::_M_create");
  <D.41288>:
  _3 = *__capacity;
  if (__old_capacity < _3) goto <D.41289>; else goto <D.41290>;
  <D.41289>:
  _4 = *__capacity;
  _5 = __old_capacity * 2;
  if (_4 < _5) goto <D.41291>; else goto <D.41292>;
  <D.41291>:
  _6 = __old_capacity * 2;
  *__capacity = _6;
  _7 = *__capacity;
  _8 = std::__cxx11::basic_string<unsigned char>::max_size (this);
  retval.17 = _7 > _8;
  if (retval.17 != 0) goto <D.41294>; else goto <D.41295>;
  <D.41294>:
  _9 = std::__cxx11::basic_string<unsigned char>::max_size (this);
  *__capacity = _9;
  goto <D.41296>;
  <D.41295>:
  <D.41296>:
  goto <D.41297>;
  <D.41292>:
  <D.41297>:
  <D.41290>:
  _10 = *__capacity;
  _11 = _10 + 1;
  _12 = std::__cxx11::basic_string<unsigned char>::_M_get_allocator (this);
  D.41300 = std::allocator_traits<std::allocator<unsigned char> >::allocate (_12, _11);
  D.41298 = D.41300;
  goto <D.41299>;
  <D.41299>:
  return D.41298;
}



;; Function static _Tp* std::allocator_traits<std::allocator<_CharT> >::allocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = unsigned char] (_ZNSt16allocator_traitsISaIhEE8allocateERS0_m, funcdef_no=2503, decl_uid=29408, cgraph_uid=1244, symbol_order=1247)

static _Tp* std::allocator_traits<std::allocator<_CharT> >::allocate(std::allocator_traits<std::allocator<_CharT> >::allocator_type&, std::allocator_traits<std::allocator<_CharT> >::size_type) [with _Tp = unsigned char] (struct allocator_type & __a, size_type __n)
{
  unsigned char * D.41303;
  unsigned char * D.41301;

  D.41303 = __gnu_cxx::new_allocator<unsigned char>::allocate (__a, __n, 0B);
  D.41301 = D.41303;
  goto <D.41302>;
  <D.41302>:
  return D.41301;
}



;; Function _Tp* __gnu_cxx::new_allocator<_Tp>::allocate(__gnu_cxx::new_allocator<_Tp>::size_type, const void*) [with _Tp = unsigned char] (_ZN9__gnu_cxx13new_allocatorIhE8allocateEmPKv, funcdef_no=2513, decl_uid=29654, cgraph_uid=1254, symbol_order=1257)

_Tp* __gnu_cxx::new_allocator<_Tp>::allocate(__gnu_cxx::new_allocator<_Tp>::size_type, const void*) [with _Tp = unsigned char] (struct new_allocator * const this, size_type __n, const void * D.29657)
{
  unsigned char * D.41309;
  unsigned char * D.41307;
  bool retval.18;

  _1 = __gnu_cxx::new_allocator<unsigned char>::max_size (this);
  retval.18 = __n > _1;
  if (retval.18 != 0) goto <D.41305>; else goto <D.41306>;
  <D.41305>:
  std::__throw_bad_alloc ();
  <D.41306>:
  D.41309 = operator new (__n);
  D.41307 = D.41309;
  goto <D.41308>;
  <D.41308>:
  return D.41307;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE7_M_dataEPh, funcdef_no=2477, decl_uid=29812, cgraph_uid=1218, symbol_order=1221)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_data(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::pointer) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, unsigned char * __p)
{
  this->_M_dataplus._M_p = __p;
  return;
}



;; Function void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_capacity(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE11_M_capacityEm, funcdef_no=2478, decl_uid=29824, cgraph_uid=1219, symbol_order=1222)

void std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_capacity(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __capacity)
{
  this->D.29808._M_allocated_capacity = __capacity;
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE4sizeEv, funcdef_no=2173, decl_uid=30112, cgraph_uid=914, symbol_order=917)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size() const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this)
{
  size_type D.41310;

  D.41310 = this->_M_string_length;
  goto <D.41311>;
  <D.41311>:
  return D.41310;
}



;; Function virtual void pbzip2::BZ2StreamScanner::dispose() (_ZN6pbzip216BZ2StreamScanner7disposeEv, funcdef_no=1926, decl_uid=29333, cgraph_uid=667, symbol_order=670)

virtual void pbzip2::BZ2StreamScanner::dispose() (struct BZ2StreamScanner * const this)
{
  _1 = &this->_outBuff.buf;
  disposeMemory<char> (_1);
  this->_outBuff.bufSize = 0;
  _2 = &this->_inBuff;
  disposeMemory<unsigned char> (_2);
  this->_inBuffCapacity = 0;
  return;
}



;; Function void disposeMemory(C*&) [with C = char] (_Z13disposeMemoryIcEvRPT_, funcdef_no=2184, decl_uid=36382, cgraph_uid=925, symbol_order=928)

void disposeMemory(C*&) [with C = char] (char * & pBuff)
{
  _1 = *pBuff;
  if (_1 != 0B) goto <D.41312>; else goto <D.41313>;
  <D.41312>:
  _2 = *pBuff;
  if (_2 != 0B) goto <D.41314>; else goto <D.41315>;
  <D.41314>:
  _3 = *pBuff;
  operator delete [] (_3);
  goto <D.41316>;
  <D.41315>:
  <D.41316>:
  *pBuff = 0B;
  goto <D.41317>;
  <D.41313>:
  <D.41317>:
  return;
}



;; Function void disposeMemory(C*&) [with C = unsigned char] (_Z13disposeMemoryIhEvRPT_, funcdef_no=2185, decl_uid=36384, cgraph_uid=926, symbol_order=929)

void disposeMemory(C*&) [with C = unsigned char] (unsigned char * & pBuff)
{
  _1 = *pBuff;
  if (_1 != 0B) goto <D.41318>; else goto <D.41319>;
  <D.41318>:
  _2 = *pBuff;
  if (_2 != 0B) goto <D.41320>; else goto <D.41321>;
  <D.41320>:
  _3 = *pBuff;
  operator delete [] (_3);
  goto <D.41322>;
  <D.41321>:
  <D.41322>:
  *pBuff = 0B;
  goto <D.41323>;
  <D.41319>:
  <D.41323>:
  return;
}



;; Function pbzip2::BZ2StreamScanner::~BZ2StreamScanner() (_ZN6pbzip216BZ2StreamScannerD2Ev, funcdef_no=1928, decl_uid=30776, cgraph_uid=669, symbol_order=672)

pbzip2::BZ2StreamScanner::~BZ2StreamScanner() (struct BZ2StreamScanner * const this)
{
  _1 = &_ZTVN6pbzip216BZ2StreamScannerE + 16;
  this->_vptr.BZ2StreamScanner = _1;
  pbzip2::BZ2StreamScanner::dispose (this);
  _2 = &this->_bz2HeaderZero;
  std::__cxx11::basic_string<unsigned char>::~basic_string (_2);
  _3 = &this->_bz2Header;
  std::__cxx11::basic_string<unsigned char>::~basic_string (_3);
  MEM[(struct  &)this] = {CLOBBER};
  <D.36393>:
  return;
}



;; Function virtual pbzip2::BZ2StreamScanner::~BZ2StreamScanner() (_ZN6pbzip216BZ2StreamScannerD0Ev, funcdef_no=1930, decl_uid=30770, cgraph_uid=671, symbol_order=674)

virtual pbzip2::BZ2StreamScanner::~BZ2StreamScanner() (struct BZ2StreamScanner * const this)
{
  pbzip2::BZ2StreamScanner::~BZ2StreamScanner (this);
  operator delete (this, 208);
  return;
}



;; Function int pbzip2::BZ2StreamScanner::ensureOutBuffCapacity(size_t) (_ZN6pbzip216BZ2StreamScanner21ensureOutBuffCapacityEm, funcdef_no=1931, decl_uid=29351, cgraph_uid=672, symbol_order=675)

int pbzip2::BZ2StreamScanner::ensureOutBuffCapacity(size_t) (struct BZ2StreamScanner * const this, size_t newSize)
{
  char * newBuff;
  long unsigned int D.41339;
  bool iftmp.20;
  bool retval.19;
  int D.41326;

  _1 = this->_outBuffCapacity;
  if (newSize <= _1) goto <D.41324>; else goto <D.41325>;
  <D.41324>:
  D.41326 = 0;
  goto <D.41348>;
  <D.41325>:
  _2 = this->_outBuffCapacityHint;
  if (newSize > _2) goto <D.41327>; else goto <D.41328>;
  <D.41327>:
  _3 = newSize * 11;
  _4 = _3 / 10;
  this->_outBuffCapacityHint = _4;
  _5 = pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit (this);
  if (newSize <= _5) goto <D.41333>; else goto <D.41331>;
  <D.41333>:
  _6 = this->_outBuffCapacityHint;
  _7 = pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit (this);
  if (_6 > _7) goto <D.41334>; else goto <D.41331>;
  <D.41334>:
  iftmp.20 = 1;
  goto <D.41332>;
  <D.41331>:
  iftmp.20 = 0;
  <D.41332>:
  retval.19 = iftmp.20;
  if (retval.19 != 0) goto <D.41335>; else goto <D.41336>;
  <D.41335>:
  _8 = pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit (this);
  this->_outBuffCapacityHint = _8;
  goto <D.41337>;
  <D.41336>:
  <D.41337>:
  goto <D.41338>;
  <D.41328>:
  <D.41338>:
  D.41339 = this->_outBuffCapacityHint;
  newBuff = operator new [] (D.41339, &nothrow);
  if (newBuff == 0B) goto <D.41340>; else goto <D.41341>;
  <D.41340>:
  _9 = this->_outBuffCapacityHint;
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (ensureOutBuffCapacity/%u)!Aborting...\n", _9);
  _10 = this->_errState;
  _11 = _10 | 2;
  this->_errState = _11;
  D.41326 = -1;
  goto <D.41348>;
  <D.41341>:
  _12 = this->_outBuff.buf;
  if (_12 != 0B) goto <D.41342>; else goto <D.41343>;
  <D.41342>:
  _13 = this->_outBuff.bufSize;
  _14 = (long unsigned int) _13;
  _15 = this->_outBuff.buf;
  memcpy (newBuff, _15, _14);
  _16 = this->_outBuff.buf;
  if (_16 != 0B) goto <D.41344>; else goto <D.41345>;
  <D.41344>:
  _17 = this->_outBuff.buf;
  operator delete [] (_17);
  goto <D.41346>;
  <D.41345>:
  <D.41346>:
  goto <D.41347>;
  <D.41343>:
  <D.41347>:
  _18 = this->_outBuffCapacityHint;
  _19 = this->_outBuff.bufSize;
  _20 = (long unsigned int) _19;
  pbzip2::BZ2StreamScanner::initOutBuff (this, newBuff, _20, _18);
  D.41326 = 0;
  goto <D.41348>;
  <D.41348>:
  return D.41326;
}



;; Function size_t pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit() const (_ZNK6pbzip216BZ2StreamScanner23getOutBuffCapacityLimitEv, funcdef_no=1446, decl_uid=29375, cgraph_uid=455, symbol_order=457)

size_t pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit() const (const struct BZ2StreamScanner * const this)
{
  size_t D.41349;

  D.41349 = this->_outBuffCapacityLimit;
  goto <D.41350>;
  <D.41350>:
  return D.41349;
}



;; Function void* memcpy(void*, const void*, size_t) (memcpy, funcdef_no=1911, decl_uid=1023, cgraph_uid=652, symbol_order=654)

__attribute__((nothrow, __artificial__, __gnu_inline__, __always_inline__, __leaf__))
void* memcpy(void*, const void*, size_t) (void * restrict __dest, const void * restrict __src, size_t __len)
{
  void * D.41351;

  _1 = __builtin_object_size (__dest, 0);
  D.41351 = __builtin___memcpy_chk (__dest, __src, __len, _1);
  goto <D.41352>;
  <D.41352>:
  return D.41351;
}



;; Function int pbzip2::BZ2StreamScanner::appendOutBuffData(pbzip2::BZ2StreamScanner::CharType*) (_ZN6pbzip216BZ2StreamScanner17appendOutBuffDataEPh, funcdef_no=1932, decl_uid=29344, cgraph_uid=673, symbol_order=676)

int pbzip2::BZ2StreamScanner::appendOutBuffData(pbzip2::BZ2StreamScanner::CharType*) (struct BZ2StreamScanner * const this, CharType * end)
{
  int D.41364;
  size_t newSize;
  int additionSize;
  bool retval.24;
  bool retval.22;
  int D.41355;

  end.21_1 = (long int) end;
  _2 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _3 = (long int) _2;
  _4 = end.21_1 - _3;
  additionSize = (int) _4;
  if (additionSize <= 0) goto <D.41353>; else goto <D.41354>;
  <D.41353>:
  D.41355 = 0;
  goto <D.41363>;
  <D.41354>:
  retval.22 = pbzip2::BZ2StreamScanner::isBz2HeaderFound (this);
  if (retval.22 != 0) goto <D.41357>; else goto <D.41358>;
  <D.41357>:
  _5 = this->_outBuff.bufSize;
  additionSize.23_6 = (unsigned int) additionSize;
  _7 = _5 + additionSize.23_6;
  newSize = (size_t) _7;
  D.41364 = pbzip2::BZ2StreamScanner::ensureOutBuffCapacity (this, newSize);
  _8 = D.41364;
  retval.24 = _8 != 0;
  if (retval.24 != 0) goto <D.41360>; else goto <D.41361>;
  <D.41360>:
  D.41355 = -1;
  goto <D.41363>;
  <D.41361>:
  _9 = (long unsigned int) additionSize;
  _10 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _11 = pbzip2::BZ2StreamScanner::getOutBuffEnd (this);
  memcpy (_11, _10, _9);
  _12 = this->_outBuff.bufSize;
  additionSize.25_13 = (unsigned int) additionSize;
  _14 = _12 + additionSize.25_13;
  this->_outBuff.bufSize = _14;
  goto <D.41362>;
  <D.41358>:
  <D.41362>:
  this->_inBuffCurrent = end;
  D.41355 = additionSize;
  goto <D.41363>;
  <D.41363>:
  return D.41355;
}



;; Function bool pbzip2::BZ2StreamScanner::isBz2HeaderFound() const (_ZNK6pbzip216BZ2StreamScanner16isBz2HeaderFoundEv, funcdef_no=1432, decl_uid=29325, cgraph_uid=441, symbol_order=443)

bool pbzip2::BZ2StreamScanner::isBz2HeaderFound() const (const struct BZ2StreamScanner * const this)
{
  bool D.41365;

  D.41365 = this->_bz2HeaderFound;
  goto <D.41366>;
  <D.41366>:
  return D.41365;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffCurrent() (_ZN6pbzip216BZ2StreamScanner16getInBuffCurrentEv, funcdef_no=1442, decl_uid=29359, cgraph_uid=451, symbol_order=453)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffCurrent() (struct BZ2StreamScanner * const this)
{
  CharType * D.41367;

  D.41367 = this->_inBuffCurrent;
  goto <D.41368>;
  <D.41368>:
  return D.41367;
}



;; Function char* pbzip2::BZ2StreamScanner::getOutBuffEnd() (_ZN6pbzip216BZ2StreamScanner13getOutBuffEndEv, funcdef_no=1444, decl_uid=29363, cgraph_uid=453, symbol_order=455)

char* pbzip2::BZ2StreamScanner::getOutBuffEnd() (struct BZ2StreamScanner * const this)
{
  char * D.41369;

  _1 = this->_outBuff.buf;
  _2 = this->_outBuff.bufSize;
  _3 = (sizetype) _2;
  D.41369 = _1 + _3;
  goto <D.41370>;
  <D.41370>:
  return D.41369;
}



;; Function int pbzip2::BZ2StreamScanner::appendOutBuffDataUpToLimit() (_ZN6pbzip216BZ2StreamScanner26appendOutBuffDataUpToLimitEv, funcdef_no=1933, decl_uid=29348, cgraph_uid=675, symbol_order=678)

int pbzip2::BZ2StreamScanner::appendOutBuffDataUpToLimit() (struct BZ2StreamScanner * const this)
{
  int D.41379;
  CharType * end;
  CharType * end2;
  CharType * end1;
  int maxAddition;
  size_t maxCapacity;
  unsigned char * const D.36422;
  unsigned char * const D.36423;
  bool retval.26;
  int D.41373;
  const long unsigned int D.36416;

  _1 = &this->_outBuffCapacity;
  _2 = pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit (this);
  D.36416 = _2;
  _3 = std::max<long unsigned int> (&D.36416, _1);
  maxCapacity = *_3;
  D.36416 = {CLOBBER};
  _4 = (unsigned int) maxCapacity;
  _5 = this->_outBuff.bufSize;
  _6 = _4 - _5;
  maxAddition = (int) _6;
  if (maxAddition <= 0) goto <D.41371>; else goto <D.41372>;
  <D.41371>:
  D.41373 = 0;
  goto <D.41381>;
  <D.41372>:
  retval.26 = pbzip2::BZ2StreamScanner::eof (this);
  if (retval.26 != 0) goto <D.41375>; else goto <D.41376>;
  <D.41375>:
  _7 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  end1 = _7;
  goto <D.41377>;
  <D.41376>:
  _8 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  _9 = pbzip2::BZ2StreamScanner::getHeaderSize (this);
  _10 = 1 - _9;
  _11 = _8 + _10;
  D.36423 = _11;
  _12 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  D.36422 = _12;
  _13 = std::min<unsigned char*> (&D.36422, &D.36423);
  _14 = *_13;
  end1 = _14;
  D.36422 = {CLOBBER};
  D.36423 = {CLOBBER};
  <D.41377>:
  _15 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _16 = (sizetype) maxAddition;
  _17 = _15 + _16;
  end2 = _17;
  _18 = std::min<unsigned char*> (&end1, &end2);
  end = *_18;
  D.41379 = pbzip2::BZ2StreamScanner::appendOutBuffData (this, end);
  D.41373 = D.41379;
  goto <D.41381>;
  <D.41381>:
  end1 = {CLOBBER};
  end2 = {CLOBBER};
  goto <D.41378>;
  <D.41378>:
  return D.41373;
  <D.41380>:
  end1 = {CLOBBER};
  end2 = {CLOBBER};
  resx 1
}



;; Function constexpr const _Tp& std::max(const _Tp&, const _Tp&) [with _Tp = long unsigned int] (_ZSt3maxImERKT_S2_S2_, funcdef_no=2186, decl_uid=36413, cgraph_uid=927, symbol_order=930)

constexpr const _Tp& std::max(const _Tp&, const _Tp&) [with _Tp = long unsigned int] (const long unsigned int & __a, const long unsigned int & __b)
{
  const long unsigned int & D.41384;

  _1 = *__a;
  _2 = *__b;
  if (_1 < _2) goto <D.41382>; else goto <D.41383>;
  <D.41382>:
  D.41384 = __b;
  goto <D.41385>;
  <D.41383>:
  D.41384 = __a;
  goto <D.41385>;
  <D.41385>:
  return D.41384;
}



;; Function bool pbzip2::BZ2StreamScanner::eof() const (_ZNK6pbzip216BZ2StreamScanner3eofEv, funcdef_no=1434, decl_uid=29329, cgraph_uid=443, symbol_order=445)

bool pbzip2::BZ2StreamScanner::eof() const (const struct BZ2StreamScanner * const this)
{
  bool D.41386;

  D.41386 = this->_eof;
  goto <D.41387>;
  <D.41387>:
  return D.41386;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffEnd() (_ZN6pbzip216BZ2StreamScanner12getInBuffEndEv, funcdef_no=1440, decl_uid=29355, cgraph_uid=449, symbol_order=451)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffEnd() (struct BZ2StreamScanner * const this)
{
  CharType * D.41388;

  D.41388 = this->_inBuffEnd;
  goto <D.41389>;
  <D.41389>:
  return D.41388;
}



;; Function size_t pbzip2::BZ2StreamScanner::getHeaderSize() const (_ZNK6pbzip216BZ2StreamScanner13getHeaderSizeEv, funcdef_no=1429, decl_uid=29319, cgraph_uid=438, symbol_order=440)

size_t pbzip2::BZ2StreamScanner::getHeaderSize() const (const struct BZ2StreamScanner * const this)
{
  size_t D.41390;

  _1 = &this->_bz2Header;
  D.41390 = std::__cxx11::basic_string<unsigned char>::size (_1);
  goto <D.41391>;
  <D.41391>:
  return D.41390;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffSearchPtr() (_ZN6pbzip216BZ2StreamScanner18getInBuffSearchPtrEv, funcdef_no=1443, decl_uid=29361, cgraph_uid=452, symbol_order=454)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffSearchPtr() (struct BZ2StreamScanner * const this)
{
  CharType * D.41392;

  D.41392 = this->_inBuffSearchPtr;
  goto <D.41393>;
  <D.41393>:
  return D.41392;
}



;; Function constexpr const _Tp& std::min(const _Tp&, const _Tp&) [with _Tp = unsigned char*] (_ZSt3minIPhERKT_S3_S3_, funcdef_no=1934, decl_uid=36419, cgraph_uid=674, symbol_order=677)

constexpr const _Tp& std::min(const _Tp&, const _Tp&) [with _Tp = unsigned char*] (unsigned char * const & __a, unsigned char * const & __b)
{
  unsigned char * const & D.41396;

  _1 = *__b;
  _2 = *__a;
  if (_1 < _2) goto <D.41394>; else goto <D.41395>;
  <D.41394>:
  D.41396 = __b;
  goto <D.41397>;
  <D.41395>:
  D.41396 = __a;
  goto <D.41397>;
  <D.41397>:
  return D.41396;
}



;; Function int pbzip2::BZ2StreamScanner::rewindInBuff() (_ZN6pbzip216BZ2StreamScanner12rewindInBuffEv, funcdef_no=1935, decl_uid=29371, cgraph_uid=676, symbol_order=679)

int pbzip2::BZ2StreamScanner::rewindInBuff() (struct BZ2StreamScanner * const this)
{
  int D.41411;
  int tailSize;
  int ret;
  int D.41406;
  bool iftmp.28;
  bool retval.27;

  _1 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  _2 = &this->_bz2Header;
  _3 = std::__cxx11::basic_string<unsigned char>::size (_2);
  _4 = 1 - _3;
  _5 = _1 + _4;
  this->_inBuffSearchPtr = _5;
  D.41411 = pbzip2::BZ2StreamScanner::appendOutBuffData (this);
  ret = D.41411;
  _6 = pbzip2::BZ2StreamScanner::failed (this);
  if (_6 != 0) goto <D.41400>; else goto <D.41403>;
  <D.41403>:
  if (ret < 0) goto <D.41400>; else goto <D.41401>;
  <D.41400>:
  iftmp.28 = 1;
  goto <D.41402>;
  <D.41401>:
  iftmp.28 = 0;
  <D.41402>:
  retval.27 = iftmp.28;
  if (retval.27 != 0) goto <D.41404>; else goto <D.41405>;
  <D.41404>:
  D.41406 = -1;
  goto <D.41410>;
  <D.41405>:
  if (ret == 0) goto <D.41407>; else goto <D.41408>;
  <D.41407>:
  _7 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  this->_inBuffSearchPtr = _7;
  goto <D.41409>;
  <D.41408>:
  <D.41409>:
  _8 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  _9 = (long int) _8;
  _10 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _11 = (long int) _10;
  _12 = _9 - _11;
  tailSize = (int) _12;
  _13 = pbzip2::BZ2StreamScanner::getInBuffBegin (this);
  _14 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  _15 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  std::copy<unsigned char*, unsigned char*> (_15, _14, _13);
  _16 = pbzip2::BZ2StreamScanner::getInBuffBegin (this);
  _17 = (sizetype) tailSize;
  _18 = _16 + _17;
  this->_inBuffEnd = _18;
  _19 = pbzip2::BZ2StreamScanner::getInBuffBegin (this);
  this->_inBuffCurrent = _19;
  _20 = pbzip2::BZ2StreamScanner::getInBuffBegin (this);
  this->_inBuffSearchPtr = _20;
  D.41406 = 0;
  goto <D.41410>;
  <D.41410>:
  return D.41406;
}



;; Function int pbzip2::BZ2StreamScanner::appendOutBuffData() (_ZN6pbzip216BZ2StreamScanner17appendOutBuffDataEv, funcdef_no=1439, decl_uid=29346, cgraph_uid=448, symbol_order=450)

int pbzip2::BZ2StreamScanner::appendOutBuffData() (struct BZ2StreamScanner * const this)
{
  int D.41414;
  int D.41412;

  _1 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  D.41414 = pbzip2::BZ2StreamScanner::appendOutBuffData (this, _1);
  D.41412 = D.41414;
  goto <D.41413>;
  <D.41413>:
  return D.41412;
}



;; Function bool pbzip2::BZ2StreamScanner::failed() (_ZN6pbzip216BZ2StreamScanner6failedEv, funcdef_no=1431, decl_uid=29323, cgraph_uid=440, symbol_order=442)

bool pbzip2::BZ2StreamScanner::failed() (struct BZ2StreamScanner * const this)
{
  bool D.41415;

  _1 = this->_errState;
  D.41415 = _1 != 0;
  goto <D.41416>;
  <D.41416>:
  return D.41415;
}



;; Function _OI std::copy(_II, _II, _OI) [with _II = unsigned char*; _OI = unsigned char*] (_ZSt4copyIPhS0_ET0_T_S2_S1_, funcdef_no=2187, decl_uid=36432, cgraph_uid=928, symbol_order=931)

_OI std::copy(_II, _II, _OI) [with _II = unsigned char*; _OI = unsigned char*] (unsigned char * __first, unsigned char * __last, unsigned char * __result)
{
  unsigned char * D.41419;
  unsigned char * D.41417;

  _1 = std::__miter_base<unsigned char*> (__last);
  _2 = std::__miter_base<unsigned char*> (__first);
  D.41419 = std::__copy_move_a2<false, unsigned char*, unsigned char*> (_2, _1, __result);
  D.41417 = D.41419;
  goto <D.41418>;
  <D.41418>:
  return D.41417;
}



;; Function _Iterator std::__miter_base(_Iterator) [with _Iterator = unsigned char*] (_ZSt12__miter_baseIPhET_S1_, funcdef_no=2310, decl_uid=38483, cgraph_uid=1051, symbol_order=1054)

_Iterator std::__miter_base(_Iterator) [with _Iterator = unsigned char*] (unsigned char * __it)
{
  unsigned char * D.41420;

  D.41420 = __it;
  goto <D.41421>;
  <D.41421>:
  return D.41420;
}



;; Function _OI std::__copy_move_a2(_II, _II, _OI) [with bool _IsMove = false; _II = unsigned char*; _OI = unsigned char*] (_ZSt14__copy_move_a2ILb0EPhS0_ET1_T0_S2_S1_, funcdef_no=2311, decl_uid=38485, cgraph_uid=1052, symbol_order=1055)

_OI std::__copy_move_a2(_II, _II, _OI) [with bool _IsMove = false; _II = unsigned char*; _OI = unsigned char*] (unsigned char * __first, unsigned char * __last, unsigned char * __result)
{
  unsigned char * D.41424;
  unsigned char * D.41422;

  _1 = std::__niter_base<unsigned char*> (__result);
  _2 = std::__niter_base<unsigned char*> (__last);
  _3 = std::__niter_base<unsigned char*> (__first);
  D.41424 = std::__copy_move_a<false, unsigned char*, unsigned char*> (_3, _2, _1);
  D.41422 = D.41424;
  goto <D.41423>;
  <D.41423>:
  return D.41422;
}



;; Function _OI std::__copy_move_a(_II, _II, _OI) [with bool _IsMove = false; _II = unsigned char*; _OI = unsigned char*] (_ZSt13__copy_move_aILb0EPhS0_ET1_T0_S2_S1_, funcdef_no=2371, decl_uid=38960, cgraph_uid=1112, symbol_order=1115)

_OI std::__copy_move_a(_II, _II, _OI) [with bool _IsMove = false; _II = unsigned char*; _OI = unsigned char*] (unsigned char * __first, unsigned char * __last, unsigned char * __result)
{
  const bool __simple;
  unsigned char * D.41425;

  __simple = 1;
  D.41425 = std::__copy_move<false, true, std::random_access_iterator_tag>::__copy_m<unsigned char> (__first, __last, __result);
  goto <D.41426>;
  <D.41426>:
  return D.41425;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffBegin() (_ZN6pbzip216BZ2StreamScanner14getInBuffBeginEv, funcdef_no=1441, decl_uid=29357, cgraph_uid=450, symbol_order=452)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::getInBuffBegin() (struct BZ2StreamScanner * const this)
{
  CharType * D.41427;

  D.41427 = this->_inBuff;
  goto <D.41428>;
  <D.41428>:
  return D.41427;
}



;; Function int pbzip2::BZ2StreamScanner::readData() (_ZN6pbzip216BZ2StreamScanner8readDataEv, funcdef_no=1936, decl_uid=29353, cgraph_uid=677, symbol_order=680)

int pbzip2::BZ2StreamScanner::readData() (struct BZ2StreamScanner * const this)
{
  long int D.41442;
  int bytesRead;
  bool retval.30;
  int D.41432;
  bool retval.29;

  pbzip2::BZ2StreamScanner::rewindInBuff (this);
  retval.29 = pbzip2::BZ2StreamScanner::failed (this);
  if (retval.29 != 0) goto <D.41430>; else goto <D.41431>;
  <D.41430>:
  D.41432 = -1;
  goto <D.41441>;
  <D.41431>:
  _1 = pbzip2::BZ2StreamScanner::getInBuffSize (this);
  _2 = pbzip2::BZ2StreamScanner::getInBuffCapacity (this);
  retval.30 = _1 >= _2;
  if (retval.30 != 0) goto <D.41434>; else goto <D.41435>;
  <D.41434>:
  _3 = pbzip2::BZ2StreamScanner::getInBuffCapacity (this);
  _4 = pbzip2::BZ2StreamScanner::getInBuffSize (this);
  handle_error (1, -1, "pbzip2: *ERROR: BZ2StreamScanner::readData not enough buffer free space! inBuffSize=%u, _inBuffCapacity=%u! Aborting...\n", _4, _3);
  _5 = this->_errState;
  _6 = _5 | 8;
  this->_errState = _6;
  D.41432 = -1;
  goto <D.41441>;
  <D.41435>:
  _7 = pbzip2::BZ2StreamScanner::getInBuffCapacity (this);
  _8 = pbzip2::BZ2StreamScanner::getInBuffSize (this);
  _9 = _7 - _8;
  _10 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  _11 = this->_hInFile;
  D.41442 = do_read (_11, _10, _9);
  _12 = D.41442;
  bytesRead = (int) _12;
  if (bytesRead > 0) goto <D.41436>; else goto <D.41437>;
  <D.41436>:
  _13 = this->_inBuffEnd;
  _14 = (sizetype) bytesRead;
  _15 = _13 + _14;
  this->_inBuffEnd = _15;
  goto <D.41438>;
  <D.41437>:
  if (bytesRead < 0) goto <D.41439>; else goto <D.41440>;
  <D.41439>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not read from input file [err=%d]! Aborting...\n", bytesRead);
  _16 = this->_errState;
  _17 = _16 | 4;
  this->_errState = _17;
  D.41432 = bytesRead;
  goto <D.41441>;
  <D.41440>:
  this->_eof = 1;
  <D.41438>:
  D.41432 = bytesRead;
  goto <D.41441>;
  <D.41441>:
  return D.41432;
}



;; Function size_t pbzip2::BZ2StreamScanner::getInBuffCapacity() const (_ZNK6pbzip216BZ2StreamScanner17getInBuffCapacityEv, funcdef_no=1427, decl_uid=29312, cgraph_uid=436, symbol_order=438)

size_t pbzip2::BZ2StreamScanner::getInBuffCapacity() const (const struct BZ2StreamScanner * const this)
{
  size_t D.41443;

  D.41443 = this->_inBuffCapacity;
  goto <D.41444>;
  <D.41444>:
  return D.41443;
}



;; Function size_t pbzip2::BZ2StreamScanner::getInBuffSize() const (_ZNK6pbzip216BZ2StreamScanner13getInBuffSizeEv, funcdef_no=1426, decl_uid=29310, cgraph_uid=435, symbol_order=437)

size_t pbzip2::BZ2StreamScanner::getInBuffSize() const (const struct BZ2StreamScanner * const this)
{
  size_t D.41445;

  _1 = this->_inBuffEnd;
  _2 = (long int) _1;
  _3 = this->_inBuff;
  _4 = (long int) _3;
  _5 = _2 - _4;
  D.41445 = (size_t) _5;
  goto <D.41446>;
  <D.41446>:
  return D.41445;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::locateHeaderPrefixInBuff() (_ZN6pbzip216BZ2StreamScanner24locateHeaderPrefixInBuffEv, funcdef_no=1937, decl_uid=29373, cgraph_uid=678, symbol_order=681)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::locateHeaderPrefixInBuff() (struct BZ2StreamScanner * const this)
{
  unsigned char * D.41469;
  int D.41468;
  size_t prefixLen;
  CharType * D.41466;
  struct __normal_iterator D.41465;
  struct __normal_iterator D.41464;
  struct iterator D.36547;
  bool retval.34;
  bool iftmp.33;
  bool retval.32;
  bool retval.31;

  prefixLen = 3;
  _1 = pbzip2::BZ2StreamScanner::isBz2HeaderFound (this);
  retval.31 = ~_1;
  if (retval.31 != 0) goto <D.41448>; else goto <D.41449>;
  <D.41448>:
  _2 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _3 = pbzip2::BZ2StreamScanner::getInBuffBegin (this);
  if (_2 != _3) goto <D.41452>; else goto <D.41455>;
  <D.41455>:
  _4 = pbzip2::BZ2StreamScanner::getInBuffSize (this);
  _5 = &this->_bz2Header;
  _6 = std::__cxx11::basic_string<unsigned char>::size (_5);
  if (_4 < _6) goto <D.41452>; else goto <D.41453>;
  <D.41452>:
  iftmp.33 = 1;
  goto <D.41454>;
  <D.41453>:
  iftmp.33 = 0;
  <D.41454>:
  retval.32 = iftmp.33;
  if (retval.32 != 0) goto <D.41456>; else goto <D.41457>;
  <D.41456>:
  _7 = this->_errState;
  _8 = _7 | 32;
  this->_errState = _8;
  _9 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  this->_inBuffSearchPtr = _9;
  goto <D.41458>;
  <D.41457>:
  _10 = &this->_bz2Header;
  _11 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  D.41468 = std::__cxx11::basic_string<unsigned char>::compare (_10, 0, prefixLen, _11, prefixLen);
  _12 = D.41468;
  retval.34 = _12 == 0;
  if (retval.34 != 0) goto <D.41460>; else goto <D.41461>;
  <D.41460>:
  goto <D.41462>;
  <D.41461>:
  _13 = this->_errState;
  _14 = _13 | 32;
  this->_errState = _14;
  _15 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  this->_inBuffSearchPtr = _15;
  <D.41462>:
  <D.41458>:
  goto <D.41463>;
  <D.41449>:
  prefixLen.35_16 = (long int) prefixLen;
  _17 = &this->_bz2Header;
  D.36547 = std::__cxx11::basic_string<unsigned char>::begin (_17);
  D.41464 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::operator+ (&D.36547, prefixLen.35_16);
  _18 = &this->_bz2Header;
  D.41465 = std::__cxx11::basic_string<unsigned char>::begin (_18);
  _19 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  _20 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  D.41469 = std::search<unsigned char*, __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > (_20, _19, D.41465, D.41464);
  _21 = D.41469;
  this->_inBuffSearchPtr = _21;
  D.36547 = {CLOBBER};
  <D.41463>:
  D.41466 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  goto <D.41467>;
  <D.41467>:
  return D.41466;
  <D.41470>:
  D.36547 = {CLOBBER};
  resx 1
}



;; Function int std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::compare(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE7compareEmmPKhm, funcdef_no=2188, decl_uid=30604, cgraph_uid=930, symbol_order=933)

int std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::compare(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const _CharT*, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this, size_type __pos, size_type __n1, const unsigned char * __s, size_type __n2)
{
  int D.41476;
  int __r;
  const size_type __len;
  int D.41474;

  std::__cxx11::basic_string<unsigned char>::_M_check (this, __pos, "basic_string::compare");
  __n1.36_1 = __n1;
  _2 = std::__cxx11::basic_string<unsigned char>::_M_limit (this, __pos, __n1.36_1);
  __n1 = _2;
  _3 = std::min<long unsigned int> (&__n1, &__n2);
  __len = *_3;
  _4 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  _5 = _4 + __pos;
  D.41476 = __gnu_cxx::char_traits<unsigned char>::compare (_5, __s, __len);
  __r = D.41476;
  if (__r == 0) goto <D.41471>; else goto <D.41472>;
  <D.41471>:
  __n2.37_6 = __n2;
  __n1.38_7 = __n1;
  __r = std::__cxx11::basic_string<unsigned char>::_S_compare (__n1.38_7, __n2.37_6);
  goto <D.41473>;
  <D.41472>:
  <D.41473>:
  D.41474 = __r;
  goto <D.41475>;
  <D.41475>:
  return D.41474;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_check(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const char*) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE8_M_checkEmPKc, funcdef_no=2312, decl_uid=29917, cgraph_uid=1053, symbol_order=1056)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_check(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, const char*) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this, size_type __pos, const char * __s)
{
  size_type D.41480;
  bool retval.39;

  _1 = std::__cxx11::basic_string<unsigned char>::size (this);
  retval.39 = __pos > _1;
  if (retval.39 != 0) goto <D.41478>; else goto <D.41479>;
  <D.41478>:
  _2 = std::__cxx11::basic_string<unsigned char>::size (this);
  std::__throw_out_of_range_fmt ("%s: __pos (which is %zu) > this->size() (which is %zu)", __s, __pos, _2);
  <D.41479>:
  D.41480 = __pos;
  goto <D.41481>;
  <D.41481>:
  return D.41480;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_limit(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE8_M_limitEmm, funcdef_no=2313, decl_uid=29926, cgraph_uid=1054, symbol_order=1057)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_M_limit(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) const [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (const struct basic_string * const this, size_type __pos, size_type __off)
{
  const bool __testoff;
  size_type iftmp.40;
  size_type D.41482;

  _1 = std::__cxx11::basic_string<unsigned char>::size (this);
  _2 = _1 - __pos;
  __testoff = __off < _2;
  if (__testoff != 0) goto <D.41484>; else goto <D.41485>;
  <D.41484>:
  iftmp.40 = __off;
  goto <D.41486>;
  <D.41485>:
  _3 = std::__cxx11::basic_string<unsigned char>::size (this);
  iftmp.40 = _3 - __pos;
  <D.41486>:
  D.41482 = iftmp.40;
  goto <D.41487>;
  <D.41487>:
  return D.41482;
}



;; Function constexpr const _Tp& std::min(const _Tp&, const _Tp&) [with _Tp = long unsigned int] (_ZSt3minImERKT_S2_S2_, funcdef_no=2189, decl_uid=8982, cgraph_uid=929, symbol_order=932)

constexpr const _Tp& std::min(const _Tp&, const _Tp&) [with _Tp = long unsigned int] (const long unsigned int & __a, const long unsigned int & __b)
{
  const long unsigned int & D.41490;

  _1 = *__b;
  _2 = *__a;
  if (_1 < _2) goto <D.41488>; else goto <D.41489>;
  <D.41488>:
  D.41490 = __b;
  goto <D.41491>;
  <D.41489>:
  D.41490 = __a;
  goto <D.41491>;
  <D.41491>:
  return D.41490;
}



;; Function static constexpr int __gnu_cxx::char_traits<_CharT>::compare(const char_type*, const char_type*, std::size_t) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE7compareEPKhS3_m, funcdef_no=2314, decl_uid=29554, cgraph_uid=1056, symbol_order=1059)

static constexpr int __gnu_cxx::char_traits<_CharT>::compare(const char_type*, const char_type*, std::size_t) [with _CharT = unsigned char] (const char_type * __s1, const char_type * __s2, size_t __n)
{
  size_t __i;
  bool retval.42;
  int D.41497;
  bool retval.41;

  __i = 0;
  <D.41492>:
  if (__i >= __n) goto <D.38970>; else goto <D.41493>;
  <D.41493>:
  _1 = __s2 + __i;
  _2 = __s1 + __i;
  retval.41 = __gnu_cxx::char_traits<unsigned char>::lt (_2, _1);
  if (retval.41 != 0) goto <D.41495>; else goto <D.41496>;
  <D.41495>:
  D.41497 = -1;
  goto <D.41501>;
  <D.41496>:
  _3 = __s1 + __i;
  _4 = __s2 + __i;
  retval.42 = __gnu_cxx::char_traits<unsigned char>::lt (_4, _3);
  if (retval.42 != 0) goto <D.41499>; else goto <D.41500>;
  <D.41499>:
  D.41497 = 1;
  goto <D.41501>;
  <D.41500>:
  __i = __i + 1;
  goto <D.41492>;
  <D.38970>:
  D.41497 = 0;
  goto <D.41501>;
  <D.41501>:
  return D.41497;
}



;; Function static constexpr bool __gnu_cxx::char_traits<_CharT>::lt(const char_type&, const char_type&) [with _CharT = unsigned char] (_ZN9__gnu_cxx11char_traitsIhE2ltERKhS3_, funcdef_no=2315, decl_uid=29551, cgraph_uid=1055, symbol_order=1058)

static constexpr bool __gnu_cxx::char_traits<_CharT>::lt(const char_type&, const char_type&) [with _CharT = unsigned char] (const char_type & __c1, const char_type & __c2)
{
  bool D.41502;

  _1 = *__c1;
  _2 = *__c2;
  D.41502 = _1 < _2;
  goto <D.41503>;
  <D.41503>:
  return D.41502;
}



;; Function static int std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_compare(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE10_S_compareEmm, funcdef_no=2317, decl_uid=29973, cgraph_uid=1058, symbol_order=1061)

static int std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::_S_compare(std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type, std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (size_type __n1, size_type __n2)
{
  const difference_type __d;
  int D.41506;

  _1 = __n1 - __n2;
  __d = (const difference_type) _1;
  if (__d > 2147483647) goto <D.41504>; else goto <D.41505>;
  <D.41504>:
  D.41506 = 2147483647;
  goto <D.41509>;
  <D.41505>:
  if (__d < -2147483648) goto <D.41507>; else goto <D.41508>;
  <D.41507>:
  D.41506 = -2147483648;
  goto <D.41509>;
  <D.41508>:
  D.41506 = (int) __d;
  goto <D.41509>;
  <D.41509>:
  return D.41506;
}



;; Function __gnu_cxx::__normal_iterator<_Iterator, _Container> __gnu_cxx::__normal_iterator<_Iterator, _Container>::operator+(__gnu_cxx::__normal_iterator<_Iterator, _Container>::difference_type) const [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (_ZNK9__gnu_cxx17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEplEl, funcdef_no=2191, decl_uid=36507, cgraph_uid=932, symbol_order=935)

__gnu_cxx::__normal_iterator<_Iterator, _Container> __gnu_cxx::__normal_iterator<_Iterator, _Container>::operator+(__gnu_cxx::__normal_iterator<_Iterator, _Container>::difference_type) const [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (const struct __normal_iterator * const this, difference_type __n)
{
  unsigned char * const D.38497;
  struct __normal_iterator D.38498;
  struct __normal_iterator D.41510;

  _1 = this->_M_current;
  __n.43_2 = (sizetype) __n;
  _3 = _1 + __n.43_2;
  D.38497 = _3;
  __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::__normal_iterator (&D.38498, &D.38497);
  D.41510 = D.38498;
  goto <D.41512>;
  <D.41512>:
  D.38498 = {CLOBBER};
  goto <D.41513>;
  <D.41513>:
  D.38497 = {CLOBBER};
  goto <D.41511>;
  <D.41511>:
  return D.41510;
}



;; Function __gnu_cxx::__normal_iterator<_Iterator, _Container>::__normal_iterator(const _Iterator&) [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (_ZN9__gnu_cxx17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEC2ERKS1_, funcdef_no=2319, decl_uid=36529, cgraph_uid=1060, symbol_order=1063)

__gnu_cxx::__normal_iterator<_Iterator, _Container>::__normal_iterator(const _Iterator&) [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (struct __normal_iterator * const this, unsigned char * const & __i)
{
  MEM[(struct  &)this] = {CLOBBER};
  _1 = *__i;
  this->_M_current = _1;
  return;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::iterator std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::begin() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE5beginEv, funcdef_no=2190, decl_uid=30088, cgraph_uid=931, symbol_order=934)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::iterator std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::begin() [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this)
{
  unsigned char * const D.38494;
  struct iterator D.38495;
  struct iterator D.41514;

  _1 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  D.38494 = _1;
  __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::__normal_iterator (&D.38495, &D.38494);
  D.41514 = D.38495;
  goto <D.41516>;
  <D.41516>:
  D.38495 = {CLOBBER};
  goto <D.41517>;
  <D.41517>:
  D.38494 = {CLOBBER};
  goto <D.41515>;
  <D.41515>:
  return D.41514;
}



;; Function _FIter1 std::search(_FIter1, _FIter1, _FIter2, _FIter2) [with _FIter1 = unsigned char*; _FIter2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (_ZSt6searchIPhN9__gnu_cxx17__normal_iteratorIS0_NSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEET_SA_SA_T0_SB_, funcdef_no=2192, decl_uid=36549, cgraph_uid=933, symbol_order=936)

_FIter1 std::search(_FIter1, _FIter1, _FIter2, _FIter2) [with _FIter1 = unsigned char*; _FIter2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (unsigned char * __first1, unsigned char * __last1, struct __normal_iterator __first2, struct __normal_iterator __last2)
{
  unsigned char * D.41521;
  struct _Iter_equal_to_iter D.41519;
  struct _Iter_equal_to_iter D.38500;
  unsigned char * D.41518;

  __gnu_cxx::__ops::__iter_equal_to_iter ();
  D.41521 = std::__search<unsigned char*, __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >, __gnu_cxx::__ops::_Iter_equal_to_iter> (__first1, __last1, __first2, __last2, D.41519);
  D.41518 = D.41521;
  goto <D.41523>;
  <D.41523>:
  D.38500 = {CLOBBER};
  goto <D.41520>;
  <D.41520>:
  return D.41518;
  <D.41522>:
  D.38500 = {CLOBBER};
  resx 1
}



;; Function __gnu_cxx::__ops::_Iter_equal_to_iter __gnu_cxx::__ops::__iter_equal_to_iter() (_ZN9__gnu_cxx5__ops20__iter_equal_to_iterEv, funcdef_no=205, decl_uid=8057, cgraph_uid=12, symbol_order=13)

__gnu_cxx::__ops::_Iter_equal_to_iter __gnu_cxx::__ops::__iter_equal_to_iter() ()
{
  struct _Iter_equal_to_iter D.41524;

  goto <D.41525>;
  <D.41525>:
  return D.41524;
}



;; Function _ForwardIterator1 std::__search(_ForwardIterator1, _ForwardIterator1, _ForwardIterator2, _ForwardIterator2, _BinaryPredicate) [with _ForwardIterator1 = unsigned char*; _ForwardIterator2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >; _BinaryPredicate = __gnu_cxx::__ops::_Iter_equal_to_iter] (_ZSt8__searchIPhN9__gnu_cxx17__normal_iteratorIS0_NSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEENS1_5__ops19_Iter_equal_to_iterEET_SC_SC_T0_SD_T1_, funcdef_no=2321, decl_uid=38501, cgraph_uid=1065, symbol_order=1068)

_ForwardIterator1 std::__search(_ForwardIterator1, _ForwardIterator1, _ForwardIterator2, _ForwardIterator2, _BinaryPredicate) [with _ForwardIterator1 = unsigned char*; _ForwardIterator2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >; _BinaryPredicate = __gnu_cxx::__ops::_Iter_equal_to_iter] (unsigned char * __first1, unsigned char * __last1, struct __normal_iterator __first2, struct __normal_iterator __last2, struct _Iter_equal_to_iter __predicate)
{
  unsigned char * D.41558;
  unsigned char * D.41557;
  unsigned char * __current;
  struct __normal_iterator __p;
  struct __normal_iterator __p1;
  bool retval.50;
  bool retval.49;
  bool retval.48;
  bool retval.47;
  struct _Iter_equals_iter D.41541;
  struct _Iter_equal_to_iter D.39052;
  struct _Iter_equals_iter D.41539;
  struct _Iter_equal_to_iter D.41538;
  struct _Iter_equal_to_iter D.38990;
  bool retval.46;
  unsigned char * D.41534;
  bool iftmp.45;
  bool retval.44;

  if (__first1 == __last1) goto <D.41528>; else goto <D.41531>;
  <D.41531>:
  _1 = __gnu_cxx::operator==<unsigned char*, std::__cxx11::basic_string<unsigned char> > (&__first2, &__last2);
  if (_1 != 0) goto <D.41528>; else goto <D.41529>;
  <D.41528>:
  iftmp.45 = 1;
  goto <D.41530>;
  <D.41529>:
  iftmp.45 = 0;
  <D.41530>:
  retval.44 = iftmp.45;
  if (retval.44 != 0) goto <D.41532>; else goto <D.41533>;
  <D.41532>:
  D.41534 = __first1;
  goto <D.41560>;
  <D.41533>:
  __p1 = __first2;
  _2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::operator++ (&__p1);
  retval.46 = __gnu_cxx::operator==<unsigned char*, std::__cxx11::basic_string<unsigned char> > (_2, &__last2);
  if (retval.46 != 0) goto <D.41536>; else goto <D.41537>;
  <D.41536>:
  D.41539 = __gnu_cxx::__ops::__iter_comp_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > (D.41538, __first2);
  D.41557 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > > (__first1, __last1, D.41539);
  D.41534 = D.41557;
  goto <D.41560>;
  <D.41537>:
  __p._M_current = 0B;
  __current = __first1;
  <D.41540>:
  D.41541 = __gnu_cxx::__ops::__iter_comp_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > (D.41538, __first2);
  D.41558 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > > (__first1, __last1, D.41541);
  __first1 = D.41558;
  if (__first1 == __last1) goto <D.41542>; else goto <D.41543>;
  <D.41542>:
  D.41534 = __last1;
  goto <D.41560>;
  <D.41543>:
  __p = __p1;
  __current = __first1;
  __current = __current + 1;
  retval.47 = __current == __last1;
  if (retval.47 != 0) goto <D.41545>; else goto <D.41546>;
  <D.41545>:
  D.41534 = __last1;
  goto <D.41560>;
  <D.41546>:
  <D.41547>:
  retval.48 = __gnu_cxx::__ops::_Iter_equal_to_iter::operator()<unsigned char*, __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > (&__predicate, __current, __p);
  if (retval.48 != 0) goto <D.41549>; else goto <D.39069>;
  <D.41549>:
  _3 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::operator++ (&__p);
  retval.49 = __gnu_cxx::operator==<unsigned char*, std::__cxx11::basic_string<unsigned char> > (_3, &__last2);
  if (retval.49 != 0) goto <D.41551>; else goto <D.41552>;
  <D.41551>:
  D.41534 = __first1;
  goto <D.41560>;
  <D.41552>:
  __current = __current + 1;
  retval.50 = __current == __last1;
  if (retval.50 != 0) goto <D.41554>; else goto <D.41555>;
  <D.41554>:
  D.41534 = __last1;
  goto <D.41560>;
  <D.41555>:
  goto <D.41547>;
  <D.39069>:
  __first1 = __first1 + 1;
  goto <D.41540>;
  D.41534 = __first1;
  goto <D.41560>;
  <D.41560>:
  __p1 = {CLOBBER};
  __p = {CLOBBER};
  goto <D.41556>;
  <D.41556>:
  return D.41534;
  <D.41559>:
  __p1 = {CLOBBER};
  __p = {CLOBBER};
  resx 1
}



;; Function __gnu_cxx::__ops::_Iter_equals_iter<_Iterator> __gnu_cxx::__ops::__iter_comp_iter(__gnu_cxx::__ops::_Iter_equal_to_iter, _Iterator) [with _Iterator = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (_ZN9__gnu_cxx5__ops16__iter_comp_iterINS_17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEENS0_17_Iter_equals_iterIT_EENS0_19_Iter_equal_to_iterESC_, funcdef_no=2374, decl_uid=38987, cgraph_uid=1115, symbol_order=1118)

__gnu_cxx::__ops::_Iter_equals_iter<_Iterator> __gnu_cxx::__ops::__iter_comp_iter(__gnu_cxx::__ops::_Iter_equal_to_iter, _Iterator) [with _Iterator = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (struct _Iter_equal_to_iter D.38988, struct __normal_iterator __it)
{
  struct _Iter_equals_iter D.39579;
  struct _Iter_equals_iter D.41561;

  __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::_Iter_equals_iter (&D.39579, __it);
  D.41561 = D.39579;
  goto <D.41563>;
  <D.41563>:
  D.39579 = {CLOBBER};
  goto <D.41562>;
  <D.41562>:
  return D.41561;
}



;; Function __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::_Iter_equals_iter(_Iterator1) [with _Iterator1 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (_ZN9__gnu_cxx5__ops17_Iter_equals_iterINS_17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEC2ESA_, funcdef_no=2429, decl_uid=39010, cgraph_uid=1170, symbol_order=1173)

__gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::_Iter_equals_iter(_Iterator1) [with _Iterator1 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (struct _Iter_equals_iter * const this, struct __normal_iterator __it1)
{
  MEM[(struct  &)this] = {CLOBBER};
  this->_M_it1 = __it1;
  return;
}



;; Function _Iterator std::__find_if(_Iterator, _Iterator, _Predicate) [with _Iterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >] (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterINS1_17__normal_iteratorIS0_NSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEEET_SD_SD_T0_, funcdef_no=2375, decl_uid=39024, cgraph_uid=1117, symbol_order=1120)

_Iterator std::__find_if(_Iterator, _Iterator, _Predicate) [with _Iterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >] (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred)
{
  unsigned char * D.41567;
  struct random_access_iterator_tag D.41565;
  struct iterator_category D.39583;
  unsigned char * D.41564;

  __first.51_1 = __first;
  D.41567 = std::__find_if<unsigned char*, __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > > > (__first.51_1, __last, __pred, D.41565);
  D.41564 = D.41567;
  goto <D.41569>;
  <D.41569>:
  D.39583 = {CLOBBER};
  goto <D.41566>;
  <D.41566>:
  return D.41564;
  <D.41568>:
  D.39583 = {CLOBBER};
  resx 1
}



;; Function _RandomAccessIterator std::__find_if(_RandomAccessIterator, _RandomAccessIterator, _Predicate, std::random_access_iterator_tag) [with _RandomAccessIterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >] (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterINS1_17__normal_iteratorIS0_NSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEEET_SD_SD_T0_St26random_access_iterator_tag, funcdef_no=2431, decl_uid=39584, cgraph_uid=1172, symbol_order=1175)

_RandomAccessIterator std::__find_if(_RandomAccessIterator, _RandomAccessIterator, _Predicate, std::random_access_iterator_tag) [with _RandomAccessIterator = unsigned char*; _Predicate = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >] (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred, struct random_access_iterator_tag D.39588)
{
  difference_type __trip_count;
  bool retval.62;
  bool retval.61;
  bool retval.60;
  bool retval.57;
  bool retval.56;
  bool retval.55;
  unsigned char * D.41575;
  bool retval.54;

  __last.52_1 = (long int) __last;
  __first.53_2 = (long int) __first;
  _3 = __last.52_1 - __first.53_2;
  __trip_count = _3 >> 2;
  <D.41570>:
  if (__trip_count <= 0) goto <D.40873>; else goto <D.41571>;
  <D.41571>:
  retval.54 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.54 != 0) goto <D.41573>; else goto <D.41574>;
  <D.41573>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41574>:
  __first = __first + 1;
  retval.55 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.55 != 0) goto <D.41577>; else goto <D.41578>;
  <D.41577>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41578>:
  __first = __first + 1;
  retval.56 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.56 != 0) goto <D.41580>; else goto <D.41581>;
  <D.41580>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41581>:
  __first = __first + 1;
  retval.57 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.57 != 0) goto <D.41583>; else goto <D.41584>;
  <D.41583>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41584>:
  __first = __first + 1;
  __trip_count = __trip_count + -1;
  goto <D.41570>;
  <D.40873>:
  __last.58_4 = (long int) __last;
  __first.59_5 = (long int) __first;
  _6 = __last.58_4 - __first.59_5;
  switch (_6) <default: <D.40872>, case 0: <D.40871>, case 1: <D.40870>, case 2: <D.40869>, case 3: <D.40868>>
  <D.40868>:
  retval.60 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.60 != 0) goto <D.41586>; else goto <D.41587>;
  <D.41586>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41587>:
  __first = __first + 1;
  <D.40869>:
  retval.61 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.61 != 0) goto <D.41589>; else goto <D.41590>;
  <D.41589>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41590>:
  __first = __first + 1;
  <D.40870>:
  retval.62 = __gnu_cxx::__ops::_Iter_equals_iter<__gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> > >::operator()<unsigned char*> (&__pred, __first);
  if (retval.62 != 0) goto <D.41592>; else goto <D.41593>;
  <D.41592>:
  D.41575 = __first;
  goto <D.41594>;
  <D.41593>:
  __first = __first + 1;
  <D.40871>:
  <D.40872>:
  D.41575 = __last;
  goto <D.41594>;
  <D.41594>:
  return D.41575;
}



;; Function bool __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::operator()(_Iterator2) [with _Iterator2 = unsigned char*; _Iterator1 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (_ZN9__gnu_cxx5__ops17_Iter_equals_iterINS_17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEclIS3_EEbT_, funcdef_no=2479, decl_uid=40865, cgraph_uid=1220, symbol_order=1223)

bool __gnu_cxx::__ops::_Iter_equals_iter<_Iterator1>::operator()(_Iterator2) [with _Iterator2 = unsigned char*; _Iterator1 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (struct _Iter_equals_iter * const this, unsigned char * __it2)
{
  bool D.41595;

  _1 = *__it2;
  _2 = &this->_M_it1;
  _3 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::operator* (_2);
  _4 = *_3;
  D.41595 = _1 == _4;
  goto <D.41596>;
  <D.41596>:
  return D.41595;
}



;; Function __gnu_cxx::__normal_iterator<_Iterator, _Container>::reference __gnu_cxx::__normal_iterator<_Iterator, _Container>::operator*() const [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (_ZNK9__gnu_cxx17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEdeEv, funcdef_no=2432, decl_uid=36487, cgraph_uid=1173, symbol_order=1176)

__gnu_cxx::__normal_iterator<_Iterator, _Container>::reference __gnu_cxx::__normal_iterator<_Iterator, _Container>::operator*() const [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (const struct __normal_iterator * const this)
{
  unsigned char & D.41597;

  D.41597 = this->_M_current;
  goto <D.41598>;
  <D.41598>:
  return D.41597;
}



;; Function bool __gnu_cxx::__ops::_Iter_equal_to_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = unsigned char*; _Iterator2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (_ZNK9__gnu_cxx5__ops19_Iter_equal_to_iterclIPhNS_17__normal_iteratorIS3_NSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEEbT_T0_, funcdef_no=2377, decl_uid=39062, cgraph_uid=1118, symbol_order=1121)

bool __gnu_cxx::__ops::_Iter_equal_to_iter::operator()(_Iterator1, _Iterator2) const [with _Iterator1 = unsigned char*; _Iterator2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >] (const struct _Iter_equal_to_iter * const this, unsigned char * __it1, struct __normal_iterator __it2)
{
  bool D.41599;

  _1 = *__it1;
  _2 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::operator* (&__it2);
  _3 = *_2;
  D.41599 = _1 == _3;
  goto <D.41600>;
  <D.41600>:
  return D.41599;
}



;; Function __gnu_cxx::__normal_iterator<_Iterator, _Container>& __gnu_cxx::__normal_iterator<_Iterator, _Container>::operator++() [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (_ZN9__gnu_cxx17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEppEv, funcdef_no=2373, decl_uid=36491, cgraph_uid=1114, symbol_order=1117)

__gnu_cxx::__normal_iterator<_Iterator, _Container>& __gnu_cxx::__normal_iterator<_Iterator, _Container>::operator++() [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (struct __normal_iterator * const this)
{
  struct __normal_iterator & D.41601;

  _1 = this->_M_current;
  _2 = _1 + 1;
  this->_M_current = _2;
  D.41601 = this;
  goto <D.41602>;
  <D.41602>:
  return D.41601;
}



;; Function bool __gnu_cxx::operator==(const __gnu_cxx::__normal_iterator<_Iterator, _Container>&, const __gnu_cxx::__normal_iterator<_Iterator, _Container>&) [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (_ZN9__gnu_cxxeqIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEbRKNS_17__normal_iteratorIT_T0_EESD_, funcdef_no=2372, decl_uid=38982, cgraph_uid=1113, symbol_order=1116)

bool __gnu_cxx::operator==(const __gnu_cxx::__normal_iterator<_Iterator, _Container>&, const __gnu_cxx::__normal_iterator<_Iterator, _Container>&) [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (const struct __normal_iterator & __lhs, const struct __normal_iterator & __rhs)
{
  bool D.41603;

  _1 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::base (__lhs);
  _2 = *_1;
  _3 = __gnu_cxx::__normal_iterator<unsigned char*, std::__cxx11::basic_string<unsigned char> >::base (__rhs);
  _4 = *_3;
  D.41603 = _2 == _4;
  goto <D.41604>;
  <D.41604>:
  return D.41603;
}



;; Function const _Iterator& __gnu_cxx::__normal_iterator<_Iterator, _Container>::base() const [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (_ZNK9__gnu_cxx17__normal_iteratorIPhNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEE4baseEv, funcdef_no=2427, decl_uid=36516, cgraph_uid=1168, symbol_order=1171)

const _Iterator& __gnu_cxx::__normal_iterator<_Iterator, _Container>::base() const [with _Iterator = unsigned char*; _Container = std::__cxx11::basic_string<unsigned char>] (const struct __normal_iterator * const this)
{
  unsigned char * const & D.41605;

  D.41605 = &this->_M_current;
  goto <D.41606>;
  <D.41606>:
  return D.41605;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::searchNextHeaderInBuff() (_ZN6pbzip216BZ2StreamScanner22searchNextHeaderInBuffEv, funcdef_no=1938, decl_uid=29369, cgraph_uid=679, symbol_order=682)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::searchNextHeaderInBuff() (struct BZ2StreamScanner * const this)
{
  int D.41639;
  value_type & D.41638;
  value_type & D.41637;
  value_type & D.41636;
  int cmpres;
  CharType bwtSizeChar;
  struct basic_string * pHdr;
  size_t hsp;
  size_t prefixLen;
  bool retval.67;
  CharType * D.41629;
  bool retval.66;
  bool retval.65;
  bool iftmp.64;
  bool retval.63;

  this->_searchStatus = 0;
  prefixLen = 3;
  hsp = prefixLen + 1;
  pbzip2::BZ2StreamScanner::locateHeaderPrefixInBuff (this);
  <D.41607>:
  _1 = pbzip2::BZ2StreamScanner::failed (this);
  _2 = ~_1;
  if (_2 != 0) goto <D.41612>; else goto <D.41610>;
  <D.41612>:
  _3 = pbzip2::BZ2StreamScanner::getUnsearchedCount (this);
  _4 = pbzip2::BZ2StreamScanner::getHeaderSize (this);
  if (_3 >= _4) goto <D.41613>; else goto <D.41610>;
  <D.41613>:
  iftmp.64 = 1;
  goto <D.41611>;
  <D.41610>:
  iftmp.64 = 0;
  <D.41611>:
  retval.63 = iftmp.64;
  if (retval.63 != 0) goto <D.41614>; else goto <D.36599>;
  <D.41614>:
  pHdr = 0B;
  _5 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _6 = _5 + hsp;
  _7 = *_6;
  _8 = &this->_bz2Header;
  D.41636 = std::__cxx11::basic_string<unsigned char>::operator[] (_8, hsp);
  _9 = D.41636;
  _10 = *_9;
  retval.65 = _7 == _10;
  if (retval.65 != 0) goto <D.41616>; else goto <D.41617>;
  <D.41616>:
  pHdr = &this->_bz2Header;
  goto <D.41618>;
  <D.41617>:
  _11 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _12 = _11 + hsp;
  _13 = *_12;
  _14 = &this->_bz2HeaderZero;
  D.41637 = std::__cxx11::basic_string<unsigned char>::operator[] (_14, hsp);
  _15 = D.41637;
  _16 = *_15;
  retval.66 = _13 == _16;
  if (retval.66 != 0) goto <D.41620>; else goto <D.41621>;
  <D.41620>:
  pHdr = &this->_bz2HeaderZero;
  goto <D.41622>;
  <D.41621>:
  <D.41622>:
  <D.41618>:
  if (pHdr != 0B) goto <D.41623>; else goto <D.41624>;
  <D.41623>:
  _17 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _18 = _17 + prefixLen;
  bwtSizeChar = *_18;
  _19 = bwtSizeChar + 207;
  if (_19 <= 8) goto <D.41625>; else goto <D.41626>;
  <D.41625>:
  D.41638 = std::__cxx11::basic_string<unsigned char>::operator[] (pHdr, prefixLen);
  _20 = D.41638;
  *_20 = bwtSizeChar;
  _21 = std::__cxx11::basic_string<unsigned char>::size (pHdr);
  _22 = _21 - hsp;
  _23 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _24 = _23 + hsp;
  _25 = std::__cxx11::basic_string<unsigned char>::size (pHdr);
  _26 = _25 - hsp;
  D.41639 = std::__cxx11::basic_string<unsigned char>::compare (pHdr, hsp, _26, _24, _22);
  cmpres = D.41639;
  if (cmpres == 0) goto <D.41627>; else goto <D.41628>;
  <D.41627>:
  this->_searchStatus = 1;
  D.41629 = this->_inBuffSearchPtr;
  goto <D.41635>;
  <D.41628>:
  goto <D.41630>;
  <D.41626>:
  <D.41630>:
  goto <D.41631>;
  <D.41624>:
  <D.41631>:
  _27 = pbzip2::BZ2StreamScanner::isBz2HeaderFound (this);
  retval.67 = ~_27;
  if (retval.67 != 0) goto <D.41633>; else goto <D.41634>;
  <D.41633>:
  _28 = this->_errState;
  _29 = _28 | 32;
  this->_errState = _29;
  goto <D.36599>;
  <D.41634>:
  _30 = this->_inBuffSearchPtr;
  _31 = _30 + prefixLen;
  this->_inBuffSearchPtr = _31;
  pbzip2::BZ2StreamScanner::locateHeaderPrefixInBuff (this);
  goto <D.41607>;
  <D.36599>:
  _32 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  this->_inBuffSearchPtr = _32;
  D.41629 = this->_inBuffSearchPtr;
  goto <D.41635>;
  <D.41635>:
  return D.41629;
}



;; Function size_t pbzip2::BZ2StreamScanner::getUnsearchedCount() const (_ZNK6pbzip216BZ2StreamScanner18getUnsearchedCountEv, funcdef_no=1445, decl_uid=29365, cgraph_uid=454, symbol_order=456)

size_t pbzip2::BZ2StreamScanner::getUnsearchedCount() const (const struct BZ2StreamScanner * const this)
{
  size_t D.41640;

  _1 = this->_inBuffEnd;
  _2 = (long int) _1;
  _3 = this->_inBuffSearchPtr;
  _4 = (long int) _3;
  _5 = _2 - _4;
  D.41640 = (size_t) _5;
  goto <D.41641>;
  <D.41641>:
  return D.41640;
}



;; Function std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::reference std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator[](std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEixEm, funcdef_no=2193, decl_uid=30139, cgraph_uid=934, symbol_order=937)

std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::reference std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::operator[](std::__cxx11::basic_string<_CharT, _Traits, _Alloc>::size_type) [with _CharT = unsigned char; _Traits = std::char_traits<unsigned char>; _Alloc = std::allocator<unsigned char>] (struct basic_string * const this, size_type __pos)
{
  value_type & D.41642;

  _1 = std::__cxx11::basic_string<unsigned char>::_M_data (this);
  D.41642 = _1 + __pos;
  goto <D.41643>;
  <D.41643>:
  return D.41642;
}



;; Function pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::searchNextHeader() (_ZN6pbzip216BZ2StreamScanner16searchNextHeaderEv, funcdef_no=1939, decl_uid=29367, cgraph_uid=680, symbol_order=683)

pbzip2::BZ2StreamScanner::CharType* pbzip2::BZ2StreamScanner::searchNextHeader() (struct BZ2StreamScanner * const this)
{
  bool D.41670;
  bool retval.73;
  bool retval.72;
  CharType * D.41661;
  bool retval.71;
  bool iftmp.70;
  bool retval.69;
  bool retval.68;

  _1 = pbzip2::BZ2StreamScanner::getUnsearchedCount (this);
  retval.68 = _1 != 0;
  if (retval.68 != 0) goto <D.41645>; else goto <D.41646>;
  <D.41645>:
  pbzip2::BZ2StreamScanner::searchNextHeaderInBuff (this);
  goto <D.41647>;
  <D.41646>:
  <D.41647>:
  <D.41648>:
  _2 = pbzip2::BZ2StreamScanner::getSearchStatus (this);
  _3 = ~_2;
  if (_3 != 0) goto <D.41653>; else goto <D.41651>;
  <D.41653>:
  _4 = pbzip2::BZ2StreamScanner::eof (this);
  _5 = ~_4;
  if (_5 != 0) goto <D.41654>; else goto <D.41651>;
  <D.41654>:
  _6 = pbzip2::BZ2StreamScanner::failed (this);
  _7 = ~_6;
  if (_7 != 0) goto <D.41655>; else goto <D.41651>;
  <D.41655>:
  D.41670 = pbzip2::BZ2StreamScanner::isOutBuffFullEnough (this);
  _8 = D.41670;
  _9 = ~_8;
  if (_9 != 0) goto <D.41656>; else goto <D.41651>;
  <D.41656>:
  iftmp.70 = 1;
  goto <D.41652>;
  <D.41651>:
  iftmp.70 = 0;
  <D.41652>:
  retval.69 = iftmp.70;
  if (retval.69 != 0) goto <D.41657>; else goto <D.36604>;
  <D.41657>:
  pbzip2::BZ2StreamScanner::readData (this);
  retval.71 = pbzip2::BZ2StreamScanner::failed (this);
  if (retval.71 != 0) goto <D.41659>; else goto <D.41660>;
  <D.41659>:
  D.41661 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  goto <D.41669>;
  <D.41660>:
  pbzip2::BZ2StreamScanner::searchNextHeaderInBuff (this);
  goto <D.41648>;
  <D.36604>:
  retval.72 = pbzip2::BZ2StreamScanner::getSearchStatus (this);
  if (retval.72 != 0) goto <D.41663>; else goto <D.41664>;
  <D.41663>:
  this->_bz2HeaderFound = 1;
  goto <D.41665>;
  <D.41664>:
  <D.41665>:
  retval.73 = pbzip2::BZ2StreamScanner::failed (this);
  if (retval.73 != 0) goto <D.41667>; else goto <D.41668>;
  <D.41667>:
  _10 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  this->_inBuffSearchPtr = _10;
  D.41661 = this->_inBuffSearchPtr;
  goto <D.41669>;
  <D.41668>:
  D.41661 = this->_inBuffSearchPtr;
  goto <D.41669>;
  <D.41669>:
  return D.41661;
}



;; Function bool pbzip2::BZ2StreamScanner::isOutBuffFullEnough() const (_ZNK6pbzip216BZ2StreamScanner19isOutBuffFullEnoughEv, funcdef_no=1435, decl_uid=29331, cgraph_uid=444, symbol_order=446)

bool pbzip2::BZ2StreamScanner::isOutBuffFullEnough() const (const struct BZ2StreamScanner * const this)
{
  bool D.41671;

  _1 = this->_outBuff.bufSize;
  _2 = (long unsigned int) _1;
  _3 = pbzip2::BZ2StreamScanner::getOutBuffCapacityLimit (this);
  D.41671 = _2 >= _3;
  goto <D.41672>;
  <D.41672>:
  return D.41671;
}



;; Function bool pbzip2::BZ2StreamScanner::getSearchStatus() const (_ZNK6pbzip216BZ2StreamScanner15getSearchStatusEv, funcdef_no=1433, decl_uid=29327, cgraph_uid=442, symbol_order=444)

bool pbzip2::BZ2StreamScanner::getSearchStatus() const (const struct BZ2StreamScanner * const this)
{
  bool D.41673;

  D.41673 = this->_searchStatus;
  goto <D.41674>;
  <D.41674>:
  return D.41673;
}



;; Function outBuff* pbzip2::BZ2StreamScanner::getNextStream() (_ZN6pbzip216BZ2StreamScanner13getNextStreamEv, funcdef_no=1940, decl_uid=29308, cgraph_uid=681, symbol_order=684)

outBuff* pbzip2::BZ2StreamScanner::getNextStream() (struct BZ2StreamScanner * const this)
{
  struct outBuff * res;
  bool retval.85;
  bool retval.84;
  bool retval.83;
  bool retval.82;
  unsigned char * const D.36611;
  unsigned char * const D.36612;
  bool iftmp.81;
  bool retval.80;
  bool iftmp.79;
  bool retval.78;
  bool retval.77;
  bool iftmp.76;
  bool retval.75;
  struct outBuff * D.41681;
  struct outBuff * iftmp.74;
  void * D.36610;

  pbzip2::BZ2StreamScanner::initOutBuff (this, 0B, 0, 0);
  D.36610 = operator new (40, &nothrow);
  if (D.36610 != 0B) goto <D.41676>; else goto <D.41677>;
  <D.41676>:
  outBuff::outBuff (D.36610, 0B, 0, 0, 0, 0, 1, 0B);
  iftmp.74 = D.36610;
  goto <D.41678>;
  <D.41677>:
  iftmp.74 = D.36610;
  <D.41678>:
  res = iftmp.74;
  if (res == 0B) goto <D.41679>; else goto <D.41680>;
  <D.41679>:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (getNextStream/%u)!Aborting...\n", 40);
  _1 = this->_errState;
  _2 = _1 | 2;
  this->_errState = _2;
  D.41681 = res;
  goto <D.41729>;
  <D.41680>:
  res->buf = 0B;
  res->bufSize = 4294967295;
  _3 = pbzip2::BZ2StreamScanner::failed (this);
  _4 = ~_3;
  if (_4 != 0) goto <D.41686>; else goto <D.41684>;
  <D.41686>:
  _5 = pbzip2::BZ2StreamScanner::isBz2HeaderFound (this);
  _6 = ~_5;
  if (_6 != 0) goto <D.41687>; else goto <D.41684>;
  <D.41687>:
  iftmp.76 = 1;
  goto <D.41685>;
  <D.41684>:
  iftmp.76 = 0;
  <D.41685>:
  retval.75 = iftmp.76;
  if (retval.75 != 0) goto <D.41688>; else goto <D.41689>;
  <D.41688>:
  this->_searchStatus = 0;
  pbzip2::BZ2StreamScanner::searchNextHeader (this);
  goto <D.41690>;
  <D.41689>:
  <D.41690>:
  retval.77 = pbzip2::BZ2StreamScanner::failed (this);
  if (retval.77 != 0) goto <D.41692>; else goto <D.41693>;
  <D.41692>:
  D.41681 = res;
  goto <D.41729>;
  <D.41693>:
  _7 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _8 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  if (_7 == _8) goto <D.41698>; else goto <D.41696>;
  <D.41698>:
  _9 = pbzip2::BZ2StreamScanner::eof (this);
  if (_9 != 0) goto <D.41699>; else goto <D.41696>;
  <D.41699>:
  iftmp.79 = 1;
  goto <D.41697>;
  <D.41696>:
  iftmp.79 = 0;
  <D.41697>:
  retval.78 = iftmp.79;
  if (retval.78 != 0) goto <D.41700>; else goto <D.41701>;
  <D.41700>:
  res->bufSize = 0;
  D.41681 = res;
  goto <D.41729>;
  <D.41701>:
  _10 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _11 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  if (_10 == _11) goto <D.41704>; else goto <D.41707>;
  <D.41707>:
  _12 = pbzip2::BZ2StreamScanner::getSearchStatus (this);
  _13 = ~_12;
  if (_13 != 0) goto <D.41708>; else goto <D.41705>;
  <D.41708>:
  _14 = pbzip2::BZ2StreamScanner::eof (this);
  _15 = ~_14;
  if (_15 != 0) goto <D.41704>; else goto <D.41705>;
  <D.41704>:
  iftmp.81 = 1;
  goto <D.41706>;
  <D.41705>:
  iftmp.81 = 0;
  <D.41706>:
  retval.80 = iftmp.81;
  if (retval.80 != 0) goto <D.41709>; else goto <D.41710>;
  <D.41709>:
  _16 = pbzip2::BZ2StreamScanner::getInBuffEnd (this);
  D.36612 = _16;
  _17 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  _18 = &this->_bz2Header;
  _19 = std::__cxx11::basic_string<unsigned char>::size (_18);
  _20 = _17 + _19;
  D.36611 = _20;
  _21 = std::min<unsigned char*> (&D.36611, &D.36612);
  _22 = *_21;
  this->_inBuffSearchPtr = _22;
  D.36611 = {CLOBBER};
  D.36612 = {CLOBBER};
  this->_searchStatus = 0;
  pbzip2::BZ2StreamScanner::searchNextHeader (this);
  goto <D.41711>;
  <D.41710>:
  <D.41711>:
  retval.82 = pbzip2::BZ2StreamScanner::failed (this);
  if (retval.82 != 0) goto <D.41713>; else goto <D.41714>;
  <D.41713>:
  D.41681 = res;
  goto <D.41729>;
  <D.41714>:
  pbzip2::BZ2StreamScanner::appendOutBuffDataUpToLimit (this);
  retval.83 = pbzip2::BZ2StreamScanner::failed (this);
  if (retval.83 != 0) goto <D.41716>; else goto <D.41717>;
  <D.41716>:
  D.41681 = res;
  goto <D.41729>;
  <D.41717>:
  _23 = this->_outSequenceNumber;
  if (_23 > 0) goto <D.41718>; else goto <D.41719>;
  <D.41718>:
  _24 = this->_outSequenceNumber;
  _25 = _24 + 1;
  this->_outSequenceNumber = _25;
  goto <D.41720>;
  <D.41719>:
  _26 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _27 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  retval.84 = _26 != _27;
  if (retval.84 != 0) goto <D.41722>; else goto <D.41723>;
  <D.41722>:
  this->_outSequenceNumber = 1;
  goto <D.41724>;
  <D.41723>:
  <D.41724>:
  <D.41720>:
  _28 = this->_outSequenceNumber;
  this->_outBuff.sequenceNumber = _28;
  _29 = this->_outBuff.bufSize;
  this->_outBuff.inSize = _29;
  _30 = this->_streamNumber;
  this->_outBuff.blockNumber = _30;
  _31 = pbzip2::BZ2StreamScanner::getInBuffCurrent (this);
  _32 = pbzip2::BZ2StreamScanner::getInBuffSearchPtr (this);
  retval.85 = _31 == _32;
  if (retval.85 != 0) goto <D.41726>; else goto <D.41727>;
  <D.41726>:
  this->_outBuff.isLastInSequence = 1;
  this->_outSequenceNumber = 0;
  _33 = this->_streamNumber;
  _34 = _33 + 1;
  this->_streamNumber = _34;
  goto <D.41728>;
  <D.41727>:
  this->_outBuff.isLastInSequence = 0;
  <D.41728>:
  *res = this->_outBuff;
  pbzip2::BZ2StreamScanner::initOutBuff (this, 0B, 0, 0);
  D.41681 = res;
  goto <D.41729>;
  <D.41729>:
  return D.41681;
}



;; Function void pbzip2::BZ2StreamScanner::initOutBuff(char*, size_t, size_t) (_ZN6pbzip216BZ2StreamScanner11initOutBuffEPcmm, funcdef_no=1941, decl_uid=29341, cgraph_uid=682, symbol_order=685)

void pbzip2::BZ2StreamScanner::initOutBuff(char*, size_t, size_t) (struct BZ2StreamScanner * const this, char * buf, size_t bufSize, size_t bufCapacity)
{
  this->_outBuff.buf = buf;
  _1 = (unsigned int) bufSize;
  this->_outBuff.bufSize = _1;
  this->_outBuffCapacity = bufCapacity;
  this->_outBuff.inSize = 0;
  return;
}

