
;; Function __find_if (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterINS1_17__normal_iteratorIS0_NSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEEEEEEEET_SD_SD_T0_St26random_access_iterator_tag, funcdef_no=106, decl_uid=4048, cgraph_uid=6, symbol_order=3591) (unlikely executed)

__find_if (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred, struct random_access_iterator_tag D.6106)
{
  unsigned char * const __pred$_M_current;
  difference_type __trip_count;
  long int _1;
  long int _2;
  long int _3;
  long int _4;
  long int _5;
  unsigned char * _10;
  unsigned char _18;
  unsigned char _22;
  unsigned char _25;
  unsigned char _26;
  unsigned char _27;
  unsigned char _28;
  unsigned char _29;
  unsigned char _30;
  unsigned char _31;
  unsigned char _32;
  unsigned char _33;

  <bb 2> [14.13%]:
  __pred$_M_current_34 = MEM[(struct _Iter_equals_iter *)&__pred];
  _1 = (long int) __last_12(D);
  _2 = (long int) __first_13(D);
  _3 = _1 - _2;
  __trip_count_14 = _3 >> 2;

  <bb 3> [100.00%]:
  # __first_6 = PHI <__first_13(D)(2), __first_20(8)>
  # __trip_count_9 = PHI <__trip_count_14(2), __trip_count_21(8)>
  if (__trip_count_9 <= 0)
    goto <bb 9>; [3.00%]
  else
    goto <bb 4>; [97.00%]

  <bb 4> [97.00%]:
  _25 = *__first_6;
  _22 = *__pred$_M_current_34;
  if (_22 == _25)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 5>; [97.00%]

  <bb 5> [94.09%]:
  __first_16 = __first_6 + 1;
  _18 = MEM[(unsigned char *)__first_6 + 1B];
  if (_18 == _22)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 6>; [97.00%]

  <bb 6> [91.27%]:
  __first_17 = &MEM[(void *)__first_6 + 2B];
  _26 = MEM[(unsigned char *)__first_6 + 2B];
  if (_22 == _26)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 7>; [97.00%]

  <bb 7> [88.53%]:
  __first_19 = &MEM[(void *)__first_6 + 3B];
  _27 = MEM[(unsigned char *)__first_6 + 3B];
  if (_22 == _27)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 8>; [97.00%]

  <bb 8> [85.87%]:
  __first_20 = &MEM[(void *)__first_6 + 4B];
  __trip_count_21 = __trip_count_9 + -1;
  goto <bb 3>; [100.00%]

  <bb 9> [3.00%]:
  _4 = (long int) __first_6;
  _5 = _1 - _4;
  switch (_5) <default: <L3> [25.00%], case 1: <L2> [25.00%], case 2: <L1> [25.00%], case 3: <L0> [25.00%]>

<L0> [0.75%]:
  _28 = *__first_6;
  _29 = *__pred$_M_current_34;
  if (_28 == _29)
    goto <bb 16> (<L3>); [30.50%]
  else
    goto <bb 11>; [69.50%]

  <bb 11> [0.52%]:
  __first_23 = __first_6 + 1;

  # __first_7 = PHI <__first_6(9), __first_23(11)>
<L1> [1.27%]:
  _30 = *__first_7;
  _31 = *__pred$_M_current_34;
  if (_30 == _31)
    goto <bb 16> (<L3>); [30.50%]
  else
    goto <bb 13>; [69.50%]

  <bb 13> [0.88%]:
  __first_24 = __first_7 + 1;

  # __first_8 = PHI <__first_6(9), __first_24(13)>
<L2> [1.63%]:
  _32 = *__first_8;
  _33 = *__pred$_M_current_34;
  if (_32 == _33)
    goto <bb 16> (<L3>); [30.50%]
  else
    goto <bb 15>; [69.50%]

  <bb 15> [1.14%]:

  # _10 = PHI <__first_6(4), __first_16(5), __first_17(6), __first_19(7), __last_12(D)(9), __first_6(10), __first_7(12), __first_8(14), __last_12(D)(15)>
<L3> [14.13%]:
  return _10;

}



;; Function __find_if (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterIS0_EEET_S5_S5_T0_St26random_access_iterator_tag, funcdef_no=83, decl_uid=4145, cgraph_uid=37, symbol_order=1395) (unlikely executed)

__find_if (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred, struct random_access_iterator_tag D.5876)
{
  unsigned char * __pred$_M_it1;
  difference_type __trip_count;
  long int _1;
  long int _2;
  long int _3;
  long int _4;
  long int _5;
  unsigned char * _10;
  unsigned char _18;
  unsigned char _22;
  unsigned char _25;
  unsigned char _26;
  unsigned char _27;
  unsigned char _28;
  unsigned char _29;
  unsigned char _30;
  unsigned char _31;
  unsigned char _32;
  unsigned char _33;

  <bb 2> [14.13%]:
  __pred$_M_it1_34 = MEM[(struct _Iter_equals_iter *)&__pred];
  _1 = (long int) __last_12(D);
  _2 = (long int) __first_13(D);
  _3 = _1 - _2;
  __trip_count_14 = _3 >> 2;

  <bb 3> [100.00%]:
  # __first_6 = PHI <__first_13(D)(2), __first_20(8)>
  # __trip_count_9 = PHI <__trip_count_14(2), __trip_count_21(8)>
  if (__trip_count_9 <= 0)
    goto <bb 9>; [3.00%]
  else
    goto <bb 4>; [97.00%]

  <bb 4> [97.00%]:
  _25 = *__first_6;
  _22 = *__pred$_M_it1_34;
  if (_22 == _25)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 5>; [97.00%]

  <bb 5> [94.09%]:
  __first_16 = __first_6 + 1;
  _18 = MEM[(unsigned char *)__first_6 + 1B];
  if (_18 == _22)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 6>; [97.00%]

  <bb 6> [91.27%]:
  __first_17 = &MEM[(void *)__first_6 + 2B];
  _26 = MEM[(unsigned char *)__first_6 + 2B];
  if (_22 == _26)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 7>; [97.00%]

  <bb 7> [88.53%]:
  __first_19 = &MEM[(void *)__first_6 + 3B];
  _27 = MEM[(unsigned char *)__first_6 + 3B];
  if (_22 == _27)
    goto <bb 16> (<L3>); [3.00%]
  else
    goto <bb 8>; [97.00%]

  <bb 8> [85.87%]:
  __first_20 = &MEM[(void *)__first_6 + 4B];
  __trip_count_21 = __trip_count_9 + -1;
  goto <bb 3>; [100.00%]

  <bb 9> [3.00%]:
  _4 = (long int) __first_6;
  _5 = _1 - _4;
  switch (_5) <default: <L3> [25.00%], case 1: <L2> [25.00%], case 2: <L1> [25.00%], case 3: <L0> [25.00%]>

<L0> [0.75%]:
  _28 = *__first_6;
  _29 = *__pred$_M_it1_34;
  if (_28 == _29)
    goto <bb 16> (<L3>); [30.50%]
  else
    goto <bb 11>; [69.50%]

  <bb 11> [0.52%]:
  __first_23 = __first_6 + 1;

  # __first_7 = PHI <__first_6(9), __first_23(11)>
<L1> [1.27%]:
  _30 = *__first_7;
  _31 = *__pred$_M_it1_34;
  if (_30 == _31)
    goto <bb 16> (<L3>); [30.50%]
  else
    goto <bb 13>; [69.50%]

  <bb 13> [0.88%]:
  __first_24 = __first_7 + 1;

  # __first_8 = PHI <__first_6(9), __first_24(13)>
<L2> [1.63%]:
  _32 = *__first_8;
  _33 = *__pred$_M_it1_34;
  if (_32 == _33)
    goto <bb 16> (<L3>); [30.50%]
  else
    goto <bb 15>; [69.50%]

  <bb 15> [1.14%]:

  # _10 = PHI <__first_6(4), __first_16(5), __first_17(6), __first_19(7), __last_12(D)(9), __first_6(10), __first_7(12), __first_8(14), __last_12(D)(15)>
<L3> [14.13%]:
  return _10;

}



;; Function __find_if (_ZSt9__find_ifIPhN9__gnu_cxx5__ops17_Iter_equals_iterIPcEEET_S6_S6_T0_St26random_access_iterator_tag, funcdef_no=82, decl_uid=4148, cgraph_uid=38, symbol_order=1391)

__find_if (unsigned char * __first, unsigned char * __last, struct _Iter_equals_iter __pred, struct random_access_iterator_tag D.5868)
{
  char * __pred$_M_it1;
  difference_type __trip_count;
  long int _1;
  long int _2;
  long int _3;
  long int _4;
  long int _5;
  unsigned char * _10;
  unsigned char _16;
  int _19;
  char _21;
  int _25;
  unsigned char _27;
  int _28;
  unsigned char _29;
  int _30;
  unsigned char _31;
  int _32;
  unsigned char _33;
  int _34;
  char _35;
  int _36;
  unsigned char _37;
  int _38;
  char _39;
  int _40;
  unsigned char _41;
  int _42;
  char _43;
  int _44;
  unsigned char _49;
  int _50;
  char _51;
  int _52;
  unsigned char _54;
  int _55;
  unsigned char _57;
  int _58;
  unsigned char _60;
  int _61;

  <bb 2> [14.13%]:
  __pred$_M_it1_45 = MEM[(struct _Iter_equals_iter *)&__pred];
  _1 = (long int) __last_12(D);
  _2 = (long int) __first_13(D);
  _3 = _1 - _2;
  __trip_count_14 = _3 >> 2;
  if (__trip_count_14 <= 0)
    goto <bb 12>; [3.00%]
  else
    goto <bb 3>; [97.00%]

  <bb 3> [13.68%]:
  _49 = *__first_13(D);
  _50 = (int) _49;
  _51 = *__pred$_M_it1_45;
  _52 = (int) _51;
  if (_52 == _50)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 4>; [97.00%]

  <bb 4> [13.27%]:
  __first_53 = __first_13(D) + 1;
  _54 = MEM[(unsigned char *)__first_13(D) + 1B];
  _55 = (int) _54;
  if (_52 == _55)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 5>; [97.00%]

  <bb 5> [12.87%]:
  __first_56 = &MEM[(void *)__first_13(D) + 2B];
  _57 = MEM[(unsigned char *)__first_13(D) + 2B];
  _58 = (int) _57;
  if (_52 == _58)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 6>; [97.00%]

  <bb 6> [12.48%]:
  __first_59 = &MEM[(void *)__first_13(D) + 3B];
  _60 = MEM[(unsigned char *)__first_13(D) + 3B];
  _61 = (int) _60;
  if (_52 == _61)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 11>; [97.00%]

  <bb 7> [83.32%]:
  _27 = *__first_22;
  _25 = (int) _27;
  _21 = *__pred$_M_it1_45;
  _19 = (int) _21;
  if (_19 == _25)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 8>; [97.00%]

  <bb 8> [80.82%]:
  __first_17 = __first_22 + 1;
  _16 = MEM[(unsigned char *)__first_22 + 1B];
  _28 = (int) _16;
  if (_19 == _28)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 9>; [97.00%]

  <bb 9> [78.40%]:
  __first_18 = &MEM[(void *)__first_22 + 2B];
  _29 = MEM[(unsigned char *)__first_22 + 2B];
  _30 = (int) _29;
  if (_19 == _30)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 10>; [97.00%]

  <bb 10> [76.05%]:
  __first_20 = &MEM[(void *)__first_22 + 3B];
  _31 = MEM[(unsigned char *)__first_22 + 3B];
  _32 = (int) _31;
  if (_19 == _32)
    goto <bb 19> (<L3>); [3.00%]
  else
    goto <bb 11>; [97.00%]

  <bb 11> [85.90%]:
  # __first_62 = PHI <__first_22(10), __first_13(D)(6)>
  # __trip_count_64 = PHI <__trip_count_23(10), __trip_count_14(6)>
  __first_22 = &MEM[(void *)__first_62 + 4B];
  __trip_count_23 = __trip_count_64 + -1;
  if (__trip_count_23 <= 0)
    goto <bb 12>; [3.00%]
  else
    goto <bb 7>; [97.00%]

  <bb 12> [3.00%]:
  # __first_63 = PHI <__first_22(11), __first_13(D)(2)>
  _4 = (long int) __first_63;
  _5 = _1 - _4;
  switch (_5) <default: <L3> [25.00%], case 1: <L2> [25.00%], case 2: <L1> [25.00%], case 3: <L0> [25.00%]>

<L0> [0.75%]:
  _33 = *__first_63;
  _34 = (int) _33;
  _35 = *__pred$_M_it1_45;
  _36 = (int) _35;
  if (_34 == _36)
    goto <bb 19> (<L3>); [30.50%]
  else
    goto <bb 14>; [69.50%]

  <bb 14> [0.52%]:
  __first_24 = __first_63 + 1;

  # __first_7 = PHI <__first_63(12), __first_24(14)>
<L1> [1.27%]:
  _37 = *__first_7;
  _38 = (int) _37;
  _39 = *__pred$_M_it1_45;
  _40 = (int) _39;
  if (_38 == _40)
    goto <bb 19> (<L3>); [30.50%]
  else
    goto <bb 16>; [69.50%]

  <bb 16> [0.88%]:
  __first_26 = __first_7 + 1;

  # __first_8 = PHI <__first_63(12), __first_26(16)>
<L2> [1.63%]:
  _41 = *__first_8;
  _42 = (int) _41;
  _43 = *__pred$_M_it1_45;
  _44 = (int) _43;
  if (_42 == _44)
    goto <bb 19> (<L3>); [30.50%]
  else
    goto <bb 18>; [69.50%]

  <bb 18> [1.14%]:

  # _10 = PHI <__first_22(7), __first_17(8), __first_18(9), __first_20(10), __last_12(D)(12), __first_63(13), __first_7(15), __first_8(17), __last_12(D)(18), __first_13(D)(3), __first_53(4), __first_56(5), __first_59(6)>
<L3> [14.13%]:
  return _10;

}



;; Function memstr (_Z6memstrIPhPcET_S2_iT0_i, funcdef_no=78, decl_uid=4170, cgraph_uid=41, symbol_order=1053)

memstr (unsigned char * searchBuf, int searchBufSize, char * searchString, int searchStringSize)
{
  char * __p1;
  char * __p;
  unsigned char * __current;
  struct _Iter_equals_iter __pred;
  struct random_access_iterator_tag D.5853;
  struct _Iter_equals_iter __pred;
  struct random_access_iterator_tag D.5852;
  unsigned char * D.5851;
  unsigned char * __first1;
  struct _Iter_equal_to_iter __predicate;
  struct _Iter_equal_to_iter D.5825;
  unsigned char * searchBufEnd;
  sizetype _1;
  sizetype _2;
  char * _3;
  unsigned char * _4;
  bool _14;
  bool _15;
  bool _16;
  unsigned char * _18;
  unsigned char * _20;
  unsigned char _23;
  int _24;
  char _26;
  int _27;
  unsigned char * _30;

  <bb 2> [26.28%]:
  _1 = (sizetype) searchBufSize_5(D);
  searchBufEnd_7 = searchBuf_6(D) + _1;
  _2 = (sizetype) searchStringSize_8(D);
  _3 = searchString_9(D) + _2;
  _14 = searchBuf_6(D) == searchBufEnd_7;
  _15 = _3 == searchString_9(D);
  _16 = _14 | _15;
  if (_16 != 0)
    goto <bb 10>; [46.00%]
  else
    goto <bb 3>; [54.00%]

  <bb 3> [14.19%]:
  __p1_17 = searchString_9(D) + 1;
  if (_3 == __p1_17)
    goto <bb 4>; [26.74%]
  else
    goto <bb 13>; [73.26%]

  <bb 4> [3.79%]:
  MEM[(struct _Iter_equals_iter *)&__pred] = searchString_9(D);
  _18 = __find_if (searchBuf_6(D), searchBufEnd_7, __pred, D.5853);
  goto <bb 10>; [100.00%]

  <bb 13> [10.40%]:

  <bb 5> [15.40%]:
  # __first1_19 = PHI <searchBuf_6(D)(13), __current_21(15)>
  MEM[(struct _Iter_equals_iter *)&__pred] = searchString_9(D);
  _20 = __find_if (__first1_19, searchBufEnd_7, __pred, D.5852);
  if (searchBufEnd_7 == _20)
    goto <bb 10>; [3.75%]
  else
    goto <bb 6>; [96.25%]

  <bb 6> [14.82%]:
  __current_21 = _20 + 1;
  if (searchBufEnd_7 == __current_21)
    goto <bb 10>; [3.75%]
  else
    goto <bb 14>; [96.25%]

  <bb 14> [14.26%]:

  <bb 7> [100.00%]:
  # __p_25 = PHI <__p1_17(14), __p_28(16)>
  # __current_22 = PHI <__current_21(14), __current_29(16)>
  _23 = *__current_22;
  _24 = (int) _23;
  _26 = *__p_25;
  _27 = (int) _26;
  if (_24 == _27)
    goto <bb 8>; [95.00%]
  else
    goto <bb 15>; [5.00%]

  <bb 15> [5.00%]:
  goto <bb 5>; [100.00%]

  <bb 8> [95.00%]:
  __p_28 = __p_25 + 1;
  if (_3 == __p_28)
    goto <bb 10>; [5.00%]
  else
    goto <bb 9>; [95.00%]

  <bb 9> [90.25%]:
  __current_29 = __current_22 + 1;
  if (searchBufEnd_7 == __current_29)
    goto <bb 10>; [5.00%]
  else
    goto <bb 16>; [95.00%]

  <bb 16> [85.74%]:
  goto <bb 7>; [100.00%]

  <bb 10> [26.28%]:
  # _30 = PHI <searchBuf_6(D)(2), _18(4), searchBufEnd_7(5), searchBufEnd_7(6), _20(8), searchBufEnd_7(9)>
  if (searchBufEnd_7 != _30)
    goto <bb 12>; [95.93%]
  else
    goto <bb 11>; [4.07%]

  <bb 11> [1.07%]:

  <bb 12> [26.28%]:
  # _4 = PHI <_30(10), 0B(11)>
  return _4;

}



;; Function add (_ZN5queue3addEP7outBuff, funcdef_no=31, decl_uid=4248, cgraph_uid=65, symbol_order=366) (unlikely executed)

add (struct queue * const this, struct ElementType * element)
{
  int _1;
  struct ElementType * _2;
  struct ElementType * * _3;
  long int _4;
  long unsigned int _5;
  long unsigned int _6;
  struct ElementType * * _7;
  long int _8;
  long int _9;
  long int _10;
  long int _11;
  long int _12;
  long int _13;
  long int _14;
  long int _15;

  <bb 2> [100.00%]:
  _1 = element_21(D)->sequenceNumber;
  if (_1 > 1)
    goto <bb 3>; [64.00%]
  else
    goto <bb 4>; [36.00%]

  <bb 3> [64.00%]:
  _2 = this_22(D)->lastElement;
  _2->next = element_21(D);
  goto <bb 9>; [100.00%]

  <bb 4> [36.00%]:
  _3 = this_22(D)->qData;
  _4 = this_22(D)->tail;
  _5 = (long unsigned int) _4;
  _6 = _5 * 8;
  _7 = _3 + _6;
  *_7 = element_21(D);
  _8 = this_22(D)->tail;
  _9 = _8 + 1;
  this_22(D)->tail = _9;
  _10 = this_22(D)->size;
  if (_9 == _10)
    goto <bb 5>; [34.00%]
  else
    goto <bb 6>; [66.00%]

  <bb 5> [12.24%]:
  this_22(D)->tail = 0;

  <bb 6> [36.00%]:
  _11 = this_22(D)->tail;
  _12 = this_22(D)->head;
  if (_11 == _12)
    goto <bb 7>; [34.00%]
  else
    goto <bb 8>; [66.00%]

  <bb 7> [12.24%]:
  this_22(D)->topLevelFull = 1;

  <bb 8> [36.00%]:
  this_22(D)->topLevelEmpty = 0;

  <bb 9> [100.00%]:
  this_22(D)->lastElement = element_21(D);
  _13 = this_22(D)->count;
  _14 = _13 + 1;
  this_22(D)->count = _14;
  _15 = this_22(D)->size;
  if (_14 == _15)
    goto <bb 10>; [37.68%]
  else
    goto <bb 11>; [62.32%]

  <bb 10> [37.68%]:
  this_22(D)->full = 1;

  <bb 11> [100.00%]:
  this_22(D)->empty = 0;
  return;

}



;; Function saveError (_ZN6pbzip212ErrorContext9saveErrorEv, funcdef_no=112, decl_uid=3989, cgraph_uid=0, symbol_order=4641)

saveError (struct ErrorContext * const this)
{
  int newerr;
  int * _1;
  int _2;
  int _3;
  int * _4;
  int * _10;
  int * _11;

  <bb 2> [100.00%]:
  _1 = __errno_location ();
  newerr_7 = *_1;
  pthread_mutex_lock (&_err_ctx_mutex);
  if (newerr_7 != 0)
    goto <bb 3>; [54.00%]
  else
    goto <bb 7>; [46.00%]

  <bb 3> [54.00%]:
  _2 = this_9(D)->_first_kernel_err_no;
  if (_2 == 0)
    goto <bb 4>; [50.00%]
  else
    goto <bb 5>; [50.00%]

  <bb 4> [27.00%]:
  _11 = &this_9(D)->_first_kernel_err_no;
  goto <bb 6>; [100.00%]

  <bb 5> [27.00%]:
  _10 = &this_9(D)->_last_kernel_err_no;

  <bb 6> [54.00%]:
  # _4 = PHI <_11(4), _10(5)>
  *_4 = newerr_7;

  <bb 7> [100.00%]:
  _3 = *_1;
  this_9(D)->_last_kernel_err_no = _3;
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;

}



;; Function banner (_Z6bannerv, funcdef_no=24, decl_uid=4290, cgraph_uid=125, symbol_order=791) (executed once)

banner ()
{
  struct FILE * _1;
  struct FILE * _2;
  struct FILE * _3;
  struct FILE * _4;

  <bb 2> [100.00%]:
  _1 = stderr;
  __builtin_fwrite ("Parallel BZIP2 v1.1.11 [Dec 12, 2014]\n", 1, 38, _1);
  _2 = stderr;
  __builtin_fwrite ("By: Jeff Gilchrist [http://compression.ca]\n", 1, 43, _2);
  _3 = stderr;
  __builtin_fwrite ("Major contributions: Yavor Nikolov [http://javornikolov.wordpress.com]\n", 1, 71, _3);
  _4 = stderr;
  __builtin_fwrite ("Uses libbzip2 by Julian Seward\n", 1, 31, _4);
  return;

}



;; Function printErrnoMsg (_ZN6pbzip212ErrorContext13printErrnoMsgEP8_IO_FILEi, funcdef_no=109, decl_uid=4031, cgraph_uid=3, symbol_order=4638)

printErrnoMsg (struct FILE * out, int err)
{
  char * _1;

  <bb 2> [100.00%]:
  if (err_3(D) != 0)
    goto <bb 3>; [36.64%]
  else
    goto <bb 4>; [63.36%]

  <bb 3> [36.64%]:
  _1 = strerror (err_3(D));
  __fprintf_chk (out_6(D), 1, "pbzip2: *ERROR: system call failed with errno=[%d: %s]!\n", err_3(D), _1);

  <bb 4> [100.00%]:
  return;

}



;; Function syncPrintErrnoMsg (_ZN6pbzip212ErrorContext17syncPrintErrnoMsgEP8_IO_FILEi, funcdef_no=110, decl_uid=4030, cgraph_uid=2, symbol_order=4639)

syncPrintErrnoMsg (struct FILE * out, int err)
{
  <bb 2> [100.00%]:
  pthread_mutex_lock (&_err_ctx_mutex);
  printErrnoMsg (out_3(D), err_4(D));
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;

}



;; Function printErrorMessages (_ZN6pbzip212ErrorContext18printErrorMessagesEP8_IO_FILE, funcdef_no=111, decl_uid=4029, cgraph_uid=1, symbol_order=4640)

printErrorMessages (struct ErrorContext * const this, struct FILE * out)
{
  int _1;
  int _2;

  <bb 2> [100.00%]:
  pthread_mutex_lock (&_err_ctx_mutex);
  _1 = this_5(D)->_first_kernel_err_no;
  printErrnoMsg (out_6(D), _1);
  _2 = this_5(D)->_last_kernel_err_no;
  printErrnoMsg (out_6(D), _2);
  pthread_mutex_unlock (&_err_ctx_mutex);
  return;

}



;; Function getInstance (_ZN6pbzip212ErrorContext11getInstanceEv, funcdef_no=108, decl_uid=4032, cgraph_uid=4, symbol_order=4637)

getInstance ()
{
  struct ErrorContext * _1;
  struct FILE * _2;
  void * _8;
  struct ErrorContext * _11;

  <bb 2> [100.00%]:
  pthread_mutex_lock (&_err_ctx_mutex);
  _1 = _instance;
  if (_1 == 0B)
    goto <bb 3>; [19.86%]
  else
    goto <bb 7>; [80.14%]

  <bb 3> [19.86%]:
  _8 = operator new (8, &nothrow);
  if (_8 != 0B)
    goto <bb 4>; [70.00%]
  else
    goto <bb 5>; [30.00%]

  <bb 4> [13.90%]:
  MEM[(struct  &)_8] ={v} {CLOBBER};
  MEM[(struct ErrorContext *)_8]._first_kernel_err_no = 0;
  MEM[(struct ErrorContext *)_8]._last_kernel_err_no = 0;

  <bb 5> [19.86%]:
  _instance = _8;
  if (_8 == 0B)
    goto <bb 6>; [19.86%]
  else
    goto <bb 7>; [80.14%]

  <bb 6> [3.94%]:
  _2 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Can\'t initialize error context - out of memory!\n", 1, 64, _2);

  <bb 7> [100.00%]:
  pthread_mutex_unlock (&_err_ctx_mutex);
  _11 = _instance;
  return _11;

}



;; Function copy (_ZN9__gnu_cxx11char_traitsIhE4copyEPhPKhm, funcdef_no=107, decl_uid=4038, cgraph_uid=5, symbol_order=3634) (unlikely executed)

copy (char_type * __s1, const char_type * __s2, size_t __n)
{
  const ptrdiff_t _Num;

  <bb 2> [100.00%]:
  if (__n_2(D) != 0)
    goto <bb 3>; [36.64%]
  else
    goto <bb 4>; [63.36%]

  <bb 3> [36.64%]:
  __builtin_memmove (__s1_4(D), __s2_1(D), __n_2(D));

  <bb 4> [100.00%]:
  return __s1_4(D);

}



;; Function __base_dtor  (_ZNSt6vectorI7outBuffSaIS0_EED2Ev, funcdef_no=81, decl_uid=4149, cgraph_uid=39, symbol_order=1255)

__base_dtor  (struct vector * const this)
{
  struct outBuff * _1;

  <bb 2> [100.00%]:
  _1 = this_2(D)->D.4140._M_impl._M_start;
  if (_1 != 0B)
    goto <bb 3>; [57.44%]
  else
    goto <bb 4>; [42.56%]

  <bb 3> [57.44%]:
  operator delete (_1);

  <bb 4> [100.00%]:
  MEM[(struct  &)this_2(D)] ={v} {CLOBBER};
  return;

}



;; Function assign (_ZNSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE6assignEPKh, funcdef_no=101, decl_uid=4070, cgraph_uid=7, symbol_order=3469) (unlikely executed)

assign (struct basic_string * const this, const unsigned char * __s)
{
  unsigned char * D.6101;
  const size_type __how_much;
  size_type __new_capacity;
  char_type * D.6076;
  char_type * D.6072;
  char_type * D.6068;
  char_type * D.6064;
  char_type * D.6060;
  const size_type __new_size;
  unsigned char * __p;
  const size_type __how_much;
  const size_type __nleft;
  struct basic_string & D.6051;
  size_t __i;
  long unsigned int _5;
  const char_type * _8;
  unsigned char _9;
  unsigned char * _18;
  const unsigned char[16] * _19;
  long unsigned int _20;
  long unsigned int _21;
  unsigned char * _25;
  bool _26;
  bool _27;
  bool _28;
  unsigned char _35;
  unsigned char _36;
  unsigned char _45;
  sizetype _46;
  const unsigned char * _47;
  unsigned char _48;
  long int _49;
  long int _50;
  long int _51;
  unsigned char _53;
  long unsigned int _54;
  unsigned char * _55;
  unsigned char * _56;
  unsigned char _57;
  unsigned char * _58;
  unsigned char * _59;
  signed long _66;
  bool _77;
  bool _78;
  bool _79;
  unsigned char _81;
  unsigned char * _88;
  signed long _91;
  long unsigned int _92;
  signed long _93;
  long unsigned int _95;
  unsigned char * _96;

  <bb 2> [15.00%]:

  <bb 3> [100.00%]:
  # __i_3 = PHI <0(2), __i_10(4)>
  _8 = __s_2(D) + __i_3;
  _9 = *_8;
  if (_9 != 0)
    goto <bb 4>; [85.00%]
  else
    goto <bb 5>; [15.00%]

  <bb 4> [85.00%]:
  __i_10 = __i_3 + 1;
  goto <bb 3>; [100.00%]

  <bb 5> [15.00%]:
  _5 = MEM[(const struct basic_string *)this_4(D)]._M_string_length;
  _66 = (signed long) __i_3;
  if (_66 < 0)
    goto <bb 6>; [0.04%]
  else
    goto <bb 7>; [99.96%]

  <bb 6> [0.01%]:
  __throw_length_error ("basic_string::_M_replace");

  <bb 7> [14.99%]:
  _18 = MEM[(const struct basic_string *)this_4(D)]._M_dataplus._M_p;
  _19 = &MEM[(const struct basic_string *)this_4(D)].D.4058._M_local_buf;
  if (_18 == _19)
    goto <bb 9>; [30.00%]
  else
    goto <bb 8>; [70.00%]

  <bb 8> [10.50%]:
  _20 = MEM[(const struct basic_string *)this_4(D)].D.4058._M_allocated_capacity;

  <bb 9> [14.99%]:
  # _21 = PHI <15(7), _20(8)>
  if (__i_3 <= _21)
    goto <bb 10>; [67.00%]
  else
    goto <bb 35>; [33.00%]

  <bb 10> [10.05%]:
  if (__s_2(D) < _18)
    goto <bb 13>; [50.00%]
  else
    goto <bb 11>; [50.00%]

  <bb 11> [5.02%]:
  _25 = _18 + _5;
  if (__s_2(D) > _25)
    goto <bb 13>; [50.00%]
  else
    goto <bb 12>; [50.00%]

  <bb 12> [2.51%]:
  _26 = __i_3 != 0;
  _27 = __i_3 <= _5;
  _28 = _26 & _27;
  if (_28 != 0)
    goto <bb 17>; [50.00%]
  else
    goto <bb 20>; [50.00%]

  <bb 13> [7.53%]:
  if (__i_3 != 0)
    goto <bb 14>; [54.00%]
  else
    goto <bb 47>; [46.00%]

  <bb 14> [4.07%]:
  if (__i_3 == 1)
    goto <bb 15>; [51.12%]
  else
    goto <bb 16>; [48.88%]

  <bb 15> [2.08%]:
  _35 = MEM[(const char_type &)__s_2(D)];
  MEM[(char_type &)_18] = _35;
  goto <bb 47>; [100.00%]

  <bb 16> [1.99%]:
  copy (_18, __s_2(D), __i_3);
  goto <bb 47>; [100.00%]

  <bb 17> [1.26%]:
  if (__i_3 == 1)
    goto <bb 18>; [51.12%]
  else
    goto <bb 19>; [48.88%]

  <bb 18> [0.64%]:
  _36 = MEM[(const char_type &)__s_2(D)];
  MEM[(char_type &)_18] = _36;
  goto <bb 20>; [100.00%]

  <bb 19> [0.61%]:
  __builtin_memmove (_18, __s_2(D), __i_3);

  <bb 20> [2.51%]:
  if (__i_3 > _5)
    goto <bb 21>; [54.00%]
  else
    goto <bb 47>; [46.00%]

  <bb 21> [1.36%]:
  if (_8 <= _25)
    goto <bb 22>; [50.00%]
  else
    goto <bb 25>; [50.00%]

  <bb 22> [0.68%]:
  if (__i_3 == 1)
    goto <bb 23>; [51.12%]
  else
    goto <bb 24>; [48.88%]

  <bb 23> [0.35%]:
  _45 = MEM[(const char_type &)__s_2(D)];
  MEM[(char_type &)_18] = _45;
  goto <bb 47>; [100.00%]

  <bb 24> [0.33%]:
  __builtin_memmove (_18, __s_2(D), __i_3);
  goto <bb 47>; [100.00%]

  <bb 25> [0.68%]:
  if (__s_2(D) >= _25)
    goto <bb 26>; [50.00%]
  else
    goto <bb 29>; [50.00%]

  <bb 26> [0.34%]:
  _46 = __i_3 - _5;
  _47 = __s_2(D) + _46;
  if (__i_3 == 1)
    goto <bb 27>; [51.12%]
  else
    goto <bb 28>; [48.88%]

  <bb 27> [0.17%]:
  _48 = MEM[(const char_type &)_47];
  MEM[(char_type &)_18] = _48;
  goto <bb 47>; [100.00%]

  <bb 28> [0.17%]:
  copy (_18, _47, __i_3);
  goto <bb 47>; [100.00%]

  <bb 29> [0.34%]:
  _49 = (long int) _25;
  _50 = (long int) __s_2(D);
  _51 = _49 - _50;
  __nleft_52 = (const size_type) _51;
  if (__nleft_52 == 1)
    goto <bb 30>; [51.12%]
  else
    goto <bb 31>; [48.88%]

  <bb 30> [0.17%]:
  _53 = MEM[(const char_type &)__s_2(D)];
  MEM[(char_type &)_18] = _53;
  goto <bb 32>; [100.00%]

  <bb 31> [0.17%]:
  __builtin_memmove (_18, __s_2(D), __nleft_52);

  <bb 32> [0.34%]:
  _54 = __i_3 - __nleft_52;
  _55 = _18 + __i_3;
  _56 = _18 + __nleft_52;
  if (_54 == 1)
    goto <bb 33>; [51.12%]
  else
    goto <bb 34>; [48.88%]

  <bb 33> [0.17%]:
  _57 = MEM[(const char_type &)_55];
  MEM[(char_type &)_56] = _57;
  goto <bb 47>; [100.00%]

  <bb 34> [0.17%]:
  copy (_56, _55, _54);
  goto <bb 47>; [100.00%]

  <bb 35> [4.95%]:
  _91 = (signed long) __i_3;
  if (_91 < 0)
    goto <bb 36>; [0.04%]
  else
    goto <bb 37>; [99.96%]

  <bb 36> [0.00%]:
  __throw_length_error ("basic_string::_M_create");

  <bb 37> [2.67%]:
  _92 = _21 * 2;
  if (__i_3 < _92)
    goto <bb 38>; [54.00%]
  else
    goto <bb 40>; [46.00%]

  <bb 38> [1.44%]:
  _93 = (signed long) _92;
  if (_93 < 0)
    goto <bb 39>; [39.77%]
  else
    goto <bb 40>; [60.23%]

  <bb 39> [0.57%]:

  <bb 40> [4.95%]:
  # __new_capacity_145 = PHI <9223372036854775807(39), __i_3(37), _92(38)>
  _95 = __new_capacity_145 + 1;
  _96 = operator new (_95);
  _77 = __s_2(D) != 0B;
  _78 = __i_3 != 0;
  _79 = _77 & _78;
  if (_79 != 0)
    goto <bb 41>; [50.00%]
  else
    goto <bb 44>; [50.00%]

  <bb 41> [2.47%]:
  if (__i_3 == 1)
    goto <bb 42>; [51.12%]
  else
    goto <bb 43>; [48.88%]

  <bb 42> [1.26%]:
  _81 = MEM[(const char_type &)__s_2(D)];
  MEM[(char_type &)_96] = _81;
  goto <bb 44>; [100.00%]

  <bb 43> [1.21%]:
  copy (_96, __s_2(D), __i_3);

  <bb 44> [4.95%]:
  _88 = MEM[(const struct basic_string *)this_4(D)]._M_dataplus._M_p;
  if (_19 != _88)
    goto <bb 45>; [57.44%]
  else
    goto <bb 46>; [42.56%]

  <bb 45> [2.84%]:
  operator delete (_88);

  <bb 46> [4.95%]:
  this_4(D)->_M_dataplus._M_p = _96;
  this_4(D)->D.4058._M_allocated_capacity = __new_capacity_145;

  <bb 47> [14.99%]:
  this_4(D)->_M_string_length = __i_3;
  _58 = MEM[(const struct basic_string *)this_4(D)]._M_dataplus._M_p;
  _59 = _58 + __i_3;
  MEM[(char_type &)_59] = 0;
  return this_4(D);

}



;; Function compare (_ZNKSt7__cxx1112basic_stringIhSt11char_traitsIhESaIhEE7compareEmmPKhm, funcdef_no=100, decl_uid=4075, cgraph_uid=16, symbol_order=3349) (unlikely executed)

No longer having address taken: __n1
No longer having address taken: __n2

Symbols to be put in SSA form
{ D.6018 D.6020 }
Incremental SSA update started at block: 0
Number of blocks in CFG: 14
Number of blocks to update: 13 ( 93%)


compare (const struct basic_string * const this, size_type __pos, size_type __n1, const unsigned char * __s, size_type __n2)
{
  const difference_type __d;
  size_t __i;
  int __r;
  const size_type __len;
  long unsigned int _1;
  unsigned char _6;
  unsigned char _8;
  const char_type * _10;
  const char_type * _12;
  int _15;
  unsigned char * _16;
  long unsigned int _17;
  long unsigned int _19;
  long unsigned int _21;
  long unsigned int _22;
  int _24;
  long unsigned int _25;
  long unsigned int _30;

  <bb 2> [14.27%]:
  _21 = this_4(D)->_M_string_length;
  if (__pos_5(D) > _21)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.01%]:
  __throw_out_of_range_fmt ("%s: __pos (which is %zu) > this->size() (which is %zu)", "basic_string::compare", __pos_5(D), _21);

  <bb 4> [14.26%]:
  _1 = __n1_9(D);
  _19 = _21 - __pos_5(D);
  _30 = MIN_EXPR <_19, _1>;
  __n1_20 = _30;
  _17 = __n2_29(D);
  __len_18 = MIN_EXPR <_17, _30>;
  _16 = this_4(D)->_M_dataplus._M_p;

  <bb 5> [100.00%]:
  # __i_13 = PHI <0(4), __i_14(8)>
  if (__i_13 >= __len_18)
    goto <bb 9>; [5.00%]
  else
    goto <bb 6>; [95.00%]

  <bb 6> [95.00%]:
  _12 = __s_11(D) + __i_13;
  _25 = __pos_5(D) + __i_13;
  _10 = _16 + _25;
  _8 = *_10;
  _6 = *_12;
  if (_6 > _8)
    goto <bb 9>; [5.00%]
  else
    goto <bb 7>; [95.00%]

  <bb 7> [90.25%]:
  if (_6 < _8)
    goto <bb 9>; [5.00%]
  else
    goto <bb 8>; [95.00%]

  <bb 8> [85.74%]:
  __i_14 = __i_13 + 1;
  goto <bb 5>; [100.00%]

  <bb 9> [14.26%]:
  # _15 = PHI <0(5), -1(6), 1(7)>
  if (_15 == 0)
    goto <bb 10>; [54.00%]
  else
    goto <bb 13>; [46.00%]

  <bb 10> [7.70%]:
  _22 = _30 - _17;
  __d_23 = (const difference_type) _22;
  if (__d_23 > 2147483647)
    goto <bb 13>; [31.00%]
  else
    goto <bb 11>; [69.00%]

  <bb 11> [5.31%]:
  if (__d_23 < -2147483648)
    goto <bb 13>; [2.00%]
  else
    goto <bb 12>; [98.00%]

  <bb 12> [5.21%]:
  _24 = (int) _22;

  <bb 13> [14.26%]:
  # __r_2 = PHI <_15(9), 2147483647(10), -2147483648(11), _24(12)>
  return __r_2;

}



;; Function locateHeaderPrefixInBuff (_ZN6pbzip216BZ2StreamScanner24locateHeaderPrefixInBuffEv, funcdef_no=92, decl_uid=4116, cgraph_uid=26, symbol_order=3097) (unlikely executed)

locateHeaderPrefixInBuff (struct BZ2StreamScanner * const this)
{
  unsigned char * __current;
  struct _Iter_equals_iter __pred;
  struct random_access_iterator_tag D.5956;
  struct _Iter_equals_iter __pred;
  struct random_access_iterator_tag D.5955;
  unsigned char * const __last2;
  unsigned char * __p$_M_current;
  unsigned char * D.5951;
  unsigned char * __first1;
  struct __normal_iterator __first2;
  struct __normal_iterator __last2;
  struct _Iter_equal_to_iter __predicate;
  struct _Iter_equal_to_iter D.5926;
  struct __normal_iterator __first2;
  struct __normal_iterator __last2;
  unsigned int _1;
  unsigned int _2;
  struct basic_string * _3;
  unsigned int _4;
  unsigned int _5;
  CharType * _9;
  CharType * _10;
  CharType * _11;
  long unsigned int _13;
  long int _14;
  long int _15;
  CharType * _16;
  int _18;
  long unsigned int _20;
  long int _23;
  bool _25;
  CharType * _26;
  CharType * _28;
  CharType * _29;
  unsigned char * _30;
  unsigned char * _31;
  CharType * _32;
  bool _35;
  bool _36;
  unsigned char * _46;
  unsigned char * _47;
  unsigned char * _49;
  unsigned char _52;
  unsigned char _54;
  unsigned char * _55;
  unsigned char * _57;
  bool _60;

  <bb 2> [80.12%]:
  _25 = MEM[(const struct BZ2StreamScanner *)this_8(D)]._bz2HeaderFound;
  if (_25 != 0)
    goto <bb 8>; [33.00%]
  else
    goto <bb 3>; [67.00%]

  <bb 3> [53.68%]:
  _10 = this_8(D)->_inBuffSearchPtr;
  _11 = this_8(D)->_inBuff;
  if (_10 != _11)
    goto <bb 5>; [66.53%]
  else
    goto <bb 4>; [33.47%]

  <bb 4> [17.97%]:
  _16 = MEM[(const struct BZ2StreamScanner *)this_8(D)]._inBuffEnd;
  _23 = (long int) _16;
  _15 = (long int) _11;
  _14 = _23 - _15;
  _13 = (long unsigned int) _14;
  _20 = MEM[(const struct basic_string *)this_8(D) + 16B]._M_string_length;
  if (_13 < _20)
    goto <bb 5>; [63.36%]
  else
    goto <bb 6>; [36.64%]

  <bb 5> [47.10%]:
  _1 = this_8(D)->_errState;
  _2 = _1 | 32;
  this_8(D)->_errState = _2;
  _9 = this_8(D)->_inBuffEnd;
  this_8(D)->_inBuffSearchPtr = _9;
  goto <bb 17>; [100.00%]

  <bb 6> [6.58%]:
  _3 = &this_8(D)->_bz2Header;
  _18 = compare (_3, 0, 3, _10, 3);
  if (_18 == 0)
    goto <bb 17>; [50.00%]
  else
    goto <bb 7>; [50.00%]

  <bb 7> [3.29%]:
  _4 = this_8(D)->_errState;
  _5 = _4 | 32;
  this_8(D)->_errState = _5;
  _26 = this_8(D)->_inBuffEnd;
  this_8(D)->_inBuffSearchPtr = _26;
  goto <bb 17>; [100.00%]

  <bb 8> [26.44%]:
  _31 = MEM[(const struct basic_string *)this_8(D) + 16B]._M_dataplus._M_p;
  _30 = _31 + 3;
  _29 = this_8(D)->_inBuffEnd;
  _28 = this_8(D)->_inBuffSearchPtr;
  _35 = _30 == _31;
  _36 = _28 == _29;
  _60 = _35 | _36;
  if (_60 != 0)
    goto <bb 16>; [46.33%]
  else
    goto <bb 9>; [53.67%]

  <bb 9> [14.19%]:
  _46 = _31 + 1;
  if (_30 == _46)
    goto <bb 10>; [26.74%]
  else
    goto <bb 18>; [73.26%]

  <bb 10> [3.79%]:
  MEM[(struct _Iter_equals_iter *)&__pred] = _31;
  _47 = __find_if (_28, _29, __pred, D.5956);
  goto <bb 16>; [100.00%]

  <bb 18> [10.40%]:

  <bb 11> [15.40%]:
  # __first1_48 = PHI <_28(18), __current_50(20)>
  MEM[(struct _Iter_equals_iter *)&__pred] = _31;
  _49 = __find_if (__first1_48, _29, __pred, D.5955);
  if (_29 == _49)
    goto <bb 16>; [3.75%]
  else
    goto <bb 12>; [96.25%]

  <bb 12> [14.82%]:
  __current_50 = _49 + 1;
  if (_29 == __current_50)
    goto <bb 16>; [3.75%]
  else
    goto <bb 19>; [96.25%]

  <bb 19> [14.26%]:

  <bb 13> [100.00%]:
  # __current_51 = PHI <__current_50(19), __current_56(21)>
  # __p$_M_current_53 = PHI <_46(19), _55(21)>
  _52 = *__current_51;
  _54 = *__p$_M_current_53;
  if (_52 == _54)
    goto <bb 14>; [95.00%]
  else
    goto <bb 20>; [5.00%]

  <bb 20> [5.00%]:
  goto <bb 11>; [100.00%]

  <bb 14> [95.00%]:
  _55 = __p$_M_current_53 + 1;
  if (_30 == _55)
    goto <bb 16>; [5.00%]
  else
    goto <bb 15>; [95.00%]

  <bb 15> [90.25%]:
  __current_56 = __current_51 + 1;
  if (_29 == __current_56)
    goto <bb 16>; [5.00%]
  else
    goto <bb 21>; [95.00%]

  <bb 21> [85.74%]:
  goto <bb 13>; [100.00%]

  <bb 16> [26.44%]:
  # _57 = PHI <_29(15), _28(8), _47(10), _29(11), _29(12), _49(14)>
  this_8(D)->_inBuffSearchPtr = _57;

  <bb 17> [80.12%]:
  _32 = this_8(D)->_inBuffSearchPtr;
  return _32;

}



;; Function searchNextHeaderInBuff (_ZN6pbzip216BZ2StreamScanner22searchNextHeaderInBuffEv, funcdef_no=94, decl_uid=4115, cgraph_uid=25, symbol_order=3098) (unlikely executed)

searchNextHeaderInBuff (struct BZ2StreamScanner * const this)
{
  CharType bwtSizeChar;
  struct basic_string * pHdr;
  unsigned char _1;
  unsigned char _2;
  unsigned char _3;
  unsigned char _4;
  long unsigned int _5;
  CharType * _6;
  unsigned int _7;
  unsigned int _8;
  CharType * _9;
  CharType * _10;
  CharType * _12;
  unsigned char * _21;
  unsigned char * _22;
  long unsigned int _23;
  long int _24;
  long int _27;
  CharType * _29;
  long int _31;
  CharType * _32;
  long unsigned int _33;
  int _35;
  CharType * _37;
  unsigned int _41;
  CharType * _43;
  long unsigned int _44;
  unsigned char * _45;
  bool _46;
  CharType * _47;

  <bb 2> [11.27%]:
  this_18(D)->_searchStatus = 0;
  locateHeaderPrefixInBuff (this_18(D));

  <bb 3> [100.00%]:
  _41 = this_18(D)->_errState;
  if (_41 == 0)
    goto <bb 4>; [96.25%]
  else
    goto <bb 16>; [3.75%]

  <bb 4> [96.25%]:
  _32 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._inBuffEnd;
  _31 = (long int) _32;
  _29 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._inBuffSearchPtr;
  _27 = (long int) _29;
  _24 = _31 - _27;
  _23 = (long unsigned int) _24;
  _33 = MEM[(const struct basic_string *)this_18(D) + 16B]._M_string_length;
  if (_23 >= _33)
    goto <bb 5>; [96.25%]
  else
    goto <bb 16>; [3.75%]

  <bb 5> [92.64%]:
  _1 = MEM[(CharType *)_29 + 4B];
  _22 = MEM[(const struct basic_string *)this_18(D) + 16B]._M_dataplus._M_p;
  _2 = MEM[(value_type &)_22 + 4];
  if (_1 == _2)
    goto <bb 6>; [34.00%]
  else
    goto <bb 7>; [66.00%]

  <bb 6> [31.50%]:
  pHdr_26 = &this_18(D)->_bz2Header;
  goto <bb 9>; [100.00%]

  <bb 7> [61.14%]:
  _21 = MEM[(const struct basic_string *)this_18(D) + 48B]._M_dataplus._M_p;
  _3 = MEM[(value_type &)_21 + 4];
  if (_1 == _3)
    goto <bb 8>; [34.00%]
  else
    goto <bb 13>; [66.00%]

  <bb 8> [20.79%]:
  pHdr_25 = &this_18(D)->_bz2HeaderZero;

  <bb 9> [52.29%]:
  # pHdr_11 = PHI <pHdr_26(6), pHdr_25(8)>
  if (pHdr_11 != 0B)
    goto <bb 10>; [70.00%]
  else
    goto <bb 13>; [30.00%]

  <bb 10> [36.60%]:
  bwtSizeChar_28 = MEM[(CharType *)_29 + 3B];
  _4 = bwtSizeChar_28 + 207;
  if (_4 <= 8)
    goto <bb 11>; [33.00%]
  else
    goto <bb 13>; [67.00%]

  <bb 11> [12.08%]:
  _45 = MEM[(const struct basic_string *)pHdr_11]._M_dataplus._M_p;
  MEM[(value_type &)_45 + 3] = bwtSizeChar_28;
  _44 = MEM[(const struct basic_string *)pHdr_11]._M_string_length;
  _5 = _44 + 18446744073709551612;
  _43 = this_18(D)->_inBuffSearchPtr;
  _6 = _43 + 4;
  _35 = compare (pHdr_11, 4, _5, _6, _5);
  if (_35 == 0)
    goto <bb 12>; [3.75%]
  else
    goto <bb 13>; [96.25%]

  <bb 12> [0.45%]:
  this_18(D)->_searchStatus = 1;
  _37 = this_18(D)->_inBuffSearchPtr;
  goto <bb 17>; [100.00%]

  <bb 13> [92.19%]:
  _46 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._bz2HeaderFound;
  if (_46 != 0)
    goto <bb 15>; [96.25%]
  else
    goto <bb 14>; [3.75%]

  <bb 14> [3.46%]:
  _7 = this_18(D)->_errState;
  _8 = _7 | 32;
  this_18(D)->_errState = _8;
  goto <bb 16>; [100.00%]

  <bb 15> [88.73%]:
  _9 = this_18(D)->_inBuffSearchPtr;
  _10 = _9 + 3;
  this_18(D)->_inBuffSearchPtr = _10;
  locateHeaderPrefixInBuff (this_18(D));
  goto <bb 3>; [100.00%]

  <bb 16> [10.82%]:
  _47 = this_18(D)->_inBuffEnd;
  this_18(D)->_inBuffSearchPtr = _47;

  <bb 17> [11.27%]:
  # _12 = PHI <_37(12), _47(16)>
  return _12;

}



;; Function dispose (_ZN6pbzip216BZ2StreamScanner7disposeEv, funcdef_no=86, decl_uid=4124, cgraph_uid=33, symbol_order=3086)

dispose (struct BZ2StreamScanner * const this)
{
  unsigned char * _5;
  char * _6;

  <bb 2> [100.00%]:
  _6 = MEM[(char * &)this_1(D) + 128];
  if (_6 != 0B)
    goto <bb 3>; [53.47%]
  else
    goto <bb 4>; [46.53%]

  <bb 3> [53.47%]:
  operator delete [] (_6);
  MEM[(char * &)this_1(D) + 128] = 0B;

  <bb 4> [100.00%]:
  this_1(D)->_outBuff.bufSize = 0;
  _5 = MEM[(unsigned char * &)this_1(D) + 88];
  if (_5 != 0B)
    goto <bb 5>; [57.44%]
  else
    goto <bb 6>; [42.56%]

  <bb 5> [57.44%]:
  operator delete [] (_5);
  MEM[(unsigned char * &)this_1(D) + 88] = 0B;

  <bb 6> [100.00%]:
  this_1(D)->_inBuffCapacity = 0;
  return;

}



;; Function __base_dtor  (_ZN6pbzip216BZ2StreamScannerD2Ev, funcdef_no=87, decl_uid=4122, cgraph_uid=31, symbol_order=3088)

__base_dtor  (struct BZ2StreamScanner * const this)
{
  unsigned char * _6;
  const unsigned char[16] * _7;
  unsigned char * _8;
  const unsigned char[16] * _9;

  <bb 2> [100.00%]:
  this_2(D)->_vptr.BZ2StreamScanner = &MEM[(void *)&_ZTVN6pbzip216BZ2StreamScannerE + 16B];
  dispose (this_2(D));
  _8 = MEM[(const struct basic_string *)this_2(D) + 48B]._M_dataplus._M_p;
  _9 = &MEM[(const struct basic_string *)this_2(D) + 48B].D.4058._M_local_buf;
  if (_8 != _9)
    goto <bb 3>; [53.47%]
  else
    goto <bb 4>; [46.53%]

  <bb 3> [53.47%]:
  operator delete (_8);

  <bb 4> [100.00%]:
  MEM[(struct  &)this_2(D) + 48] ={v} {CLOBBER};
  _6 = MEM[(const struct basic_string *)this_2(D) + 16B]._M_dataplus._M_p;
  _7 = &MEM[(const struct basic_string *)this_2(D) + 16B].D.4058._M_local_buf;
  if (_6 != _7)
    goto <bb 5>; [57.44%]
  else
    goto <bb 6>; [42.56%]

  <bb 5> [57.44%]:
  operator delete (_6);

  <bb 6> [100.00%]:
  MEM[(struct  &)this_2(D)] ={v} {CLOBBER};
  return;

}



;; Function __deleting_dtor  (_ZN6pbzip216BZ2StreamScannerD0Ev, funcdef_no=88, decl_uid=4121, cgraph_uid=30, symbol_order=3090)

__deleting_dtor  (struct BZ2StreamScanner * const this)
{
  <bb 2> [100.00%]:
  __comp_dtor  (this_2(D));
  operator delete (this_2(D), 208);
  return;

}



;; Function ends_with_icase (_Z15ends_with_icaseIcEbRKNSt7__cxx1112basic_stringIT_St11char_traitsIS2_ESaIS2_EEES8_, funcdef_no=80, decl_uid=4169, cgraph_uid=40, symbol_order=1055)

ends_with_icase (const struct basic_string & str, const struct basic_string & suffix)
{
  size_t si;
  int ti;
  unsigned int _1;
  unsigned int _2;
  unsigned int _3;
  long unsigned int _4;
  char _5;
  int _6;
  int _7;
  char _8;
  int _9;
  int _10;
  bool _13;
  const value_type & _18;
  char * _20;
  long unsigned int _22;
  long unsigned int _23;
  char * _26;
  const value_type & _27;

  <bb 2> [21.35%]:
  _22 = str_17(D)->_M_string_length;
  _1 = (unsigned int) _22;
  _23 = suffix_19(D)->_M_string_length;
  _2 = (unsigned int) _23;
  _3 = _1 - _2;
  ti_21 = (int) _3;
  if (ti_21 < 0)
    goto <bb 6>; [32.39%]
  else
    goto <bb 3>; [67.61%]

  <bb 3> [14.40%]:
  # ti_29 = PHI <ti_21(2)>
  # si_30 = PHI <0(2)>
  if (si_30 < _23)
    goto <bb 4>; [92.50%]
  else
    goto <bb 6>; [7.50%]

  <bb 4> [92.50%]:
  # ti_31 = PHI <ti_25(5), ti_29(3)>
  # si_32 = PHI <si_24(5), si_30(3)>
  _4 = (long unsigned int) ti_31;
  _26 = str_17(D)->_M_dataplus._M_p;
  _27 = _26 + _4;
  _5 = *_27;
  _6 = (int) _5;
  _7 = tolower (_6);
  _20 = suffix_19(D)->_M_dataplus._M_p;
  _18 = _20 + si_32;
  _8 = *_18;
  _9 = (int) _8;
  _10 = tolower (_9);
  if (_7 != _10)
    goto <bb 6>; [7.50%]
  else
    goto <bb 5>; [92.50%]

  <bb 5> [85.60%]:
  si_24 = si_32 + 1;
  ti_25 = ti_31 + 1;
  if (_23 > si_24)
    goto <bb 4>; [92.50%]
  else
    goto <bb 6>; [7.50%]

  <bb 6> [21.35%]:
  # _13 = PHI <0(2), 1(5), 0(4), 1(3)>
  return _13;

}



;; Function do_close (_Z8do_closei, funcdef_no=5, decl_uid=4331, cgraph_uid=251, symbol_order=738)

do_close (int fd)
{
  int _5;
  struct ErrorContext * _7;

  <bb 2> [100.00%]:
  _5 = close (fd_3(D));
  if (_5 == -1)
    goto <bb 3>; [22.95%]
  else
    goto <bb 4>; [77.05%]

  <bb 3> [22.95%]:
  _7 = getInstance ();
  saveError (_7);

  <bb 4> [100.00%]:
  return _5;

}



;; Function do_remove (_Z9do_removePKc, funcdef_no=6, decl_uid=4330, cgraph_uid=250, symbol_order=742)

do_remove (const char * pathname)
{
  int ret;
  struct ErrorContext * _7;

  <bb 2> [100.00%]:
  ret_5 = remove (pathname_3(D));
  if (ret_5 == -1)
    goto <bb 3>; [22.95%]
  else
    goto <bb 4>; [77.05%]

  <bb 3> [22.95%]:
  _7 = getInstance ();
  saveError (_7);

  <bb 4> [100.00%]:
  return ret_5;

}



;; Function safe_open_output (_Z16safe_open_outputPKc, funcdef_no=3, decl_uid=4333, cgraph_uid=253, symbol_order=736)

safe_open_output (const char * path)
{
  struct ErrorContext * _5;
  int _7;

  <bb 2> [100.00%]:
  _7 = __open_alias (path_3(D), 193, 384);
  if (_7 == -1)
    goto <bb 3>; [22.95%]
  else
    goto <bb 4>; [77.05%]

  <bb 3> [22.95%]:
  _5 = getInstance ();
  saveError (_5);

  <bb 4> [100.00%]:
  return _7;

}



;; Function do_read (_Z7do_readiPvm, funcdef_no=2, decl_uid=4334, cgraph_uid=254, symbol_order=735)

do_read (int fd, void * buf, size_t count)
{
  char * pbuf;
  ssize_t nbytes;
  ssize_t bytesRemaining;
  long unsigned int _1;
  sizetype _2;
  long unsigned int _3;
  long unsigned int _4;
  long int _9;
  long int _18;
  struct ErrorContext * _20;
  long int _28;

  <bb 2> [14.44%]:
  bytesRemaining_14 = (ssize_t) count_13(D);
  if (bytesRemaining_14 > 0)
    goto <bb 3>; [92.50%]
  else
    goto <bb 5>; [7.50%]

  <bb 3> [92.50%]:
  # bytesRemaining_26 = PHI <bytesRemaining_22(4), bytesRemaining_14(2)>
  # pbuf_27 = PHI <pbuf_23(4), buf_15(D)(2)>
  _1 = (long unsigned int) bytesRemaining_26;
  _28 = __read_alias (fd_17(D), pbuf_27, _1);
  if (_28 > 0)
    goto <bb 4>; [92.50%]
  else
    goto <bb 5>; [7.50%]

  <bb 4> [85.60%]:
  bytesRemaining_22 = bytesRemaining_26 - _28;
  _2 = (sizetype) _28;
  pbuf_23 = pbuf_27 + _2;
  if (bytesRemaining_22 > 0)
    goto <bb 3>; [92.50%]
  else
    goto <bb 5>; [7.50%]

  <bb 5> [14.44%]:
  # nbytes_7 = PHI <_28(4), _28(3), 0(2)>
  # bytesRemaining_36 = PHI <bytesRemaining_22(4), bytesRemaining_26(3), bytesRemaining_14(2)>
  if (nbytes_7 < 0)
    goto <bb 6>; [19.09%]
  else
    goto <bb 7>; [80.91%]

  <bb 6> [2.76%]:
  _20 = getInstance ();
  saveError (_20);
  goto <bb 8>; [100.00%]

  <bb 7> [11.68%]:
  _3 = (long unsigned int) bytesRemaining_36;
  _4 = count_13(D) - _3;
  _18 = (long int) _4;

  <bb 8> [14.44%]:
  # _9 = PHI <nbytes_7(6), _18(7)>
  return _9;

}



;; Function testBZ2ErrorHandling (_Z20testBZ2ErrorHandlingiPvi, funcdef_no=22, decl_uid=4293, cgraph_uid=127, symbol_order=786)

testBZ2ErrorHandling (int bzerr, BZFILE * bzf, int streamNo)
{
  int bzerr_dummy;
  struct FILE * _1;
  struct FILE * _2;
  struct FILE * _3;
  struct FILE * _4;
  struct FILE * _5;
  struct FILE * _6;
  struct FILE * _7;
  struct FILE * _8;
  int _9;

  <bb 2> [100.00%]:
  BZ2_bzReadClose (&bzerr_dummy, bzf_12(D));
  switch (bzerr_14(D)) <default: <L6> [0.04%], case -9: <L0> [0.04%], case -7: <L4> [20.00%], case -6: <L1> [20.00%], case -5: <L5> [20.00%], case -4: <L2> [20.00%], case -3: <L3> [20.00%]>

<L0> [0.04%]:
  _1 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Integers are not the right size for libbzip2. Aborting!\n", 1, 72, _1);
  exit (3);

<L1> [20.00%]:
  _2 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Integers are not the right size for libbzip2. Aborting!\n", 1, 72, _2);
  goto <bb 12>; [100.00%]

<L2> [20.00%]:
  _3 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Data integrity (CRC) error in data!  Skipping...\n", 1, 65, _3);
  goto <bb 12>; [100.00%]

<L3> [20.00%]:
  _4 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Could NOT allocate enough memory. Aborting!\n", 1, 60, _4);
  goto <bb 12>; [100.00%]

<L4> [20.00%]:
  _5 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: File ends unexpectedly!  Skipping...\n", 1, 53, _5);
  goto <bb 12>; [100.00%]

<L5> [20.00%]:
  if (streamNo_15(D) == 1)
    goto <bb 9>; [1.04%]
  else
    goto <bb 10>; [98.96%]

  <bb 9> [0.21%]:
  _6 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Bad magic number (file not created by bzip2)!  Skipping...\n", 1, 75, _6);
  goto <bb 12>; [100.00%]

  <bb 10> [19.79%]:
  _7 = stderr;
  __builtin_fwrite ("pbzip2: *WARNING: Trailing garbage after EOF ignored!\n", 1, 54, _7);
  goto <bb 12>; [100.00%]

<L6> [0.04%]:
  _8 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Unexpected error. Aborting!\n", 1, 44, _8);
  exit (3);

  <bb 12> [100.00%]:
  # _9 = PHI <1(4), -1(5), 1(6), -1(7), -1(9), 0(10)>
  bzerr_dummy ={v} {CLOBBER};
  return _9;

}



;; Function usage (_Z5usagePcPKc, funcdef_no=25, decl_uid=4289, cgraph_uid=124, symbol_order=792) (executed once)

usage (char * progname, const char * reason)
{
  int ncpu;
  int D.5066;
  int _1;
  struct FILE * _2;
  struct FILE * _3;
  struct FILE * _4;
  struct FILE * _5;
  struct FILE * _6;
  struct FILE * _7;
  struct FILE * _8;
  struct FILE * _9;
  struct FILE * _10;
  struct FILE * _11;
  struct FILE * _12;
  struct FILE * _13;
  struct FILE * _14;
  struct FILE * _16;
  struct FILE * _17;
  struct FILE * _18;
  struct FILE * _19;
  struct FILE * _20;
  struct FILE * _21;
  struct FILE * _22;
  struct FILE * _23;
  struct FILE * _24;
  struct FILE * _25;
  struct FILE * _26;
  struct FILE * _27;
  struct FILE * _28;
  struct FILE * _29;
  struct FILE * _30;
  struct FILE * _31;
  struct FILE * _32;
  struct FILE * _33;
  struct FILE * _34;
  long int _77;

  <bb 2> [100.00%]:
  banner ();
  _1 = strncmp (reason_38(D), "HELP", 4);
  if (_1 == 0)
    goto <bb 3>; [50.00%]
  else
    goto <bb 4>; [50.00%]

  <bb 3> [50.00%]:
  _2 = stderr;
  __builtin_fputc (10, _2);
  goto <bb 5>; [100.00%]

  <bb 4> [50.00%]:
  _3 = stderr;
  __fprintf_chk (_3, 1, "\nInvalid command line: %s.  Aborting...\n\n", reason_38(D));

  <bb 5> [100.00%]:
  _4 = stderr;
  __fprintf_chk (_4, 1, "Usage: %s [-1 .. -9] [-b#cdfhklm#p#qrS#tVz] <filename> <filename2> <filenameN>\n", progname_39(D));
  _5 = stderr;
  __builtin_fwrite (" -1 .. -9        set BWT block size to 100k .. 900k (default 900k)\n", 1, 67, _5);
  _6 = stderr;
  __builtin_fwrite (" -b#             Block size in 100k steps (default 9 = 900k)\n", 1, 61, _6);
  _7 = stderr;
  __builtin_fwrite (" -c,--stdout     Output to standard out (stdout)\n", 1, 49, _7);
  _8 = stderr;
  __builtin_fwrite (" -d,--decompress Decompress file\n", 1, 33, _8);
  _9 = stderr;
  __builtin_fwrite (" -f,--force      Overwrite existing output file\n", 1, 48, _9);
  _10 = stderr;
  __builtin_fwrite (" -h,--help       Print this help message\n", 1, 41, _10);
  _11 = stderr;
  __builtin_fwrite (" -k,--keep       Keep input file, don\'t delete\n", 1, 47, _11);
  _12 = stderr;
  __builtin_fwrite (" -l,--loadavg    Load average determines max number processors to use\n", 1, 70, _12);
  _13 = stderr;
  __builtin_fwrite (" -m#             Maximum memory usage in 1MB steps (default 100 = 100MB)\n", 1, 73, _13);
  _14 = stderr;
  __builtin_fwrite (" -p#             Number of processors to use (default", 1, 53, _14);
  _77 = sysconf (84);
  ncpu_78 = (int) _77;
  if (ncpu_78 <= 0)
    goto <bb 6>; [39.77%]
  else
    goto <bb 7>; [60.23%]

  <bb 6> [39.77%]:

  <bb 7> [100.00%]:
  # ncpu_79 = PHI <ncpu_78(5), 1(6)>
  _16 = stderr;
  __fprintf_chk (_16, 1, ": autodetect [%d])\n", ncpu_79);
  _17 = stderr;
  __builtin_fwrite (" -q,--quiet      Quiet mode (default)\n", 1, 38, _17);
  _18 = stderr;
  __builtin_fwrite (" -r,--read       Read entire input file into RAM and split between processors\n", 1, 78, _18);
  _19 = stderr;
  __builtin_fwrite (" -S#             Child thread stack size in 1KB steps (default stack size if unspecified)\n", 1, 90, _19);
  _20 = stderr;
  __builtin_fwrite (" -t,--test       Test compressed file integrity\n", 1, 48, _20);
  _21 = stderr;
  __builtin_fwrite (" -v,--verbose    Verbose mode\n", 1, 30, _21);
  _22 = stderr;
  __builtin_fwrite (" -V,--version    Display version info for pbzip2 then exit\n", 1, 59, _22);
  _23 = stderr;
  __builtin_fwrite (" -z,--compress   Compress file (default)\n", 1, 41, _23);
  _24 = stderr;
  __builtin_fwrite (" --ignore-trailing-garbage=# Ignore trailing garbage flag (1 - ignored; 0 - forbidden)\n", 1, 87, _24);
  _25 = stderr;
  __builtin_fputc (10, _25);
  _26 = stderr;
  __builtin_fwrite ("If no file names are given, pbzip2 compresses or decompresses from standard input to standard output.\n", 1, 102, _26);
  _27 = stderr;
  __builtin_fputc (10, _27);
  _28 = stderr;
  __builtin_fwrite ("Example: pbzip2 -b15vk myfile.tar\n", 1, 34, _28);
  _29 = stderr;
  __builtin_fwrite ("Example: pbzip2 -p4 -r -5 myfile.tar second*.txt\n", 1, 49, _29);
  _30 = stderr;
  __builtin_fwrite ("Example: tar cf myfile.tar.bz2 --use-compress-prog=pbzip2 dir_to_compress/\n", 1, 75, _30);
  _31 = stderr;
  __builtin_fwrite ("Example: pbzip2 -d -m500 myfile.tar.bz2\n", 1, 40, _31);
  _32 = stderr;
  __builtin_fwrite ("Example: pbzip2 -dc myfile.tar.bz2 | tar x\n", 1, 43, _32);
  _33 = stderr;
  __builtin_fwrite ("Example: pbzip2 -c < myfile.txt > myfile.txt.bz2 \n", 1, 50, _33);
  _34 = stderr;
  __builtin_fputc (10, _34);
  exit (-1);

}



;; Function writeFileMetaData (_Z17writeFileMetaDataPKc, funcdef_no=23, decl_uid=4291, cgraph_uid=126, symbol_order=789)

writeFileMetaData (const char * fileName)
{
  struct utimbuf uTimBuf;
  int ret;
  long int _1;
  long int _2;
  unsigned int _3;
  unsigned int _4;
  unsigned int _5;
  unsigned int _6;
  int _7;
  struct ErrorContext * _20;
  struct ErrorContext * _24;
  struct ErrorContext * _27;

  <bb 2> [100.00%]:
  _1 = fileMetaData.st_atim.tv_sec;
  uTimBuf.actime = _1;
  _2 = fileMetaData.st_mtim.tv_sec;
  uTimBuf.modtime = _2;
  _3 = fileMetaData.st_mode;
  ret_14 = chmod (fileName_12(D), _3);
  if (ret_14 != 0)
    goto <bb 3>; [46.00%]
  else
    goto <bb 4>; [54.00%]

  <bb 3> [46.00%]:
  _27 = getInstance ();
  saveError (_27);
  goto <bb 9>; [100.00%]

  <bb 4> [54.00%]:
  ret_16 = utime (fileName_12(D), &uTimBuf);
  if (ret_16 != 0)
    goto <bb 5>; [46.00%]
  else
    goto <bb 6>; [54.00%]

  <bb 5> [24.84%]:
  _24 = getInstance ();
  saveError (_24);
  goto <bb 9>; [100.00%]

  <bb 6> [29.16%]:
  _4 = fileMetaData.st_gid;
  _5 = fileMetaData.st_uid;
  ret_18 = chown (fileName_12(D), _5, _4);
  if (ret_18 != 0)
    goto <bb 7>; [36.64%]
  else
    goto <bb 9>; [63.36%]

  <bb 7> [10.68%]:
  _20 = getInstance ();
  saveError (_20);
  _6 = geteuid ();
  if (_6 == 0)
    goto <bb 9>; [46.00%]
  else
    goto <bb 8>; [54.00%]

  <bb 8> [5.77%]:

  <bb 9> [100.00%]:
  # _7 = PHI <ret_14(3), ret_16(5), 0(6), ret_18(7), 0(8)>
  uTimBuf ={v} {CLOBBER};
  return _7;

}



;; Function safe_fopen_output (_Z17safe_fopen_outputPKcS0_, funcdef_no=4, decl_uid=4332, cgraph_uid=252, symbol_order=737) (unlikely executed)

safe_fopen_output (const char * path, const char * mode)
{
  struct FILE * fp;
  struct FILE * _1;
  int _6;
  struct ErrorContext * _11;

  <bb 2> [100.00%]:
  _6 = safe_open_output (path_4(D));
  if (_6 == -1)
    goto <bb 6>; [9.37%]
  else
    goto <bb 3>; [90.63%]

  <bb 3> [90.63%]:
  fp_9 = fdopen (_6, mode_7(D));
  if (fp_9 == 0B)
    goto <bb 4>; [19.86%]
  else
    goto <bb 6>; [80.14%]

  <bb 4> [18.00%]:
  _11 = getInstance ();
  saveError (_11);
  close (_6);

  <bb 6> [100.00%]:
  # _1 = PHI <0B(2), fp_9(3), fp_9(4)>
  return _1;

}



;; Function do_write (_Z8do_writeiPKvm, funcdef_no=1, decl_uid=4335, cgraph_uid=255, symbol_order=734)

do_write (int fd, const void * buf, size_t count)
{
  const char * pbuf;
  ssize_t nbytes;
  ssize_t bytesRemaining;
  long unsigned int _1;
  sizetype _2;
  long unsigned int _3;
  long unsigned int _4;
  long int _9;
  long int _19;
  long int _20;
  struct ErrorContext * _22;

  <bb 2> [14.44%]:
  bytesRemaining_14 = (ssize_t) count_13(D);
  if (bytesRemaining_14 > 0)
    goto <bb 3>; [92.50%]
  else
    goto <bb 5>; [7.50%]

  <bb 3> [92.50%]:
  # bytesRemaining_37 = PHI <bytesRemaining_24(4), bytesRemaining_14(2)>
  # pbuf_39 = PHI <pbuf_25(4), buf_15(D)(2)>
  _1 = (long unsigned int) bytesRemaining_37;
  _19 = write (fd_17(D), pbuf_39, _1);
  if (_19 > 0)
    goto <bb 4>; [92.50%]
  else
    goto <bb 5>; [7.50%]

  <bb 4> [85.60%]:
  bytesRemaining_24 = bytesRemaining_37 - _19;
  _2 = (sizetype) _19;
  pbuf_25 = pbuf_39 + _2;
  if (bytesRemaining_24 > 0)
    goto <bb 3>; [92.50%]
  else
    goto <bb 5>; [7.50%]

  <bb 5> [14.44%]:
  # nbytes_7 = PHI <_19(4), _19(3), 0(2)>
  # bytesRemaining_38 = PHI <bytesRemaining_24(4), bytesRemaining_37(3), bytesRemaining_14(2)>
  if (nbytes_7 < 0)
    goto <bb 6>; [19.09%]
  else
    goto <bb 7>; [80.91%]

  <bb 6> [2.76%]:
  _22 = getInstance ();
  saveError (_22);
  goto <bb 8>; [100.00%]

  <bb 7> [11.68%]:
  _3 = (long unsigned int) bytesRemaining_38;
  _4 = count_13(D) - _3;
  _20 = (long int) _4;

  <bb 8> [14.44%]:
  # _9 = PHI <nbytes_7(6), _20(7)>
  return _9;

}



;; Function cleanupAndQuit (_Z14cleanupAndQuiti, funcdef_no=16, decl_uid=4318, cgraph_uid=205, symbol_order=767) (executed once)

cleanupAndQuit (int exitCode)
{
  struct stat statBuf;
  int ret;
  int ret;
  struct FILE * _1;
  int _16;
  const char * _17;
  const char * _18;
  int _19;
  int _20;
  struct FILE * _21;
  const char * _22;
  int _23;
  int _24;
  const char * _25;
  struct FILE * _26;
  const char * _27;
  int * _29;
  int _30;
  struct FILE * _31;
  struct FILE * _32;
  struct FILE * _33;
  struct FILE * _34;
  const char * _35;
  struct FILE * _36;

  <bb 2> [100.00%]:
  ret_5 = pthread_mutex_lock (&ErrorHandlerMutex);
  if (ret_5 != 0)
    goto <bb 3>; [33.00%]
  else
    goto <bb 4>; [67.00%]

  <bb 3> [33.00%]:
  _1 = stderr;
  __builtin_fwrite ("Cleanup Handler: Failed to lock ErrorHandlerMutex! May double cleanup...\n", 1, 73, _1);

  <bb 4> [100.00%]:
  _16 = unfinishedWorkCleaned;
  if (_16 != 0)
    goto <bb 19>; [50.00%]
  else
    goto <bb 5>; [50.00%]

  <bb 5> [50.00%]:
  _17 = sigInFilename;
  if (_17 == 0B)
    goto <bb 8>; [30.00%]
  else
    goto <bb 6>; [70.00%]

  <bb 6> [35.00%]:
  _18 = sigOutFilename;
  if (_18 == 0B)
    goto <bb 8>; [30.00%]
  else
    goto <bb 7>; [70.00%]

  <bb 7> [24.50%]:
  _19 = OutputStdOut;
  if (_19 == 1)
    goto <bb 8>; [34.00%]
  else
    goto <bb 9>; [66.00%]

  <bb 8> [33.83%]:
  unfinishedWorkCleaned = 1;
  goto <bb 19>; [100.00%]

  <bb 9> [16.17%]:
  _20 = QuietMode;
  if (_20 != 1)
    goto <bb 10>; [48.88%]
  else
    goto <bb 11>; [51.12%]

  <bb 10> [7.90%]:
  _21 = stderr;
  __fprintf_chk (_21, 1, "Cleanup unfinished work [Outfile: %s]...\n", _18);

  <bb 11> [16.17%]:
  _22 = sigInFilename;
  _23 = __xstat (1, _22, &statBuf);
  if (_23 == 0)
    goto <bb 12>; [67.00%]
  else
    goto <bb 17>; [33.00%]

  <bb 12> [10.83%]:
  _24 = QuietMode;
  if (_24 != 1)
    goto <bb 13>; [48.88%]
  else
    goto <bb 14>; [51.12%]

  <bb 13> [5.30%]:
  _25 = sigOutFilename;
  _26 = stderr;
  __fprintf_chk (_26, 1, "Deleting output file: %s, if it exists...\n", _25);

  <bb 14> [10.83%]:
  _27 = sigOutFilename;
  ret_28 = remove (_27);
  if (ret_28 != 0)
    goto <bb 15>; [50.00%]
  else
    goto <bb 16>; [50.00%]

  <bb 15> [5.42%]:
  _29 = __errno_location ();
  _30 = *_29;
  _31 = stderr;
  syncPrintErrnoMsg (_31, _30);
  _32 = stderr;
  __builtin_fwrite ("pbzip2:  *WARNING: Deletion of output file (apparently) failed.\n", 1, 64, _32);
  goto <bb 18>; [100.00%]

  <bb 16> [5.42%]:
  _33 = stderr;
  __builtin_fwrite ("pbzip2:  *INFO: Deletion of output file succeeded.\n", 1, 51, _33);
  sigOutFilename = 0B;
  goto <bb 18>; [100.00%]

  <bb 17> [5.34%]:
  _34 = stderr;
  __builtin_fwrite ("pbzip2:  *WARNING: Output file was not deleted since input file no longer exists.\n", 1, 82, _34);
  _35 = sigOutFilename;
  _36 = stderr;
  __fprintf_chk (_36, 1, "pbzip2:  *WARNING: Output file: %s, may be incomplete!\n", _35);

  <bb 18> [16.17%]:
  unfinishedWorkCleaned = 1;
  statBuf ={v} {CLOBBER};
  goto <bb 20>; [100.00%]

  <bb 19> [83.83%]:
  statBuf ={v} {CLOBBER};

  <bb 20> [100.00%]:
  pthread_mutex_unlock (&ErrorHandlerMutex);
  exit (exitCode_8(D));

}



;; Function syncSetTerminateFlag (_Z20syncSetTerminateFlagi, funcdef_no=8, decl_uid=4320, cgraph_uid=207, symbol_order=747) (unlikely executed)

syncSetTerminateFlag (int newValue)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  union pthread_mutex_t * _1;
  union pthread_cond_t * _2;
  union pthread_mutex_t * _3;
  struct queue * _4;
  union pthread_mutex_t * _5;
  struct queue * _6;
  union pthread_cond_t * _7;
  struct queue * _8;
  union pthread_cond_t * _9;
  struct queue * _10;
  union pthread_mutex_t * _11;
  struct FILE * _31;
  struct FILE * _33;
  struct FILE * _35;
  struct FILE * _37;
  struct FILE * _39;
  struct FILE * _41;
  struct FILE * _43;

  <bb 2> [100.00%]:
  ret_30 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_30 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.04%]:
  _31 = stderr;
  __fprintf_chk (_31, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_30);
  cleanupAndQuit (-5);

  <bb 4> [99.96%]:
  terminateFlag = newValue_15(D);
  if (newValue_15(D) != 0)
    goto <bb 5>; [50.00%]
  else
    goto <bb 16>; [50.00%]

  <bb 5> [49.98%]:
  pthread_cond_signal (&TerminateCond);
  ret_36 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_36 != 0)
    goto <bb 6>; [0.04%]
  else
    goto <bb 7>; [99.96%]

  <bb 6> [0.02%]:
  _37 = stderr;
  __fprintf_chk (_37, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_36);
  cleanupAndQuit (-6);

  <bb 7> [49.96%]:
  _1 = OutMutex;
  ret_34 = pthread_mutex_lock (_1);
  if (ret_34 != 0)
    goto <bb 8>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 8> [0.02%]:
  _35 = stderr;
  __fprintf_chk (_35, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_34);
  cleanupAndQuit (-5);

  <bb 9> [49.94%]:
  _2 = notTooMuchNumBuffered;
  pthread_cond_broadcast (_2);
  _3 = OutMutex;
  ret_32 = pthread_mutex_unlock (_3);
  if (ret_32 != 0)
    goto <bb 10>; [0.04%]
  else
    goto <bb 11>; [99.96%]

  <bb 10> [0.02%]:
  _33 = stderr;
  __fprintf_chk (_33, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_32);
  cleanupAndQuit (-6);

  <bb 11> [49.92%]:
  _4 = FifoQueue;
  if (_4 != 0B)
    goto <bb 12>; [53.47%]
  else
    goto <bb 18>; [46.53%]

  <bb 12> [26.69%]:
  _5 = _4->mut;
  ret_40 = pthread_mutex_lock (_5);
  if (ret_40 != 0)
    goto <bb 13>; [0.04%]
  else
    goto <bb 14>; [99.96%]

  <bb 13> [0.01%]:
  _41 = stderr;
  __fprintf_chk (_41, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_40);
  cleanupAndQuit (-5);

  <bb 14> [26.68%]:
  _6 = FifoQueue;
  _7 = _6->notFull;
  pthread_cond_broadcast (_7);
  _8 = FifoQueue;
  _9 = _8->notEmpty;
  pthread_cond_broadcast (_9);
  _10 = FifoQueue;
  _11 = _10->mut;
  ret_38 = pthread_mutex_unlock (_11);
  if (ret_38 != 0)
    goto <bb 15>; [0.04%]
  else
    goto <bb 18>; [99.96%]

  <bb 15> [0.01%]:
  _39 = stderr;
  __fprintf_chk (_39, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_38);
  cleanupAndQuit (-6);

  <bb 16> [49.98%]:
  ret_42 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_42 != 0)
    goto <bb 17>; [0.04%]
  else
    goto <bb 18>; [99.96%]

  <bb 17> [0.02%]:
  _43 = stderr;
  __fprintf_chk (_43, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_42);
  cleanupAndQuit (-6);

  <bb 18> [99.86%]:
  return;

}



;; Function handle_error (_Z12handle_error8ExitFlagiPKcz, funcdef_no=7, decl_uid=4329, cgraph_uid=239, symbol_order=753)

handle_error (ExitFlag exitFlag, int exitCode, const char * fmt)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  struct  args[1];
  struct FILE * _1;
  struct FILE * _2;
  struct FILE * _3;
  struct ErrorContext * _9;
  union pthread_mutex_t * _28;
  union pthread_cond_t * _29;
  union pthread_mutex_t * _30;
  struct queue * _31;
  union pthread_mutex_t * _32;
  struct queue * _33;
  union pthread_cond_t * _34;
  struct queue * _35;
  union pthread_cond_t * _36;
  struct queue * _37;
  union pthread_mutex_t * _38;
  struct FILE * _40;
  struct FILE * _42;
  struct FILE * _44;
  struct FILE * _46;
  struct FILE * _48;
  struct FILE * _50;

  <bb 2> [100.00%]:
  __builtin_va_start (&args, 0);
  _1 = stderr;
  __vfprintf_chk (_1, 1, fmt_7(D), &args);
  _9 = getInstance ();
  _2 = stderr;
  printErrorMessages (_9, _2);
  _3 = stderr;
  fflush (_3);
  __builtin_va_end (&args);
  if (exitFlag_13(D) == 2)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.04%]:
  syncSetTerminateFlag (1);
  abort ();

  <bb 4> [99.96%]:
  if (exitFlag_13(D) == 1)
    goto <bb 5>; [22.95%]
  else
    goto <bb 18>; [77.05%]

  <bb 5> [22.94%]:
  ret_39 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_39 != 0)
    goto <bb 6>; [0.04%]
  else
    goto <bb 7>; [99.96%]

  <bb 6> [0.01%]:
  _40 = stderr;
  __fprintf_chk (_40, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_39);
  cleanupAndQuit (-5);

  <bb 7> [22.93%]:
  terminateFlag = 1;
  pthread_cond_signal (&TerminateCond);
  ret_45 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_45 != 0)
    goto <bb 8>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 8> [0.01%]:
  _46 = stderr;
  __fprintf_chk (_46, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_45);
  cleanupAndQuit (-6);

  <bb 9> [22.92%]:
  _28 = OutMutex;
  ret_43 = pthread_mutex_lock (_28);
  if (ret_43 != 0)
    goto <bb 10>; [0.04%]
  else
    goto <bb 11>; [99.96%]

  <bb 10> [0.01%]:
  _44 = stderr;
  __fprintf_chk (_44, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_43);
  cleanupAndQuit (-5);

  <bb 11> [22.91%]:
  _29 = notTooMuchNumBuffered;
  pthread_cond_broadcast (_29);
  _30 = OutMutex;
  ret_41 = pthread_mutex_unlock (_30);
  if (ret_41 != 0)
    goto <bb 12>; [0.04%]
  else
    goto <bb 13>; [99.96%]

  <bb 12> [0.01%]:
  _42 = stderr;
  __fprintf_chk (_42, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_41);
  cleanupAndQuit (-6);

  <bb 13> [22.90%]:
  _31 = FifoQueue;
  if (_31 != 0B)
    goto <bb 14>; [53.47%]
  else
    goto <bb 18>; [46.53%]

  <bb 14> [12.25%]:
  _32 = _31->mut;
  ret_49 = pthread_mutex_lock (_32);
  if (ret_49 != 0)
    goto <bb 15>; [0.04%]
  else
    goto <bb 16>; [99.96%]

  <bb 15> [0.00%]:
  _50 = stderr;
  __fprintf_chk (_50, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_49);
  cleanupAndQuit (-5);

  <bb 16> [12.24%]:
  _33 = FifoQueue;
  _34 = _33->notFull;
  pthread_cond_broadcast (_34);
  _35 = FifoQueue;
  _36 = _35->notEmpty;
  pthread_cond_broadcast (_36);
  _37 = FifoQueue;
  _38 = _37->mut;
  ret_47 = pthread_mutex_unlock (_38);
  if (ret_47 != 0)
    goto <bb 17>; [0.04%]
  else
    goto <bb 18>; [99.96%]

  <bb 17> [0.00%]:
  _48 = stderr;
  __fprintf_chk (_48, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_47);
  cleanupAndQuit (-6);

  <bb 18> [99.91%]:
  args ={v} {CLOBBER};
  return exitCode_15(D);

}



;; Function safe_cond_timed_wait (_Z20safe_cond_timed_waitP14pthread_cond_tP15pthread_mutex_tiPKc, funcdef_no=0, decl_uid=4336, cgraph_uid=256, symbol_order=733)

safe_cond_timed_wait (union pthread_cond_t * cond, union pthread_mutex_t * mutex, int seconds, const char * caller)
{
  struct timezone tz;
  struct timeval tv;
  struct timespec waitTimer;
  long int _1;
  long int _2;
  long int _3;
  long int _4;
  long int _5;
  bool _6;
  bool _7;
  bool _8;
  bool _9;
  bool _10;
  bool _11;
  int _20;
  struct ErrorContext * _22;

  <bb 2> [100.00%]:
  gettimeofday (&tv, &tz);
  _1 = tv.tv_sec;
  _2 = (long int) seconds_14(D);
  _3 = _1 + _2;
  waitTimer.tv_sec = _3;
  _4 = tv.tv_usec;
  _5 = _4 * 1000;
  waitTimer.tv_nsec = _5;
  _20 = pthread_cond_timedwait (cond_17(D), mutex_18(D), &waitTimer);
  _6 = _20 != 0;
  _7 = _20 != 4;
  _8 = _6 & _7;
  if (_8 != 0)
    goto <bb 3>; [54.00%]
  else
    goto <bb 5>; [46.00%]

  <bb 3> [54.00%]:
  _9 = _20 != 16;
  _10 = _20 != 110;
  _11 = _9 & _10;
  if (_11 != 0)
    goto <bb 4>; [0.04%]
  else
    goto <bb 5>; [99.96%]

  <bb 4> [0.02%]:
  _22 = getInstance ();
  saveError (_22);
  pthread_mutex_unlock (mutex_18(D));
  handle_error (1, 1, "pbzip2: *ERROR: %s:  pthread_cond_timedwait() call invalid [pret=%d].  This machine\n         does not have compatible pthreads library.  Aborting.\n", caller_25(D), _20);
  cleanupAndQuit (-9);

  <bb 5> [99.98%]:
  waitTimer ={v} {CLOBBER};
  tv ={v} {CLOBBER};
  tz ={v} {CLOBBER};
  return 0;

}



;; Function decompressErrCheckSingle (_Z24decompressErrCheckSingleiPK7outBuffiRK9bz_streamPKcb, funcdef_no=11, decl_uid=4321, cgraph_uid=223, symbol_order=757)

decompressErrCheckSingle (int bzret, const struct outBuff * fileData, int outSequenceNumber, const struct bz_stream & strm, const char * errmsg, bool isTrailingGarbageErr)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  bool changed;
  int ret;
  int D.4943;
  int _1;
  int _2;
  struct FILE * _3;
  int _4;
  int _9;
  bool _12;
  int _17;
  int _18;
  unsigned int _19;
  bool _20;
  int _21;
  unsigned int _22;
  bool _23;
  int _24;
  int _25;
  bool _28;
  bool _29;
  union pthread_mutex_t * _34;
  int _35;
  union pthread_mutex_t * _36;
  int _37;
  union pthread_mutex_t * _38;
  union pthread_mutex_t * _39;
  union pthread_mutex_t * _40;
  int _41;
  union pthread_mutex_t * _42;
  int _43;
  int _44;
  union pthread_cond_t * _46;
  union pthread_mutex_t * _47;
  struct queue * _48;
  union pthread_mutex_t * _49;
  struct queue * _50;
  union pthread_cond_t * _51;
  struct queue * _52;
  union pthread_cond_t * _53;
  struct queue * _54;
  union pthread_mutex_t * _55;
  union pthread_mutex_t * _56;
  struct FILE * _58;
  struct FILE * _60;
  struct FILE * _62;
  struct FILE * _64;
  struct FILE * _66;
  struct FILE * _68;
  struct FILE * _70;
  struct FILE * _72;
  struct FILE * _74;
  struct FILE * _76;
  struct FILE * _78;
  struct FILE * _80;
  struct FILE * _82;
  bool _127;
  bool _134;
  bool _135;
  bool _136;
  bool _137;
  bool _138;

  <bb 2> [13.50%]:
  _1 = fileData_7(D)->blockNumber;
  if (outSequenceNumber_8(D) != -1)
    goto <bb 4>; [66.00%]
  else
    goto <bb 3>; [34.00%]

  <bb 3> [4.59%]:
  _9 = _1 + -1;

  <bb 4> [13.50%]:
  # _17 = PHI <_1(2), _9(3)>
  if (_17 == -1)
    goto <bb 6>; [0.52%]
  else
    goto <bb 5>; [99.48%]

  <bb 5> [13.43%]:
  _18 = IgnoreTrailingGarbageFlag;
  if (_18 == 0)
    goto <bb 6>; [3.98%]
  else
    goto <bb 7>; [96.02%]

  <bb 6> [0.60%]:
  _19 = strm_15(D)->avail_in;
  _20 = fileData_7(D)->isLastInSequence;
  _21 = (int) _20;
  handle_error (1, -1, "pbzip2: %s: ret=%d; block=%d; seq=%d; isLastInSeq=%d; avail_in=%d\n", errmsg_16(D), bzret_14(D), _1, outSequenceNumber_8(D), _21, _19);
  goto <bb 49>; [100.00%]

  <bb 7> [12.89%]:
  _42 = OutMutex;
  ret_57 = pthread_mutex_lock (_42);
  if (ret_57 != 0)
    goto <bb 8>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 8> [0.01%]:
  _58 = stderr;
  __fprintf_chk (_58, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_57);
  cleanupAndQuit (-5);

  <bb 9> [12.89%]:
  _43 = LastGoodBlock;
  _127 = _17 < _43;
  _12 = _43 == -1;
  _28 = _12 | _127;
  if (_28 != 0)
    goto <bb 10>; [67.00%]
  else
    goto <bb 11>; [33.00%]

  <bb 10> [8.63%]:
  LastGoodBlock = _17;

  <bb 11> [12.89%]:
  # changed_45 = PHI <0(9), 1(10)>
  _44 = MinErrorBlock;
  _29 = _1 < _44;
  _134 = _44 == -1;
  _135 = _29 | _134;
  if (_135 != 0)
    goto <bb 12>; [67.00%]
  else
    goto <bb 13>; [33.00%]

  <bb 12> [8.63%]:
  MinErrorBlock = _1;
  goto <bb 14>; [100.00%]

  <bb 13> [4.25%]:
  if (changed_45 != 0)
    goto <bb 14>; [67.00%]
  else
    goto <bb 25>; [33.00%]

  <bb 14> [11.48%]:
  ret_63 = pthread_cond_signal (&ErrStateChangeCond);
  if (ret_63 != 0)
    goto <bb 15>; [0.04%]
  else
    goto <bb 16>; [99.96%]

  <bb 15> [0.00%]:
  _64 = stderr;
  __fprintf_chk (_64, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_63);
  cleanupAndQuit (-7);

  <bb 16> [11.48%]:
  ret_61 = pthread_cond_signal (&OutBufferHeadNotEmpty);
  if (ret_61 != 0)
    goto <bb 17>; [0.04%]
  else
    goto <bb 18>; [99.96%]

  <bb 17> [0.00%]:
  _62 = stderr;
  __fprintf_chk (_62, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_61);
  cleanupAndQuit (-7);

  <bb 18> [11.47%]:
  _46 = notTooMuchNumBuffered;
  pthread_cond_broadcast (_46);
  _47 = OutMutex;
  ret_59 = pthread_mutex_unlock (_47);
  if (ret_59 != 0)
    goto <bb 19>; [0.04%]
  else
    goto <bb 20>; [99.96%]

  <bb 19> [0.00%]:
  _60 = stderr;
  __fprintf_chk (_60, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_59);
  cleanupAndQuit (-6);

  <bb 20> [11.47%]:
  _48 = FifoQueue;
  if (_48 != 0B)
    goto <bb 21>; [53.47%]
  else
    goto <bb 27>; [46.53%]

  <bb 21> [6.13%]:
  _49 = _48->mut;
  ret_67 = pthread_mutex_lock (_49);
  if (ret_67 != 0)
    goto <bb 22>; [0.04%]
  else
    goto <bb 23>; [99.96%]

  <bb 22> [0.00%]:
  _68 = stderr;
  __fprintf_chk (_68, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_67);
  cleanupAndQuit (-5);

  <bb 23> [6.13%]:
  _50 = FifoQueue;
  _51 = _50->notFull;
  pthread_cond_broadcast (_51);
  _52 = FifoQueue;
  _53 = _52->notEmpty;
  pthread_cond_broadcast (_53);
  _54 = FifoQueue;
  _55 = _54->mut;
  ret_65 = pthread_mutex_unlock (_55);
  if (ret_65 != 0)
    goto <bb 24>; [0.04%]
  else
    goto <bb 27>; [99.96%]

  <bb 24> [0.00%]:
  _66 = stderr;
  __fprintf_chk (_66, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_65);
  cleanupAndQuit (-6);

  <bb 25> [1.40%]:
  _56 = OutMutex;
  ret_69 = pthread_mutex_unlock (_56);
  if (ret_69 != 0)
    goto <bb 26>; [0.04%]
  else
    goto <bb 27>; [99.96%]

  <bb 26> [0.00%]:
  _70 = stderr;
  __fprintf_chk (_70, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_69);
  cleanupAndQuit (-6);

  <bb 27> [12.87%]:
  _2 = fileData_7(D)->blockNumber;

  <bb 28> [100.00%]:
  ret_73 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_73 != 0)
    goto <bb 29>; [0.04%]
  else
    goto <bb 30>; [99.96%]

  <bb 29> [0.04%]:
  _74 = stderr;
  __fprintf_chk (_74, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_73);
  cleanupAndQuit (-5);

  <bb 30> [99.96%]:
  ret_33 = terminateFlag;
  ret_71 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_71 != 0)
    goto <bb 31>; [0.04%]
  else
    goto <bb 32>; [99.96%]

  <bb 31> [0.04%]:
  _72 = stderr;
  __fprintf_chk (_72, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_71);
  cleanupAndQuit (-6);

  <bb 32> [99.92%]:
  if (ret_33 != 0)
    goto <bb 43>; [5.00%]
  else
    goto <bb 33>; [95.00%]

  <bb 33> [94.92%]:
  _34 = OutMutex;
  ret_75 = pthread_mutex_lock (_34);
  if (ret_75 != 0)
    goto <bb 34>; [0.04%]
  else
    goto <bb 35>; [99.96%]

  <bb 34> [0.04%]:
  _76 = stderr;
  __fprintf_chk (_76, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_75);
  cleanupAndQuit (-5);

  <bb 35> [94.89%]:
  _35 = MinErrorBlock;
  _136 = _2 > _35;
  _137 = _35 != -1;
  _138 = _136 & _137;
  if (_138 != 0)
    goto <bb 36>; [3.30%]
  else
    goto <bb 38>; [96.70%]

  <bb 36> [3.13%]:
  _36 = OutMutex;
  ret_77 = pthread_mutex_unlock (_36);
  if (ret_77 != 0)
    goto <bb 37>; [0.04%]
  else
    goto <bb 43>; [99.96%]

  <bb 37> [0.00%]:
  _78 = stderr;
  __fprintf_chk (_78, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_77);
  cleanupAndQuit (-6);

  <bb 38> [91.75%]:
  _37 = NextBlockToWrite;
  if (_2 <= _37)
    goto <bb 39>; [5.00%]
  else
    goto <bb 41>; [95.00%]

  <bb 39> [4.59%]:
  _38 = OutMutex;
  ret_79 = pthread_mutex_unlock (_38);
  if (ret_79 != 0)
    goto <bb 40>; [0.04%]
  else
    goto <bb 43>; [99.96%]

  <bb 40> [0.00%]:
  _80 = stderr;
  __fprintf_chk (_80, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_79);
  cleanupAndQuit (-6);

  <bb 41> [87.17%]:
  _39 = OutMutex;
  safe_cond_timed_wait (&ErrStateChangeCond, _39, 1, "waitForPreviousBlock");
  _40 = OutMutex;
  ret_81 = pthread_mutex_unlock (_40);
  if (ret_81 != 0)
    goto <bb 42>; [0.04%]
  else
    goto <bb 50>; [99.96%]

  <bb 50> [87.14%]:
  goto <bb 28>; [100.00%]

  <bb 42> [0.03%]:
  _82 = stderr;
  __fprintf_chk (_82, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_81);
  cleanupAndQuit (-6);

  <bb 43> [12.71%]:
  # _41 = PHI <-1(32), 2(36), 0(39)>
  if (_41 == 0)
    goto <bb 44>; [50.00%]
  else
    goto <bb 47>; [50.00%]

  <bb 44> [6.36%]:
  if (isTrailingGarbageErr_13(D) != 0)
    goto <bb 45>; [98.00%]
  else
    goto <bb 46>; [2.00%]

  <bb 45> [6.23%]:
  _3 = stderr;
  __builtin_fwrite ("pbzip2: *WARNING: Trailing garbage after EOF ignored!\n", 1, 54, _3);
  goto <bb 49>; [100.00%]

  <bb 46> [0.13%]:
  _22 = strm_15(D)->avail_in;
  _23 = fileData_7(D)->isLastInSequence;
  _24 = (int) _23;
  _25 = fileData_7(D)->blockNumber;
  handle_error (1, -1, "pbzip2: %s: ret=%d; block=%d; seq=%d; isLastInSeq=%d; avail_in=%d\n", errmsg_16(D), bzret_14(D), _25, outSequenceNumber_8(D), _24, _22);
  goto <bb 49>; [100.00%]

  <bb 47> [6.36%]:
  if (_41 == 2)
    goto <bb 49>; [91.90%]
  else
    goto <bb 48>; [8.10%]

  <bb 48> [0.51%]:

  <bb 49> [13.32%]:
  # _4 = PHI <-1(6), 1(45), -1(46), 2(47), -1(48)>
  return _4;

}



;; Function appendOutBuffData (_ZN6pbzip216BZ2StreamScanner17appendOutBuffDataEPh, funcdef_no=89, decl_uid=4119, cgraph_uid=28, symbol_order=3092) (unlikely executed)

appendOutBuffData (struct BZ2StreamScanner * const this, CharType * end)
{
  char * newBuff;
  int D.5911;
  size_t newSize;
  int additionSize;
  long int _1;
  long int _2;
  long int _3;
  unsigned int _4;
  unsigned int _5;
  unsigned int _6;
  long unsigned int _7;
  unsigned int _8;
  unsigned int _9;
  int _10;
  bool _16;
  CharType * _18;
  bool _22;
  char * _25;
  unsigned int _26;
  sizetype _27;
  char * _28;
  CharType * _29;
  long unsigned int _37;
  long unsigned int _38;
  long unsigned int _39;
  long unsigned int _40;
  long unsigned int _41;
  long unsigned int _42;
  long unsigned int _44;
  unsigned int _45;
  unsigned int _46;
  char * _47;
  unsigned int _48;
  long unsigned int _49;
  char * _51;
  long unsigned int _52;
  bool _53;
  bool _54;
  int _55;

  <bb 2> [100.00%]:
  _1 = (long int) end_13(D);
  _18 = this_15(D)->_inBuffCurrent;
  _2 = (long int) _18;
  _3 = _1 - _2;
  additionSize_17 = (int) _3;
  if (additionSize_17 <= 0)
    goto <bb 17>; [32.39%]
  else
    goto <bb 3>; [67.61%]

  <bb 3> [67.61%]:
  _16 = MEM[(const struct BZ2StreamScanner *)this_15(D)]._bz2HeaderFound;
  if (_16 != 0)
    goto <bb 4>; [33.00%]
  else
    goto <bb 16>; [67.00%]

  <bb 4> [22.31%]:
  _4 = this_15(D)->_outBuff.bufSize;
  _5 = (unsigned int) _3;
  _6 = _4 + _5;
  newSize_19 = (size_t) _6;
  _37 = this_15(D)->_outBuffCapacity;
  if (newSize_19 <= _37)
    goto <bb 14>; [46.00%]
  else
    goto <bb 5>; [54.00%]

  <bb 5> [12.05%]:
  _38 = this_15(D)->_outBuffCapacityHint;
  if (newSize_19 > _38)
    goto <bb 6>; [50.00%]
  else
    goto <bb 8>; [50.00%]

  <bb 6> [6.02%]:
  _39 = newSize_19 * 11;
  _40 = _39 / 10;
  this_15(D)->_outBuffCapacityHint = _40;
  _41 = MEM[(const struct BZ2StreamScanner *)this_15(D)]._outBuffCapacityLimit;
  _22 = _40 > _41;
  _53 = newSize_19 <= _41;
  _54 = _22 & _53;
  if (_54 != 0)
    goto <bb 7>; [25.00%]
  else
    goto <bb 8>; [75.00%]

  <bb 7> [1.51%]:
  this_15(D)->_outBuffCapacityHint = _41;

  <bb 8> [12.05%]:
  _42 = this_15(D)->_outBuffCapacityHint;
  newBuff_43 = operator new [] (_42, &nothrow);
  if (newBuff_43 == 0B)
    goto <bb 9>; [0.43%]
  else
    goto <bb 10>; [99.57%]

  <bb 9> [0.05%]:
  _44 = this_15(D)->_outBuffCapacityHint;
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (ensureOutBuffCapacity/%u)!Aborting...\n", _44);
  _45 = this_15(D)->_errState;
  _46 = _45 | 2;
  this_15(D)->_errState = _46;
  goto <bb 14>; [100.00%]

  <bb 10> [12.00%]:
  _47 = this_15(D)->_outBuff.buf;
  if (_47 != 0B)
    goto <bb 11>; [53.47%]
  else
    goto <bb 13>; [46.53%]

  <bb 11> [6.41%]:
  _48 = this_15(D)->_outBuff.bufSize;
  _49 = (long unsigned int) _48;
  __builtin_memcpy (newBuff_43, _47, _49);
  _51 = this_15(D)->_outBuff.buf;
  if (_51 != 0B)
    goto <bb 12>; [53.47%]
  else
    goto <bb 13>; [46.53%]

  <bb 12> [3.43%]:
  operator delete [] (_51);

  <bb 13> [12.00%]:
  _52 = this_15(D)->_outBuffCapacityHint;
  this_15(D)->_outBuff.buf = newBuff_43;
  this_15(D)->_outBuffCapacity = _52;
  this_15(D)->_outBuff.inSize = 0;

  <bb 14> [22.31%]:
  # _55 = PHI <0(4), -1(9), 0(13)>
  if (_55 != 0)
    goto <bb 17>; [3.98%]
  else
    goto <bb 15>; [96.02%]

  <bb 15> [21.42%]:
  _7 = (long unsigned int) additionSize_17;
  _29 = this_15(D)->_inBuffCurrent;
  _25 = this_15(D)->_outBuff.buf;
  _26 = this_15(D)->_outBuff.bufSize;
  _27 = (sizetype) _26;
  _28 = _25 + _27;
  __builtin_memcpy (_28, _29, _7);
  _8 = this_15(D)->_outBuff.bufSize;
  _9 = _5 + _8;
  this_15(D)->_outBuff.bufSize = _9;

  <bb 16> [66.72%]:
  this_15(D)->_inBuffCurrent = end_13(D);

  <bb 17> [100.00%]:
  # _10 = PHI <0(2), -1(14), additionSize_17(16)>
  return _10;

}



;; Function searchNextHeader (_ZN6pbzip216BZ2StreamScanner16searchNextHeaderEv, funcdef_no=95, decl_uid=4112, cgraph_uid=22, symbol_order=3099) (unlikely executed)

searchNextHeader (struct BZ2StreamScanner * const this)
{
  int tailSize;
  int D.5980;
  int bytesRead;
  int D.5971;
  CharType * _1;
  unsigned int _8;
  bool _9;
  bool _10;
  CharType * _11;
  long int _13;
  CharType * _14;
  CharType * _15;
  long int _19;
  unsigned int _21;
  long unsigned int _22;
  long unsigned int _23;
  unsigned int _24;
  CharType * _25;
  unsigned int _26;
  CharType * _27;
  unsigned int _38;
  CharType * _39;
  long int _40;
  CharType * _41;
  long int _42;
  long int _43;
  long unsigned int _44;
  long unsigned int _45;
  unsigned int _46;
  unsigned int _47;
  long unsigned int _48;
  int _49;
  long int _50;
  CharType * _52;
  sizetype _53;
  CharType * _54;
  unsigned int _55;
  unsigned int _56;
  bool _57;
  CharType * _58;
  long unsigned int _59;
  sizetype _60;
  CharType * _61;
  int _62;
  unsigned int _63;
  CharType * _64;
  CharType * _65;
  long int _66;
  CharType * _67;
  long int _68;
  long int _69;
  CharType * _71;
  long unsigned int _72;
  CharType * _73;
  sizetype _74;
  CharType * _75;
  bool _76;
  bool _88;

  <bb 2> [14.13%]:
  _15 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._inBuffEnd;
  _13 = (long int) _15;
  _11 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._inBuffSearchPtr;
  _19 = (long int) _11;
  if (_13 != _19)
    goto <bb 3>; [48.88%]
  else
    goto <bb 5>; [51.12%]

  <bb 3> [6.91%]:
  searchNextHeaderInBuff (this_6(D));

  <bb 5> [14.13%]:

  <bb 32> [100.00%]:
  _10 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._searchStatus;
  if (_10 != 0)
    goto <bb 24>; [3.00%]
  else
    goto <bb 6>; [97.00%]

  <bb 6> [97.00%]:
  _9 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._eof;
  if (_9 != 0)
    goto <bb 24>; [3.00%]
  else
    goto <bb 7>; [97.00%]

  <bb 7> [94.09%]:
  _8 = this_6(D)->_errState;
  if (_8 == 0)
    goto <bb 8>; [97.00%]
  else
    goto <bb 24>; [3.00%]

  <bb 8> [91.27%]:
  _21 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._outBuff.bufSize;
  _22 = (long unsigned int) _21;
  _23 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._outBuffCapacityLimit;
  if (_22 < _23)
    goto <bb 9>; [97.00%]
  else
    goto <bb 24>; [3.00%]

  <bb 9> [88.53%]:
  _58 = this_6(D)->_inBuffEnd;
  _59 = MEM[(const struct basic_string *)this_6(D) + 16B]._M_string_length;
  _60 = 1 - _59;
  _61 = _58 + _60;
  this_6(D)->_inBuffSearchPtr = _61;
  _62 = appendOutBuffData (this_6(D), _61);
  _63 = this_6(D)->_errState;
  _57 = _62 < 0;
  _76 = _63 != 0;
  _88 = _57 | _76;
  if (_88 != 0)
    goto <bb 15>; [3.11%]
  else
    goto <bb 10>; [96.89%]

  <bb 10> [85.78%]:
  if (_62 == 0)
    goto <bb 11>; [50.00%]
  else
    goto <bb 12>; [50.00%]

  <bb 11> [42.89%]:
  _64 = this_6(D)->_inBuffCurrent;
  this_6(D)->_inBuffSearchPtr = _64;

  <bb 12> [85.78%]:
  _65 = this_6(D)->_inBuffEnd;
  _66 = (long int) _65;
  _67 = this_6(D)->_inBuffSearchPtr;
  _68 = (long int) _67;
  _69 = _66 - _68;
  tailSize_70 = (int) _69;
  _71 = this_6(D)->_inBuff;
  if (_69 != 0)
    goto <bb 13>; [33.00%]
  else
    goto <bb 14>; [67.00%]

  <bb 13> [28.31%]:
  _72 = (long unsigned int) _69;
  __builtin_memmove (_71, _67, _72);

  <bb 14> [85.78%]:
  _73 = this_6(D)->_inBuff;
  _74 = (sizetype) tailSize_70;
  _75 = _73 + _74;
  this_6(D)->_inBuffEnd = _75;
  this_6(D)->_inBuffCurrent = _73;
  this_6(D)->_inBuffSearchPtr = _73;

  <bb 15> [88.53%]:
  _38 = this_6(D)->_errState;
  if (_38 != 0)
    goto <bb 23>; [2.00%]
  else
    goto <bb 16>; [98.00%]

  <bb 16> [86.76%]:
  _39 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._inBuffEnd;
  _40 = (long int) _39;
  _41 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._inBuff;
  _42 = (long int) _41;
  _43 = _40 - _42;
  _44 = (long unsigned int) _43;
  _45 = MEM[(const struct BZ2StreamScanner *)this_6(D)]._inBuffCapacity;
  if (_44 >= _45)
    goto <bb 17>; [2.00%]
  else
    goto <bb 18>; [98.00%]

  <bb 17> [1.74%]:
  handle_error (1, -1, "pbzip2: *ERROR: BZ2StreamScanner::readData not enough buffer free space! inBuffSize=%u, _inBuffCapacity=%u! Aborting...\n", _44, _45);
  _46 = this_6(D)->_errState;
  _47 = _46 | 8;
  this_6(D)->_errState = _47;
  goto <bb 23>; [100.00%]

  <bb 18> [85.02%]:
  _48 = _45 - _44;
  _49 = this_6(D)->_hInFile;
  _50 = do_read (_49, _39, _48);
  bytesRead_51 = (int) _50;
  if (bytesRead_51 > 0)
    goto <bb 19>; [60.23%]
  else
    goto <bb 20>; [39.77%]

  <bb 19> [51.21%]:
  _52 = this_6(D)->_inBuffEnd;
  _53 = (sizetype) bytesRead_51;
  _54 = _52 + _53;
  this_6(D)->_inBuffEnd = _54;
  goto <bb 23>; [100.00%]

  <bb 20> [33.81%]:
  if (bytesRead_51 < 0)
    goto <bb 21>; [19.09%]
  else
    goto <bb 22>; [80.91%]

  <bb 21> [6.46%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not read from input file [err=%d]! Aborting...\n", bytesRead_51);
  _55 = this_6(D)->_errState;
  _56 = _55 | 4;
  this_6(D)->_errState = _56;
  goto <bb 23>; [100.00%]

  <bb 22> [27.36%]:
  this_6(D)->_eof = 1;

  <bb 23> [88.53%]:
  _24 = this_6(D)->_errState;
  if (_24 != 0)
    goto <bb 25>; [3.00%]
  else
    goto <bb 26>; [97.00%]

  <bb 24> [11.47%]:
  if (_10 != 0)
    goto <bb 27>; [50.00%]
  else
    goto <bb 28>; [50.00%]

  <bb 25> [2.66%]:
  _25 = this_6(D)->_inBuffEnd;
  goto <bb 31>; [100.00%]

  <bb 26> [85.87%]:
  searchNextHeaderInBuff (this_6(D));
  goto <bb 32>; [100.00%]

  <bb 27> [5.74%]:
  this_6(D)->_bz2HeaderFound = 1;

  <bb 28> [11.47%]:
  _26 = this_6(D)->_errState;
  if (_26 != 0)
    goto <bb 29>; [46.00%]
  else
    goto <bb 30>; [54.00%]

  <bb 29> [5.28%]:
  _27 = this_6(D)->_inBuffEnd;
  this_6(D)->_inBuffSearchPtr = _27;
  goto <bb 31>; [100.00%]

  <bb 30> [6.19%]:
  _14 = this_6(D)->_inBuffSearchPtr;

  <bb 31> [14.13%]:
  # _1 = PHI <_25(25), _27(29), _14(30)>
  return _1;

}



;; Function getNextStream (_ZN6pbzip216BZ2StreamScanner13getNextStreamEv, funcdef_no=98, decl_uid=4109, cgraph_uid=17, symbol_order=3100) (unlikely executed)

No longer having address taken: D.5983
No longer having address taken: D.5984
No longer having address taken: end2
No longer having address taken: end1
No longer having address taken: D.6008
No longer having address taken: D.6009
No longer having address taken: D.6010

Symbols to be put in SSA form
{ D.5983 D.5984 D.6004 D.6005 D.6008 D.6009 D.6010 }
Incremental SSA update started at block: 0
Number of blocks in CFG: 33
Number of blocks to update: 32 ( 97%)


getNextStream (struct BZ2StreamScanner * const this)
{
  const long unsigned int D.6010;
  unsigned char * const D.6009;
  unsigned char * const D.6008;
  size_t maxCapacity;
  int maxAddition;
  CharType * end1;
  CharType * end2;
  CharType * end;
  int D.6007;
  unsigned char * const D.5984;
  unsigned char * const D.5983;
  unsigned int _1;
  unsigned int _2;
  CharType * _3;
  int _5;
  int _6;
  int _7;
  unsigned int _8;
  int _9;
  int _10;
  void * _21;
  unsigned int _24;
  long unsigned int _27;
  bool _29;
  bool _30;
  CharType * _32;
  bool _33;
  CharType * _35;
  CharType * _41;
  unsigned int _43;
  bool _44;
  unsigned int _50;
  unsigned int _60;
  CharType * _61;
  CharType * _62;
  CharType * _63;
  CharType * _64;
  long unsigned int _110;
  long unsigned int _111;
  unsigned char * _112;
  unsigned int _114;
  unsigned int _115;
  unsigned int _116;
  bool _118;
  CharType * _119;
  CharType * _120;
  long unsigned int _121;
  sizetype _122;
  CharType * _123;
  CharType * _124;
  CharType * _127;
  sizetype _128;
  CharType * _129;
  unsigned char * _130;
  unsigned char * _131;

  <bb 2> [100.00%]:
  this_18(D)->_outBuff.buf = 0B;
  this_18(D)->_outBuff.bufSize = 0;
  this_18(D)->_outBuffCapacity = 0;
  this_18(D)->_outBuff.inSize = 0;
  _21 = operator new (40, &nothrow);
  if (_21 != 0B)
    goto <bb 3>; [70.00%]
  else
    goto <bb 4>; [30.00%]

  <bb 3> [70.00%]:
  MEM[(struct  &)_21] ={v} {CLOBBER};
  MEM[(struct outBuff *)_21].buf = 0B;
  MEM[(struct outBuff *)_21].bufSize = 0;
  MEM[(struct outBuff *)_21].blockNumber = 0;
  MEM[(struct outBuff *)_21].sequenceNumber = 0;
  MEM[(struct outBuff *)_21].inSize = 0;
  MEM[(struct outBuff *)_21].isLastInSequence = 1;
  MEM[(struct outBuff *)_21].next = 0B;

  <bb 4> [100.00%]:
  if (_21 == 0B)
    goto <bb 5>; [15.24%]
  else
    goto <bb 6>; [84.76%]

  <bb 5> [15.24%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (getNextStream/%u)!Aborting...\n", 40);
  _1 = this_18(D)->_errState;
  _2 = _1 | 2;
  this_18(D)->_errState = _2;
  goto <bb 32>; [100.00%]

  <bb 6> [84.76%]:
  MEM[(struct outBuff *)_21].buf = 0B;
  MEM[(struct outBuff *)_21].bufSize = 4294967295;
  _50 = this_18(D)->_errState;
  if (_50 == 0)
    goto <bb 7>; [50.00%]
  else
    goto <bb 9>; [50.00%]

  <bb 7> [42.38%]:
  _44 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._bz2HeaderFound;
  if (_44 != 0)
    goto <bb 9>; [67.00%]
  else
    goto <bb 8>; [33.00%]

  <bb 8> [13.99%]:
  this_18(D)->_searchStatus = 0;
  searchNextHeader (this_18(D));

  <bb 9> [84.76%]:
  _43 = this_18(D)->_errState;
  if (_43 != 0)
    goto <bb 32>; [46.00%]
  else
    goto <bb 10>; [54.00%]

  <bb 10> [45.77%]:
  _35 = this_18(D)->_inBuffCurrent;
  _41 = this_18(D)->_inBuffEnd;
  if (_35 == _41)
    goto <bb 11>; [30.00%]
  else
    goto <bb 12>; [70.00%]

  <bb 11> [13.73%]:
  _33 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._eof;
  if (_33 != 0)
    goto <bb 13>; [46.00%]
  else
    goto <bb 12>; [54.00%]

  <bb 12> [39.45%]:
  _32 = this_18(D)->_inBuffSearchPtr;
  if (_32 == _35)
    goto <bb 16>; [30.00%]
  else
    goto <bb 14>; [70.00%]

  <bb 13> [6.32%]:
  MEM[(struct outBuff *)_21].bufSize = 0;
  goto <bb 32>; [100.00%]

  <bb 14> [27.62%]:
  _30 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._searchStatus;
  if (_30 != 0)
    goto <bb 17>; [50.00%]
  else
    goto <bb 15>; [50.00%]

  <bb 15> [13.81%]:
  _29 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._eof;
  if (_29 != 0)
    goto <bb 17>; [50.00%]
  else
    goto <bb 16>; [50.00%]

  <bb 16> [18.74%]:
  _126 = _41;
  _27 = MEM[(const struct basic_string *)this_18(D) + 16B]._M_string_length;
  _3 = _32 + _27;
  _113 = _3;
  _112 = MIN_EXPR <_41, _3>;
  this_18(D)->_inBuffSearchPtr = _112;
  this_18(D)->_searchStatus = 0;
  searchNextHeader (this_18(D));

  <bb 17> [39.45%]:
  _24 = this_18(D)->_errState;
  if (_24 != 0)
    goto <bb 32>; [63.36%]
  else
    goto <bb 18>; [36.64%]

  <bb 18> [14.46%]:
  _110 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._outBuffCapacityLimit;
  _109 = _110;
  _111 = MEM[(const long unsigned int &)this_18(D) + 168];
  maxCapacity_125 = MAX_EXPR <_111, _110>;
  _114 = (unsigned int) maxCapacity_125;
  _115 = this_18(D)->_outBuff.bufSize;
  _116 = _114 - _115;
  maxAddition_117 = (int) _116;
  if (maxAddition_117 <= 0)
    goto <bb 23>; [32.39%]
  else
    goto <bb 19>; [67.61%]

  <bb 19> [9.77%]:
  _118 = MEM[(const struct BZ2StreamScanner *)this_18(D)]._eof;
  if (_118 != 0)
    goto <bb 20>; [50.00%]
  else
    goto <bb 21>; [50.00%]

  <bb 20> [4.89%]:
  _119 = this_18(D)->_inBuffEnd;
  end1_159 = _119;
  goto <bb 22>; [100.00%]

  <bb 21> [4.89%]:
  _120 = this_18(D)->_inBuffEnd;
  _121 = MEM[(const struct basic_string *)this_18(D) + 16B]._M_string_length;
  _122 = 1 - _121;
  _123 = _120 + _122;
  _154 = _123;
  _124 = this_18(D)->_inBuffSearchPtr;
  _108 = _124;
  _131 = MIN_EXPR <_123, _124>;
  end1_105 = _131;

  <bb 22> [9.77%]:
  # end1_132 = PHI <end1_159(20), end1_105(21)>
  _127 = this_18(D)->_inBuffCurrent;
  _128 = (sizetype) maxAddition_117;
  _129 = _127 + _128;
  end2_72 = _129;
  _130 = end1_132;
  end_134 = MIN_EXPR <_129, _130>;
  appendOutBuffData (this_18(D), end_134);

  <bb 23> [14.46%]:
  _60 = this_18(D)->_errState;
  if (_60 != 0)
    goto <bb 32>; [46.00%]
  else
    goto <bb 24>; [54.00%]

  <bb 24> [7.81%]:
  _5 = this_18(D)->_outSequenceNumber;
  if (_5 > 0)
    goto <bb 25>; [64.00%]
  else
    goto <bb 26>; [36.00%]

  <bb 25> [5.00%]:
  _6 = _5 + 1;
  this_18(D)->_outSequenceNumber = _6;
  goto <bb 28>; [100.00%]

  <bb 26> [2.81%]:
  _62 = this_18(D)->_inBuffCurrent;
  _61 = this_18(D)->_inBuffSearchPtr;
  if (_61 != _62)
    goto <bb 27>; [70.00%]
  else
    goto <bb 28>; [30.00%]

  <bb 27> [1.97%]:
  this_18(D)->_outSequenceNumber = 1;

  <bb 28> [7.81%]:
  _7 = this_18(D)->_outSequenceNumber;
  this_18(D)->_outBuff.sequenceNumber = _7;
  _8 = this_18(D)->_outBuff.bufSize;
  this_18(D)->_outBuff.inSize = _8;
  _9 = this_18(D)->_streamNumber;
  this_18(D)->_outBuff.blockNumber = _9;
  _64 = this_18(D)->_inBuffCurrent;
  _63 = this_18(D)->_inBuffSearchPtr;
  if (_63 == _64)
    goto <bb 29>; [30.00%]
  else
    goto <bb 30>; [70.00%]

  <bb 29> [2.34%]:
  this_18(D)->_outBuff.isLastInSequence = 1;
  this_18(D)->_outSequenceNumber = 0;
  _10 = _9 + 1;
  this_18(D)->_streamNumber = _10;
  goto <bb 31>; [100.00%]

  <bb 30> [5.46%]:
  this_18(D)->_outBuff.isLastInSequence = 0;

  <bb 31> [7.81%]:
  MEM[(struct outBuff *)_21] = this_18(D)->_outBuff;
  this_18(D)->_outBuff.buf = 0B;
  this_18(D)->_outBuff.bufSize = 0;
  this_18(D)->_outBuffCapacity = 0;
  this_18(D)->_outBuff.inSize = 0;

  <bb 32> [100.00%]:
  return _21;

}



;; Function outputBufferAdd (_Z15outputBufferAddRK7outBuffPKc, funcdef_no=20, decl_uid=4294, cgraph_uid=128, symbol_order=785)

outputBufferAdd (const struct outBuff & element, const char * caller)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  size_t outBuffPos;
  int ret;
  int dist;
  union pthread_mutex_t * _1;
  int _2;
  unsigned int _3;
  long unsigned int _4;
  unsigned int _5;
  unsigned int _6;
  int _7;
  union pthread_mutex_t * _8;
  int _9;
  int _10;
  union pthread_mutex_t * _11;
  union pthread_mutex_t * _12;
  union pthread_cond_t * _13;
  int _14;
  int _15;
  int _16;
  int _17;
  union pthread_mutex_t * _18;
  struct outBuff * _19;
  long unsigned int _30;
  struct outBuff * _31;
  struct value_type & _37;
  long unsigned int _38;
  long unsigned int _39;
  long unsigned int _40;
  long unsigned int _41;
  long unsigned int _43;
  struct outBuff * _46;
  struct value_type & _47;
  struct FILE * _63;
  struct FILE * _65;
  struct FILE * _67;
  int _68;
  struct FILE * _69;
  struct FILE * _71;
  struct FILE * _73;

  <bb 2> [13.46%]:
  _1 = OutMutex;
  ret_62 = pthread_mutex_lock (_1);
  if (ret_62 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.01%]:
  _63 = stderr;
  __fprintf_chk (_63, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_62);
  cleanupAndQuit (-5);

  <bb 4> [13.45%]:
  _2 = element_25(D)->blockNumber;
  _3 = (unsigned int) _2;
  _4 = NumBufferedBlocksMax;
  _5 = (unsigned int) _4;
  _6 = _3 - _5;
  dist_26 = (int) _6;

  <bb 5> [100.00%]:
  _7 = NextBlockToWrite;
  if (_7 > dist_26)
    goto <bb 17>; [5.00%]
  else
    goto <bb 6>; [95.00%]

  <bb 6> [95.00%]:
  ret_66 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_66 != 0)
    goto <bb 7>; [0.04%]
  else
    goto <bb 8>; [99.96%]

  <bb 7> [0.04%]:
  _67 = stderr;
  __fprintf_chk (_67, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_66);
  cleanupAndQuit (-5);

  <bb 8> [94.96%]:
  ret_36 = terminateFlag;
  ret_64 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_64 != 0)
    goto <bb 9>; [0.04%]
  else
    goto <bb 10>; [99.96%]

  <bb 9> [0.04%]:
  _65 = stderr;
  __fprintf_chk (_65, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_64);
  cleanupAndQuit (-6);

  <bb 10> [94.92%]:
  if (ret_36 != 0)
    goto <bb 11>; [5.00%]
  else
    goto <bb 12>; [95.00%]

  <bb 11> [4.75%]:
  _8 = OutMutex;
  pthread_mutex_unlock (_8);
  goto <bb 25>; [100.00%]

  <bb 12> [90.18%]:
  _9 = LastGoodBlock;
  if (_9 != -1)
    goto <bb 13>; [79.76%]
  else
    goto <bb 15>; [20.24%]

  <bb 13> [71.93%]:
  _10 = element_25(D)->blockNumber;
  if (_9 < _10)
    goto <bb 14>; [5.00%]
  else
    goto <bb 15>; [95.00%]

  <bb 14> [3.60%]:
  _11 = OutMutex;
  pthread_mutex_unlock (_11);
  goto <bb 25>; [100.00%]

  <bb 15> [86.58%]:
  _12 = OutMutex;
  _13 = notTooMuchNumBuffered;
  _68 = pthread_cond_wait (_13, _12);
  if (_68 != 0)
    goto <bb 16>; [0.04%]
  else
    goto <bb 26>; [99.96%]

  <bb 26> [86.55%]:
  goto <bb 5>; [100.00%]

  <bb 16> [0.03%]:
  _69 = stderr;
  __fprintf_chk (_69, 1, "pthread_cond_wait error [%d]! Aborting immediately!\n", _68);
  pthread_mutex_unlock (_12);
  cleanupAndQuit (-8);

  <bb 17> [5.00%]:
  _14 = element_25(D)->blockNumber;
  _38 = (long unsigned int) _14;
  _39 = OutBufferPosToWrite;
  _40 = _38 + _39;
  _41 = (long unsigned int) _7;
  outBuffPos_42 = _40 - _41;
  _43 = NumBufferedBlocksMax;
  if (outBuffPos_42 >= _43)
    goto <bb 18>; [50.00%]
  else
    goto <bb 19>; [50.00%]

  <bb 18> [2.50%]:
  outBuffPos_44 = outBuffPos_42 - _43;

  <bb 19> [5.00%]:
  # outBuffPos_45 = PHI <outBuffPos_42(17), outBuffPos_44(18)>
  _31 = OutputBuffer.D.4140._M_impl._M_start;
  _30 = outBuffPos_45 * 40;
  _37 = _31 + _30;
  *_37 = MEM[(const struct outBuff &)element_25(D)];
  _15 = NumBufferedBlocks;
  _16 = _15 + 1;
  NumBufferedBlocks = _16;
  _17 = element_25(D)->blockNumber;
  if (_7 == _17)
    goto <bb 20>; [20.24%]
  else
    goto <bb 22>; [79.76%]

  <bb 20> [1.01%]:
  ret_70 = pthread_cond_signal (&OutBufferHeadNotEmpty);
  if (ret_70 != 0)
    goto <bb 21>; [0.04%]
  else
    goto <bb 22>; [99.96%]

  <bb 21> [0.00%]:
  _71 = stderr;
  __fprintf_chk (_71, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_70);
  cleanupAndQuit (-7);

  <bb 22> [5.00%]:
  _18 = OutMutex;
  ret_72 = pthread_mutex_unlock (_18);
  if (ret_72 != 0)
    goto <bb 23>; [0.04%]
  else
    goto <bb 24>; [99.96%]

  <bb 23> [0.00%]:
  _73 = stderr;
  __fprintf_chk (_73, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_72);
  cleanupAndQuit (-6);

  <bb 24> [5.00%]:
  _46 = OutputBuffer.D.4140._M_impl._M_start;
  _47 = _46 + _30;

  <bb 25> [13.34%]:
  # _19 = PHI <0B(11), 0B(14), _47(24)>
  return _19;

}



;; Function signalHandlerProc (signalHandlerProc, funcdef_no=27, decl_uid=4285, cgraph_uid=119, symbol_order=766)

signalHandlerProc (void * arg)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int signalCaught;
  struct FILE * _1;
  int _2;
  struct FILE * _4;
  int _8;
  union pthread_mutex_t * _16;
  union pthread_cond_t * _17;
  union pthread_mutex_t * _18;
  struct queue * _19;
  union pthread_mutex_t * _20;
  struct queue * _21;
  union pthread_cond_t * _22;
  struct queue * _23;
  union pthread_cond_t * _24;
  struct queue * _25;
  union pthread_mutex_t * _26;
  struct FILE * _28;
  struct FILE * _30;
  struct FILE * _32;
  struct FILE * _34;
  struct FILE * _36;
  struct FILE * _38;

  <bb 2> [100.00%]:
  _8 = sigwait (&SignalMask, &signalCaught);
  if (_8 != 0)
    goto <bb 3>; [29.56%]
  else
    goto <bb 4>; [70.44%]

  <bb 3> [29.56%]:
  _1 = stderr;
  __fprintf_chk (_1, 1, "\n *signalHandlerProc - sigwait error: %d\n", _8);
  goto <bb 18>; [100.00%]

  <bb 4> [70.44%]:
  _2 = signalCaught;
  if (_2 == 10)
    goto <bb 18>; [47.11%]
  else
    goto <bb 5>; [52.89%]

  <bb 5> [37.26%]:
  _4 = stderr;
  __fprintf_chk (_4, 1, "\n *Control-C or similar caught [sig=%d], quitting...\n", _2);
  ret_27 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_27 != 0)
    goto <bb 6>; [0.04%]
  else
    goto <bb 7>; [99.96%]

  <bb 6> [0.01%]:
  _28 = stderr;
  __fprintf_chk (_28, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_27);
  cleanupAndQuit (-5);

  <bb 7> [37.24%]:
  terminateFlag = 1;
  pthread_cond_signal (&TerminateCond);
  ret_33 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_33 != 0)
    goto <bb 8>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 8> [0.01%]:
  _34 = stderr;
  __fprintf_chk (_34, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_33);
  cleanupAndQuit (-6);

  <bb 9> [37.23%]:
  _16 = OutMutex;
  ret_31 = pthread_mutex_lock (_16);
  if (ret_31 != 0)
    goto <bb 10>; [0.04%]
  else
    goto <bb 11>; [99.96%]

  <bb 10> [0.01%]:
  _32 = stderr;
  __fprintf_chk (_32, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_31);
  cleanupAndQuit (-5);

  <bb 11> [37.21%]:
  _17 = notTooMuchNumBuffered;
  pthread_cond_broadcast (_17);
  _18 = OutMutex;
  ret_29 = pthread_mutex_unlock (_18);
  if (ret_29 != 0)
    goto <bb 12>; [0.04%]
  else
    goto <bb 13>; [99.96%]

  <bb 12> [0.01%]:
  _30 = stderr;
  __fprintf_chk (_30, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_29);
  cleanupAndQuit (-6);

  <bb 13> [37.20%]:
  _19 = FifoQueue;
  if (_19 != 0B)
    goto <bb 14>; [53.47%]
  else
    goto <bb 18>; [46.53%]

  <bb 14> [19.89%]:
  _20 = _19->mut;
  ret_37 = pthread_mutex_lock (_20);
  if (ret_37 != 0)
    goto <bb 15>; [0.04%]
  else
    goto <bb 16>; [99.96%]

  <bb 15> [0.01%]:
  _38 = stderr;
  __fprintf_chk (_38, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_37);
  cleanupAndQuit (-5);

  <bb 16> [19.88%]:
  _21 = FifoQueue;
  _22 = _21->notFull;
  pthread_cond_broadcast (_22);
  _23 = FifoQueue;
  _24 = _23->notEmpty;
  pthread_cond_broadcast (_24);
  _25 = FifoQueue;
  _26 = _25->mut;
  ret_35 = pthread_mutex_unlock (_26);
  if (ret_35 != 0)
    goto <bb 17>; [0.04%]
  else
    goto <bb 18>; [99.96%]

  <bb 17> [0.01%]:
  _36 = stderr;
  __fprintf_chk (_36, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_35);
  cleanupAndQuit (-6);

  <bb 18> [99.92%]:
  signalCaught ={v} {CLOBBER};
  return 0B;

}



;; Function terminatorThreadProc (terminatorThreadProc, funcdef_no=28, decl_uid=4283, cgraph_uid=117, symbol_order=765)

terminatorThreadProc (void * arg)
{
  int ret;
  int * _1;
  int _2;
  struct FILE * _3;
  struct FILE * _4;
  int _5;
  int _6;
  struct FILE * _7;
  int _25;
  int _26;
  int _27;

  <bb 2> [14.44%]:
  ret_11 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_11 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.01%]:
  _1 = __errno_location ();
  _2 = *_1;
  _3 = stderr;
  syncPrintErrnoMsg (_3, _2);
  _4 = stderr;
  __fprintf_chk (_4, 1, "Terminator thread: pthread_mutex_lock error [%d]! Aborting...\n", ret_11);
  syncSetTerminateFlag (1);
  cleanupAndQuit (1);

  <bb 4> [14.40%]:
  _25 = finishedFlag;
  if (_25 != 0)
    goto <bb 8>; [7.50%]
  else
    goto <bb 5>; [92.50%]

  <bb 5> [13.32%]:
  _26 = terminateFlag;
  if (_26 != 0)
    goto <bb 8>; [7.50%]
  else
    goto <bb 7>; [92.50%]

  <bb 6> [79.18%]:
  _6 = terminateFlag;
  if (_6 != 0)
    goto <bb 8>; [7.50%]
  else
    goto <bb 7>; [92.50%]

  <bb 7> [85.60%]:
  pthread_cond_wait (&TerminateCond, &TerminateFlagMutex);
  _5 = finishedFlag;
  if (_5 != 0)
    goto <bb 8>; [7.50%]
  else
    goto <bb 6>; [92.50%]

  <bb 8> [14.44%]:
  # _27 = PHI <_5(7), _5(6), _25(4), _25(5)>
  if (_27 != 0)
    goto <bb 9>; [99.96%]
  else
    goto <bb 10>; [0.04%]

  <bb 9> [14.43%]:
  pthread_mutex_unlock (&TerminateFlagMutex);
  return 0B;

  <bb 10> [0.01%]:
  pthread_mutex_unlock (&TerminateFlagMutex);
  _7 = stderr;
  __builtin_fwrite ("Terminator thread: premature exit requested - quitting...\n", 1, 58, _7);
  cleanupAndQuit (1);

}



;; Function safe_cond_broadcast (_Z19safe_cond_broadcastP14pthread_cond_t, funcdef_no=29, decl_uid=4250, cgraph_uid=73, symbol_order=731) (unlikely executed)

safe_cond_broadcast (union pthread_cond_t * cond)
{
  int ret;
  struct FILE * _1;

  <bb 2> [100.00%]:
  ret_5 = pthread_cond_broadcast (cond_3(D));
  if (ret_5 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.04%]:
  _1 = stderr;
  __fprintf_chk (_1, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_5);
  cleanupAndQuit (-7);

  <bb 4> [99.96%]:
  return;

}



;; Function safe_mutex_unlock (_Z17safe_mutex_unlockP15pthread_mutex_t, funcdef_no=10, decl_uid=4245, cgraph_uid=55, symbol_order=729)

safe_mutex_unlock (union pthread_mutex_t * mutex)
{
  int ret;
  struct FILE * _1;

  <bb 2> [100.00%]:
  ret_5 = pthread_mutex_unlock (mutex_3(D));
  if (ret_5 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.04%]:
  _1 = stderr;
  __fprintf_chk (_1, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_5);
  cleanupAndQuit (-6);

  <bb 4> [99.96%]:
  return;

}



;; Function safe_mutex_lock (_Z15safe_mutex_lockP15pthread_mutex_t, funcdef_no=9, decl_uid=4244, cgraph_uid=53, symbol_order=728)

safe_mutex_lock (union pthread_mutex_t * mutex)
{
  int ret;
  struct FILE * _1;

  <bb 2> [100.00%]:
  ret_5 = pthread_mutex_lock (mutex_3(D));
  if (ret_5 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.04%]:
  _1 = stderr;
  __fprintf_chk (_1, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_5);
  cleanupAndQuit (-5);

  <bb 4> [99.96%]:
  return;

}



;; Function producerDecompressCheckInterrupt (_Z32producerDecompressCheckInterruptiRP7outBuffi, funcdef_no=18, decl_uid=4316, cgraph_uid=203, symbol_order=768) (unlikely executed)

producerDecompressCheckInterrupt (int hInfile, struct outBuff * & fileData, int lastBlock)
{
  int ret;
  int D.5021;
  int ret;
  bool isInterrupted;
  int _2;
  bool _6;
  struct outBuff * _11;
  bool _14;
  union pthread_mutex_t * _20;
  union pthread_mutex_t * _22;
  bool _25;

  <bb 2> [100.00%]:
  _20 = OutMutex;
  safe_mutex_lock (_20);
  ret_21 = LastGoodBlock;
  _22 = OutMutex;
  safe_mutex_unlock (_22);
  _14 = ret_21 != -1;
  safe_mutex_lock (&TerminateFlagMutex);
  ret_10 = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);
  _25 = ret_10 != 0;
  _6 = _14 | _25;
  if (_6 != 0)
    goto <bb 3>; [69.18%]
  else
    goto <bb 5>; [30.82%]

  <bb 3> [69.18%]:
  close (hInfile_7(D));
  _11 = MEM[(struct outBuff * &)fileData_9(D)];
  if (_11 != 0B)
    goto <bb 4>; [57.44%]
  else
    goto <bb 5>; [42.56%]

  <bb 4> [39.74%]:
  operator delete (_11, 40);
  MEM[(struct outBuff * &)fileData_9(D)] = 0B;

  <bb 5> [100.00%]:
  # _2 = PHI <0(2), 1(3), 1(4)>
  return _2;

}



;; Function syncSetProducerDone (_Z19syncSetProducerDonei, funcdef_no=35, decl_uid=4243, cgraph_uid=51, symbol_order=745) (unlikely executed)

syncSetProducerDone (int newValue)
{
  int ret;
  int ret;
  union pthread_mutex_t * _1;
  union pthread_mutex_t * _2;
  struct FILE * _11;
  struct FILE * _13;

  <bb 2> [100.00%]:
  _1 = ProducerDoneMutex;
  ret_12 = pthread_mutex_lock (_1);
  if (ret_12 != 0)
    goto <bb 3>; [0.04%]
  else
    goto <bb 4>; [99.96%]

  <bb 3> [0.04%]:
  _13 = stderr;
  __fprintf_chk (_13, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_12);
  cleanupAndQuit (-5);

  <bb 4> [99.96%]:
  producerDone = newValue_5(D);
  _2 = ProducerDoneMutex;
  ret_10 = pthread_mutex_unlock (_2);
  if (ret_10 != 0)
    goto <bb 5>; [0.04%]
  else
    goto <bb 6>; [99.96%]

  <bb 5> [0.04%]:
  _11 = stderr;
  __fprintf_chk (_11, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_10);
  cleanupAndQuit (-6);

  <bb 6> [99.92%]:
  return;

}



;; Function consumer (consumer, funcdef_no=37, decl_uid=4175, cgraph_uid=46, symbol_order=777)

consumer (void * q)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int D.5159;
  int ret;
  struct ElementType * & headElem;
  int D.5152;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  struct outBuff outBlock;
  unsigned int outSize;
  char * CompressedData;
  struct outBuff * fileData;
  union pthread_mutex_t * _1;
  int _2;
  int _4;
  union pthread_mutex_t * _5;
  union pthread_mutex_t * _6;
  union pthread_cond_t * _7;
  union pthread_cond_t * _8;
  union pthread_mutex_t * _9;
  unsigned int _11;
  double _12;
  double _13;
  double _14;
  unsigned int _15;
  int _17;
  unsigned int _19;
  char * _20;
  unsigned int _23;
  int _24;
  unsigned int _25;
  sizetype _47;
  int _51;
  struct outBuff * _53;
  char * _67;
  struct FILE * _116;
  struct FILE * _118;
  struct FILE * _120;
  struct ElementType * * _121;
  long int _122;
  long unsigned int _123;
  long unsigned int _124;
  bool _127;
  struct outBuff * _128;
  int _129;
  struct ElementType * _130;
  long int _132;
  long int _133;
  long int _134;
  long int _135;
  long int _136;
  long int _137;
  int _138;
  struct FILE * _140;
  union pthread_mutex_t * _141;
  union pthread_mutex_t * _143;
  struct FILE * _145;
  struct FILE * _147;
  struct FILE * _149;
  struct FILE * _151;
  struct FILE * _153;
  int _154;
  struct FILE * _155;
  struct FILE * _157;

  <bb 2> [5.20%]:
  outSize = 0;

  <bb 3> [6.73%]:
  ret_115 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_115 != 0)
    goto <bb 4>; [0.04%]
  else
    goto <bb 6>; [99.96%]

  <bb 4> [0.00%]:
  _116 = stderr;
  __fprintf_chk (_116, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_115);

  <bb 5> [0.00%]:
  cleanupAndQuit (-5);

  <bb 6> [6.73%]:
  ret_65 = terminateFlag;
  ret_117 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_117 != 0)
    goto <bb 7>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 7> [0.00%]:
  _118 = stderr;
  __fprintf_chk (_118, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_117);

  <bb 8> [0.00%]:
  cleanupAndQuit (-6);

  <bb 9> [6.72%]:
  if (ret_65 != 0)
    goto <bb 65>; [2.50%]
  else
    goto <bb 10>; [97.50%]

  <bb 10> [6.56%]:
  _1 = MEM[(struct queue *)q_37(D)].mut;
  ret_119 = pthread_mutex_lock (_1);
  if (ret_119 != 0)
    goto <bb 11>; [0.04%]
  else
    goto <bb 67>; [99.96%]

  <bb 11> [0.00%]:
  _120 = stderr;
  __fprintf_chk (_120, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_119);

  <bb 12> [0.00%]:
  cleanupAndQuit (-5);

  <bb 67> [6.56%]:

  <bb 13> [100.00%]:
  # fileData_209 = PHI <0B(67), fileData_212(68)>
  _2 = MEM[(struct queue *)q_37(D)].empty;
  if (_2 == 0)
    goto <bb 14>; [33.00%]
  else
    goto <bb 30>; [67.00%]

  <bb 14> [33.00%]:
  _121 = MEM[(struct queue *)q_37(D)].qData;
  _122 = MEM[(struct queue *)q_37(D)].head;
  _123 = (long unsigned int) _122;
  _124 = _123 * 8;
  headElem_125 = _121 + _124;
  if (fileData_209 != 0B)
    goto <bb 15>; [70.00%]
  else
    goto <bb 17>; [30.00%]

  <bb 15> [23.10%]:
  _127 = MEM[(struct ElementType *)fileData_209].isLastInSequence;
  if (_127 != 0)
    goto <bb 17>; [50.00%]
  else
    goto <bb 16>; [50.00%]

  <bb 16> [11.55%]:
  _128 = MEM[(struct ElementType *)fileData_209].next;
  if (_128 != 0B)
    goto <bb 23>; [73.26%]
  else
    goto <bb 26>; [26.74%]

  <bb 17> [21.45%]:
  _129 = MEM[(struct queue *)q_37(D)].topLevelEmpty;
  if (_129 != 0)
    goto <bb 26>; [46.00%]
  else
    goto <bb 18>; [54.00%]

  <bb 18> [11.58%]:
  _130 = *headElem_125;
  _132 = _122 + 1;
  MEM[(struct queue *)q_37(D)].head = _132;
  _133 = MEM[(struct queue *)q_37(D)].size;
  if (_132 == _133)
    goto <bb 19>; [34.00%]
  else
    goto <bb 20>; [66.00%]

  <bb 19> [3.94%]:
  MEM[(struct queue *)q_37(D)].head = 0;

  <bb 20> [11.58%]:
  _134 = MEM[(struct queue *)q_37(D)].head;
  _135 = MEM[(struct queue *)q_37(D)].tail;
  if (_134 == _135)
    goto <bb 21>; [34.00%]
  else
    goto <bb 22>; [66.00%]

  <bb 21> [3.94%]:
  MEM[(struct queue *)q_37(D)].topLevelEmpty = 1;

  <bb 22> [11.58%]:
  MEM[(struct queue *)q_37(D)].topLevelFull = 0;

  <bb 23> [20.04%]:
  # fileData_210 = PHI <_128(16), _130(22)>
  _136 = MEM[(struct queue *)q_37(D)].count;
  _137 = _136 + -1;
  MEM[(struct queue *)q_37(D)].count = _137;
  if (_137 == 0)
    goto <bb 24>; [50.00%]
  else
    goto <bb 25>; [50.00%]

  <bb 24> [10.02%]:
  MEM[(struct queue *)q_37(D)].empty = 1;

  <bb 25> [20.04%]:
  MEM[(struct queue *)q_37(D)].full = 0;

  <bb 26> [33.00%]:
  # _138 = PHI <0(16), 0(17), 1(25)>
  # fileData_211 = PHI <fileData_209(16), fileData_209(17), fileData_210(25)>
  if (_138 == 1)
    goto <bb 27>; [5.00%]
  else
    goto <bb 30>; [95.00%]

  <bb 27> [1.65%]:
  _8 = MEM[(struct queue *)q_37(D)].notFull;
  ret_139 = pthread_cond_signal (_8);
  if (ret_139 != 0)
    goto <bb 28>; [0.04%]
  else
    goto <bb 52>; [99.96%]

  <bb 28> [0.00%]:
  _140 = stderr;
  __fprintf_chk (_140, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_139);

  <bb 29> [0.00%]:
  cleanupAndQuit (-7);

  <bb 30> [98.35%]:
  # fileData_212 = PHI <fileData_209(13), fileData_211(26)>
  _4 = MEM[(struct queue *)q_37(D)].empty;
  if (_4 != 0)
    goto <bb 31>; [50.00%]
  else
    goto <bb 48>; [50.00%]

  <bb 31> [49.17%]:
  _141 = ProducerDoneMutex;
  ret_144 = pthread_mutex_lock (_141);
  if (ret_144 != 0)
    goto <bb 32>; [0.04%]
  else
    goto <bb 34>; [99.96%]

  <bb 32> [0.02%]:
  _145 = stderr;
  __fprintf_chk (_145, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_144);

  <bb 33> [0.02%]:
  cleanupAndQuit (-5);

  <bb 34> [49.16%]:
  ret_142 = producerDone;
  _143 = ProducerDoneMutex;
  ret_146 = pthread_mutex_unlock (_143);
  if (ret_146 != 0)
    goto <bb 35>; [0.04%]
  else
    goto <bb 37>; [99.96%]

  <bb 35> [0.02%]:
  _147 = stderr;
  __fprintf_chk (_147, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_146);

  <bb 36> [0.02%]:
  cleanupAndQuit (-6);

  <bb 37> [49.14%]:
  if (ret_142 == 1)
    goto <bb 45>; [5.00%]
  else
    goto <bb 38>; [95.00%]

  <bb 38> [46.68%]:
  ret_148 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_148 != 0)
    goto <bb 39>; [0.04%]
  else
    goto <bb 41>; [99.96%]

  <bb 39> [0.02%]:
  _149 = stderr;
  __fprintf_chk (_149, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_148);

  <bb 40> [0.02%]:
  cleanupAndQuit (-5);

  <bb 41> [46.66%]:
  ret_66 = terminateFlag;
  ret_150 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_150 != 0)
    goto <bb 42>; [0.04%]
  else
    goto <bb 44>; [99.96%]

  <bb 42> [0.02%]:
  _151 = stderr;
  __fprintf_chk (_151, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_150);

  <bb 43> [0.02%]:
  cleanupAndQuit (-6);

  <bb 44> [46.64%]:
  if (ret_66 != 0)
    goto <bb 45>; [5.00%]
  else
    goto <bb 48>; [95.00%]

  <bb 45> [4.79%]:
  _5 = MEM[(struct queue *)q_37(D)].mut;
  ret_152 = pthread_mutex_unlock (_5);
  if (ret_152 != 0)
    goto <bb 46>; [0.04%]
  else
    goto <bb 65>; [99.96%]

  <bb 46> [0.00%]:
  _153 = stderr;
  __fprintf_chk (_153, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_152);

  <bb 47> [0.00%]:
  cleanupAndQuit (-6);

  <bb 48> [93.48%]:
  _6 = MEM[(struct queue *)q_37(D)].mut;
  _7 = MEM[(struct queue *)q_37(D)].notEmpty;
  _154 = pthread_cond_wait (_7, _6);

  <bb 49> [93.48%]:
  if (_154 != 0)
    goto <bb 50>; [0.04%]
  else
    goto <bb 68>; [99.96%]

  <bb 68> [93.44%]:
  goto <bb 13>; [100.00%]

  <bb 50> [0.04%]:
  _155 = stderr;
  __fprintf_chk (_155, 1, "pthread_cond_wait error [%d]! Aborting immediately!\n", _154);

  <bb 51> [0.04%]:
  pthread_mutex_unlock (_6);
  cleanupAndQuit (-8);

  <bb 52> [1.65%]:
  _9 = MEM[(struct queue *)q_37(D)].mut;
  ret_156 = pthread_mutex_unlock (_9);
  if (ret_156 != 0)
    goto <bb 53>; [0.04%]
  else
    goto <bb 55>; [99.96%]

  <bb 53> [0.00%]:
  _157 = stderr;
  __fprintf_chk (_157, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_156);

  <bb 54> [0.00%]:
  cleanupAndQuit (-6);

  <bb 55> [1.65%]:
  _11 = fileData_211->bufSize;
  _12 = (double) _11;
  _13 = _12 * 1.0100000000000000088817841970012523233890533447265625e+0;
  _14 = _13 + 6.0e+2;
  _15 = (unsigned int) _14;
  outSize = _15;
  _47 = (sizetype) _15;
  CompressedData_49 = operator new [] (_47, &nothrow);
  if (CompressedData_49 == 0B)
    goto <bb 56>; [2.50%]
  else
    goto <bb 57>; [97.50%]

  <bb 56> [0.04%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (CompressedData)!  Aborting...\n");
  goto <bb 65>; [100.00%]

  <bb 57> [1.61%]:
  _17 = BWTblockSize;
  _19 = fileData_211->bufSize;
  _20 = fileData_211->buf;
  _51 = BZ2_bzBuffToBuffCompress (CompressedData_49, &outSize, _20, _19, _17, 0, 30);

  <bb 58> [1.61%]:
  if (_51 != 0)
    goto <bb 59>; [2.50%]
  else
    goto <bb 60>; [97.50%]

  <bb 59> [0.04%]:
  handle_error (1, -1, "pbzip2: *ERROR during compression: %d!  Aborting...\n", _51);
  goto <bb 65>; [100.00%]

  <bb 60> [1.57%]:
  _67 = MEM[(char * &)fileData_211];
  if (_67 != 0B)
    goto <bb 61>; [53.47%]
  else
    goto <bb 62>; [46.53%]

  <bb 61> [0.84%]:
  operator delete [] (_67);
  MEM[(char * &)fileData_211] = 0B;

  <bb 62> [1.57%]:
  _23 = fileData_211->bufSize;
  _24 = fileData_211->blockNumber;
  _25 = outSize;
  MEM[(struct  &)&outBlock] ={v} {CLOBBER};
  outBlock.buf = CompressedData_49;
  outBlock.bufSize = _25;
  outBlock.blockNumber = _24;
  outBlock.sequenceNumber = 0;
  outBlock.inSize = _23;
  outBlock.isLastInSequence = 1;
  outBlock.next = 0B;
  _53 = outputBufferAdd (&outBlock, "consumer");

  <bb 63> [1.57%]:
  if (_53 == 0B)
    goto <bb 65>; [2.50%]
  else
    goto <bb 64>; [97.50%]

  <bb 64> [1.53%]:
  operator delete (fileData_211, 40);
  outBlock ={v} {CLOBBER};
  goto <bb 3>; [100.00%]

  <bb 65> [5.08%]:
  outBlock ={v} {CLOBBER};
  outSize ={v} {CLOBBER};
  return 0B;

<L0> [0.00%]:
  resx 1

}



;; Function fileWriter (fileWriter, funcdef_no=40, decl_uid=4174, cgraph_uid=45, symbol_order=772)

fileWriter (void * outname)
{
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int D.5188;
  int ret;
  struct outBuff * outBlock;
  struct outBuff * prevBlockInSequence;
  off_t bytesProcessed;
  int ret;
  size_t outBufferPos;
  int currBlock;
  int hOutfile;
  int percentComplete;
  off_t CompressedSize;
  int _1;
  int _2;
  union pthread_mutex_t * _3;
  int _4;
  int _5;
  union pthread_mutex_t * _6;
  char * _7;
  struct outBuff * _8;
  union pthread_mutex_t * _9;
  union pthread_mutex_t * _10;
  union pthread_mutex_t * _11;
  unsigned int _12;
  long unsigned int _13;
  char * _14;
  int _15;
  unsigned int _16;
  long int _17;
  unsigned int _18;
  long int _19;
  char * _20;
  bool _21;
  long unsigned int _22;
  union pthread_mutex_t * _23;
  bool _24;
  int _25;
  int _26;
  int _27;
  long unsigned int _28;
  long unsigned int _29;
  union pthread_cond_t * _30;
  union pthread_mutex_t * _31;
  int _32;
  bool _33;
  int _34;
  int _35;
  long int _36;
  double _37;
  double _38;
  double _39;
  double _40;
  struct FILE * _41;
  struct FILE * _42;
  int _43;
  int _44;
  long unsigned int _45;
  struct FILE * _46;
  struct queue * _47;
  union pthread_mutex_t * _48;
  struct queue * _49;
  union pthread_cond_t * _50;
  struct queue * _51;
  union pthread_cond_t * _52;
  struct queue * _53;
  union pthread_mutex_t * _54;
  union pthread_mutex_t * _55;
  union pthread_cond_t * _56;
  union pthread_mutex_t * _57;
  int _58;
  struct FILE * _59;
  struct FILE * _60;
  int _92;
  bool _95;
  long int _103;
  long int _129;
  int _133;
  struct ErrorContext * _135;
  struct outBuff * _150;
  long unsigned int _151;
  struct value_type & _152;
  struct outBuff * _153;
  struct value_type & _154;
  bool _209;
  union pthread_mutex_t * _214;
  union pthread_mutex_t * _216;
  struct FILE * _218;
  struct FILE * _220;
  struct FILE * _222;
  struct FILE * _224;
  struct FILE * _226;
  struct FILE * _228;
  struct FILE * _230;
  struct FILE * _232;
  struct FILE * _234;
  struct FILE * _236;
  struct FILE * _238;
  struct FILE * _240;
  struct FILE * _242;
  struct FILE * _244;
  struct FILE * _246;
  struct FILE * _248;
  struct FILE * _250;
  struct FILE * _252;
  struct FILE * _254;
  bool _257;

  <bb 2> [33.59%]:
  _1 = OutputStdOut;
  if (_1 == 0)
    goto <bb 3>; [83.00%]
  else
    goto <bb 5>; [17.00%]

  <bb 3> [27.88%]:
  _92 = safe_open_output (outname_89(D));
  if (_92 == -1)
    goto <bb 4>; [83.00%]
  else
    goto <bb 5>; [17.00%]

  <bb 4> [23.14%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not create output file [%s]!\n", outname_89(D));
  goto <bb 91>; [100.00%]

  <bb 5> [10.45%]:
  # hOutfile_146 = PHI <1(2), _92(3)>
  goto <bb 92>; [100.00%]

  <bb 7> [89.55%]:
  # CompressedSize_74 = PHI <CompressedSize_105(61), CompressedSize_61(30), CompressedSize_105(64), CompressedSize_105(65)>
  # percentComplete_98 = PHI <percentComplete_62(61), percentComplete_62(30), percentComplete_63(64), percentComplete_63(65)>
  # currBlock_148 = PHI <currBlock_65(61), currBlock_64(30), currBlock_65(64), currBlock_65(65)>
  # outBufferPos_100 = PHI <outBufferPos_68(61), outBufferPos_66(30), outBufferPos_68(64), outBufferPos_68(65)>
  # bytesProcessed_86 = PHI <bytesProcessed_106(61), bytesProcessed_69(30), bytesProcessed_106(64), bytesProcessed_106(65)>
  # prevBlockInSequence_111 = PHI <prevBlockInSequence_71(61), prevBlockInSequence_70(30), prevBlockInSequence_71(64), prevBlockInSequence_71(65)>

  <bb 92> [100.00%]:
  # CompressedSize_61 = PHI <CompressedSize_74(7), 0(5)>
  # percentComplete_62 = PHI <percentComplete_98(7), 0(5)>
  # currBlock_64 = PHI <currBlock_148(7), 0(5)>
  # outBufferPos_66 = PHI <outBufferPos_100(7), 0(5)>
  # bytesProcessed_69 = PHI <bytesProcessed_86(7), 0(5)>
  # prevBlockInSequence_70 = PHI <prevBlockInSequence_111(7), 0B(5)>
  _214 = ProducerDoneMutex;
  ret_219 = pthread_mutex_lock (_214);
  if (ret_219 != 0)
    goto <bb 8>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 8> [0.04%]:
  _220 = stderr;
  __fprintf_chk (_220, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_219);
  cleanupAndQuit (-5);

  <bb 9> [99.96%]:
  ret_215 = producerDone;
  _216 = ProducerDoneMutex;
  ret_217 = pthread_mutex_unlock (_216);
  if (ret_217 != 0)
    goto <bb 10>; [0.04%]
  else
    goto <bb 11>; [99.96%]

  <bb 10> [0.04%]:
  _218 = stderr;
  __fprintf_chk (_218, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_217);
  cleanupAndQuit (-6);

  <bb 11> [99.92%]:
  if (ret_215 == 1)
    goto <bb 12>; [51.12%]
  else
    goto <bb 13>; [48.88%]

  <bb 12> [51.08%]:
  _2 = NumBlocks;
  _257 = prevBlockInSequence_70 == 0B;
  _95 = _2 <= currBlock_64;
  _209 = _95 & _257;
  if (_209 != 0)
    goto <bb 66>; [1.87%]
  else
    goto <bb 13>; [98.13%]

  <bb 13> [98.96%]:
  ret_223 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_223 != 0)
    goto <bb 14>; [0.04%]
  else
    goto <bb 15>; [99.96%]

  <bb 14> [0.04%]:
  _224 = stderr;
  __fprintf_chk (_224, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_223);
  cleanupAndQuit (-5);

  <bb 15> [98.92%]:
  ret_149 = terminateFlag;
  ret_221 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_221 != 0)
    goto <bb 16>; [0.04%]
  else
    goto <bb 17>; [99.96%]

  <bb 16> [0.04%]:
  _222 = stderr;
  __fprintf_chk (_222, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_221);
  cleanupAndQuit (-6);

  <bb 17> [98.88%]:
  if (ret_149 != 0)
    goto <bb 66>; [3.75%]
  else
    goto <bb 18>; [96.25%]

  <bb 18> [95.18%]:
  _3 = OutMutex;
  ret_225 = pthread_mutex_lock (_3);
  if (ret_225 != 0)
    goto <bb 19>; [0.04%]
  else
    goto <bb 20>; [99.96%]

  <bb 19> [0.04%]:
  _226 = stderr;
  __fprintf_chk (_226, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_225);
  cleanupAndQuit (-5);

  <bb 20> [95.14%]:
  _4 = LastGoodBlock;
  if (_4 != -1)
    goto <bb 21>; [66.00%]
  else
    goto <bb 24>; [34.00%]

  <bb 21> [62.79%]:
  _5 = NextBlockToWrite;
  if (_4 < _5)
    goto <bb 22>; [3.75%]
  else
    goto <bb 24>; [96.25%]

  <bb 22> [2.35%]:
  _6 = OutMutex;
  ret_227 = pthread_mutex_unlock (_6);
  if (ret_227 != 0)
    goto <bb 23>; [0.04%]
  else
    goto <bb 66>; [99.96%]

  <bb 23> [0.00%]:
  _228 = stderr;
  __fprintf_chk (_228, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_227);
  cleanupAndQuit (-6);

  <bb 24> [92.78%]:
  _150 = OutputBuffer.D.4140._M_impl._M_start;
  _151 = outBufferPos_66 * 40;
  _152 = _150 + _151;
  _7 = _152->buf;
  if (_7 == 0B)
    goto <bb 25>; [46.53%]
  else
    goto <bb 27>; [53.47%]

  <bb 25> [43.17%]:
  if (prevBlockInSequence_70 == 0B)
    goto <bb 30>; [9.42%]
  else
    goto <bb 26>; [90.58%]

  <bb 26> [39.10%]:
  _8 = prevBlockInSequence_70->next;
  if (_8 == 0B)
    goto <bb 30>; [17.43%]
  else
    goto <bb 27>; [82.57%]

  <bb 27> [81.90%]:
  _11 = OutMutex;
  ret_229 = pthread_mutex_unlock (_11);
  if (ret_229 != 0)
    goto <bb 28>; [0.04%]
  else
    goto <bb 29>; [99.96%]

  <bb 28> [0.03%]:
  _230 = stderr;
  __fprintf_chk (_230, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_229);
  cleanupAndQuit (-6);

  <bb 29> [81.87%]:
  if (prevBlockInSequence_70 != 0B)
    goto <bb 32>; [70.00%]
  else
    goto <bb 33>; [30.00%]

  <bb 30> [10.88%]:
  _9 = OutMutex;
  safe_cond_timed_wait (&OutBufferHeadNotEmpty, _9, 1, "fileWriter");
  _10 = OutMutex;
  ret_231 = pthread_mutex_unlock (_10);
  if (ret_231 != 0)
    goto <bb 31>; [0.04%]
  else
    goto <bb 7>; [99.96%]

  <bb 31> [0.00%]:
  _232 = stderr;
  __fprintf_chk (_232, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_231);
  cleanupAndQuit (-6);

  <bb 32> [57.31%]:
  outBlock_101 = prevBlockInSequence_70->next;
  goto <bb 34>; [100.00%]

  <bb 33> [24.56%]:
  _153 = OutputBuffer.D.4140._M_impl._M_start;
  _154 = _153 + _151;

  <bb 34> [81.87%]:
  # outBlock_72 = PHI <outBlock_101(32), _154(33)>
  _12 = outBlock_72->bufSize;
  _13 = (long unsigned int) _12;
  _14 = outBlock_72->buf;
  _103 = do_write (hOutfile_146, _14, _13);
  ret_104 = (int) _103;
  if (ret_104 < 0)
    goto <bb 35>; [3.75%]
  else
    goto <bb 38>; [96.25%]

  <bb 35> [3.07%]:
  _15 = OutputStdOut;
  if (_15 == 0)
    goto <bb 36>; [36.64%]
  else
    goto <bb 37>; [63.36%]

  <bb 36> [1.12%]:
  close (hOutfile_146);

  <bb 37> [3.07%]:
  _16 = outBlock_72->bufSize;
  handle_error (1, -1, "pbzip2: *ERROR: Could not write %d bytes to file [ret=%d]!  Aborting...\n", _16, ret_104);
  goto <bb 91>; [100.00%]

  <bb 38> [78.80%]:
  _17 = (long int) ret_104;
  CompressedSize_105 = _17 + CompressedSize_61;
  _18 = outBlock_72->inSize;
  _19 = (long int) _18;
  bytesProcessed_106 = _19 + bytesProcessed_69;
  _20 = outBlock_72->buf;
  if (_20 != 0B)
    goto <bb 39>; [53.47%]
  else
    goto <bb 40>; [46.53%]

  <bb 39> [42.13%]:
  operator delete [] (_20);

  <bb 40> [78.80%]:
  outBlock_72->buf = 0B;
  outBlock_72->bufSize = 0;
  _21 = outBlock_72->isLastInSequence;
  if (_21 != 0)
    goto <bb 41>; [50.00%]
  else
    goto <bb 44>; [50.00%]

  <bb 41> [39.40%]:
  outBufferPos_110 = outBufferPos_66 + 1;
  _22 = NumBufferedBlocksMax;
  if (_22 == outBufferPos_110)
    goto <bb 42>; [34.00%]
  else
    goto <bb 43>; [66.00%]

  <bb 42> [13.40%]:

  <bb 43> [39.40%]:
  # outBufferPos_67 = PHI <outBufferPos_110(41), 0(42)>
  currBlock_112 = currBlock_64 + 1;

  <bb 44> [78.80%]:
  # currBlock_65 = PHI <currBlock_64(40), currBlock_112(43)>
  # outBufferPos_68 = PHI <outBufferPos_66(40), outBufferPos_67(43)>
  _23 = OutMutex;
  ret_233 = pthread_mutex_lock (_23);
  if (ret_233 != 0)
    goto <bb 45>; [0.04%]
  else
    goto <bb 46>; [99.96%]

  <bb 45> [0.03%]:
  _234 = stderr;
  __fprintf_chk (_234, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_233);
  cleanupAndQuit (-5);

  <bb 46> [78.77%]:
  _24 = outBlock_72->isLastInSequence;
  if (_24 != 0)
    goto <bb 47>; [50.00%]
  else
    goto <bb 48>; [50.00%]

  <bb 47> [39.38%]:
  _25 = NextBlockToWrite;
  _26 = _25 + 1;
  NextBlockToWrite = _26;
  OutBufferPosToWrite = outBufferPos_68;

  <bb 48> [78.77%]:
  _27 = outBlock_72->sequenceNumber;
  if (_27 > 1)
    goto <bb 49>; [64.00%]
  else
    goto <bb 50>; [36.00%]

  <bb 49> [50.41%]:
  _28 = NumBufferedTailBlocks;
  _29 = _28 + 18446744073709551615;
  NumBufferedTailBlocks = _29;

  <bb 50> [78.77%]:
  _30 = notTooMuchNumBuffered;
  ret_239 = pthread_cond_broadcast (_30);
  if (ret_239 != 0)
    goto <bb 51>; [0.04%]
  else
    goto <bb 52>; [99.96%]

  <bb 51> [0.03%]:
  _240 = stderr;
  __fprintf_chk (_240, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_239);
  cleanupAndQuit (-7);

  <bb 52> [78.73%]:
  ret_237 = pthread_cond_broadcast (&ErrStateChangeCond);
  if (ret_237 != 0)
    goto <bb 53>; [0.04%]
  else
    goto <bb 54>; [99.96%]

  <bb 53> [0.03%]:
  _238 = stderr;
  __fprintf_chk (_238, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_237);
  cleanupAndQuit (-7);

  <bb 54> [78.70%]:
  _31 = OutMutex;
  ret_235 = pthread_mutex_unlock (_31);
  if (ret_235 != 0)
    goto <bb 55>; [0.04%]
  else
    goto <bb 56>; [99.96%]

  <bb 55> [0.03%]:
  _236 = stderr;
  __fprintf_chk (_236, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_235);
  cleanupAndQuit (-6);

  <bb 56> [78.67%]:
  _32 = outBlock_72->sequenceNumber;
  if (_32 > 2)
    goto <bb 57>; [33.00%]
  else
    goto <bb 58>; [67.00%]

  <bb 57> [25.96%]:
  operator delete (prevBlockInSequence_70, 40);

  <bb 58> [78.67%]:
  _33 = outBlock_72->isLastInSequence;
  if (_33 != 0)
    goto <bb 59>; [50.00%]
  else
    goto <bb 61>; [50.00%]

  <bb 59> [39.34%]:
  _34 = outBlock_72->sequenceNumber;
  if (_34 > 1)
    goto <bb 60>; [46.68%]
  else
    goto <bb 61>; [53.32%]

  <bb 60> [18.36%]:
  operator delete (outBlock_72, 40);

  <bb 61> [78.67%]:
  # prevBlockInSequence_71 = PHI <outBlock_72(58), 0B(59), 0B(60)>
  _35 = QuietMode;
  if (_35 != 1)
    goto <bb 62>; [66.00%]
  else
    goto <bb 7>; [34.00%]

  <bb 62> [51.92%]:
  _36 = InFileSize;
  if (_36 > 0)
    goto <bb 63>; [64.00%]
  else
    goto <bb 64>; [36.00%]

  <bb 63> [33.23%]:
  _37 = (double) bytesProcessed_106;
  _38 = _37 * 1.0e+2;
  _39 = (double) _36;
  _40 = _38 / _39;
  percentComplete_122 = (int) _40;

  <bb 64> [51.92%]:
  # percentComplete_63 = PHI <percentComplete_62(62), percentComplete_122(63)>
  if (percentComplete_62 != percentComplete_63)
    goto <bb 65>; [48.88%]
  else
    goto <bb 7>; [51.12%]

  <bb 65> [25.38%]:
  _41 = stderr;
  __fprintf_chk (_41, 1, "Completed: %d%%             \r", percentComplete_63);
  _42 = stderr;
  fflush (_42);
  goto <bb 7>; [100.00%]

  <bb 66> [7.02%]:
  if (currBlock_64 == 0)
    goto <bb 67>; [33.00%]
  else
    goto <bb 69>; [67.00%]

  <bb 67> [2.32%]:
  _129 = do_write (hOutfile_146, &Bz2HeaderZero, 14);
  ret_130 = (int) _129;
  if (ret_130 < 0)
    goto <bb 68>; [19.09%]
  else
    goto <bb 69>; [80.91%]

  <bb 68> [0.44%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not write to file!  Aborting...\n");
  goto <bb 91>; [100.00%]

  <bb 69> [6.58%]:
  _43 = OutputStdOut;
  if (_43 == 0)
    goto <bb 70>; [33.00%]
  else
    goto <bb 72>; [67.00%]

  <bb 70> [2.17%]:
  _133 = close (hOutfile_146);
  if (_133 == -1)
    goto <bb 71>; [17.77%]
  else
    goto <bb 72>; [82.23%]

  <bb 71> [0.39%]:
  _135 = getInstance ();
  saveError (_135);
  handle_error (1, -1, "pbzip2: *ERROR: Could not close output file!  Aborting...\n");
  goto <bb 91>; [100.00%]

  <bb 72> [6.19%]:
  _44 = QuietMode;
  if (_44 != 1)
    goto <bb 73>; [48.88%]
  else
    goto <bb 74>; [51.12%]

  <bb 73> [3.03%]:
  _45 = (long unsigned int) CompressedSize_61;
  _46 = stderr;
  __fprintf_chk (_46, 1, "    Output Size: %lu bytes\n", _45);

  <bb 74> [6.19%]:
  _47 = FifoQueue;
  if (_47 != 0B)
    goto <bb 75>; [53.47%]
  else
    goto <bb 83>; [46.53%]

  <bb 75> [3.31%]:
  _48 = _47->mut;
  ret_247 = pthread_mutex_lock (_48);
  if (ret_247 != 0)
    goto <bb 76>; [0.04%]
  else
    goto <bb 77>; [99.96%]

  <bb 76> [0.00%]:
  _248 = stderr;
  __fprintf_chk (_248, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_247);
  cleanupAndQuit (-5);

  <bb 77> [3.31%]:
  _49 = FifoQueue;
  _50 = _49->notEmpty;
  ret_245 = pthread_cond_broadcast (_50);
  if (ret_245 != 0)
    goto <bb 78>; [0.04%]
  else
    goto <bb 79>; [99.96%]

  <bb 78> [0.00%]:
  _246 = stderr;
  __fprintf_chk (_246, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_245);
  cleanupAndQuit (-7);

  <bb 79> [3.31%]:
  _51 = FifoQueue;
  _52 = _51->notFull;
  ret_243 = pthread_cond_broadcast (_52);
  if (ret_243 != 0)
    goto <bb 80>; [0.04%]
  else
    goto <bb 81>; [99.96%]

  <bb 80> [0.00%]:
  _244 = stderr;
  __fprintf_chk (_244, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_243);
  cleanupAndQuit (-7);

  <bb 81> [3.31%]:
  _53 = FifoQueue;
  _54 = _53->mut;
  ret_241 = pthread_mutex_unlock (_54);
  if (ret_241 != 0)
    goto <bb 82>; [0.04%]
  else
    goto <bb 83>; [99.96%]

  <bb 82> [0.00%]:
  _242 = stderr;
  __fprintf_chk (_242, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_241);
  cleanupAndQuit (-6);

  <bb 83> [6.19%]:
  _55 = OutMutex;
  ret_253 = pthread_mutex_lock (_55);
  if (ret_253 != 0)
    goto <bb 84>; [0.04%]
  else
    goto <bb 85>; [99.96%]

  <bb 84> [0.00%]:
  _254 = stderr;
  __fprintf_chk (_254, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_253);
  cleanupAndQuit (-5);

  <bb 85> [6.18%]:
  _56 = notTooMuchNumBuffered;
  ret_251 = pthread_cond_broadcast (_56);
  if (ret_251 != 0)
    goto <bb 86>; [0.04%]
  else
    goto <bb 87>; [99.96%]

  <bb 86> [0.00%]:
  _252 = stderr;
  __fprintf_chk (_252, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_251);
  cleanupAndQuit (-7);

  <bb 87> [6.18%]:
  _57 = OutMutex;
  ret_249 = pthread_mutex_unlock (_57);
  if (ret_249 != 0)
    goto <bb 88>; [0.04%]
  else
    goto <bb 89>; [99.96%]

  <bb 88> [0.00%]:
  _250 = stderr;
  __fprintf_chk (_250, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_249);
  cleanupAndQuit (-6);

  <bb 89> [6.18%]:
  _58 = QuietMode;
  if (_58 != 1)
    goto <bb 90>; [52.88%]
  else
    goto <bb 91>; [47.12%]

  <bb 90> [3.27%]:
  _59 = stderr;
  __fprintf_chk (_59, 1, "Completed: %d%%             \r", 100);
  _60 = stderr;
  fflush (_60);

  <bb 91> [33.22%]:
  return 0B;

}



;; Function consumer_decompress (consumer_decompress, funcdef_no=41, decl_uid=4173, cgraph_uid=44, symbol_order=771)

consumer_decompress (void * q)
{
  int D.5343;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  struct outBuff * D.5320;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  bool changed;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int D.5280;
  int ret;
  int ret;
  struct ElementType * & headElem;
  int D.5274;
  int ret;
  int ret;
  int ret;
  int ret;
  int D.5264;
  int ret;
  int ret;
  bool isInterrupted;
  int ret;
  int D.5255;
  int ret;
  int ret;
  int ret;
  bool isLast;
  unsigned int len;
  struct outBuff * addret;
  unsigned int availIn;
  int bzret;
  struct bz_stream strm;
  unsigned int processedIn;
  int outSequenceNumber;
  struct outBuff * prevOutBlockInSequence;
  char * DecompressedData;
  struct outBuff * lastFileData;
  struct outBuff * fileData;
  struct outBuff D.5237;
  union pthread_mutex_t * _1;
  bool _2;
  union pthread_mutex_t * _3;
  int _4;
  bool _5;
  int _6;
  union pthread_mutex_t * _7;
  int _8;
  int _9;
  union pthread_mutex_t * _10;
  union pthread_cond_t * _11;
  union pthread_cond_t * _12;
  union pthread_mutex_t * _13;
  int _15;
  unsigned int _18;
  char * _19;
  unsigned int _20;
  unsigned int _21;
  unsigned int _22;
  unsigned int _23;
  unsigned int _24;
  int _26;
  int _28;
  unsigned int _29;
  int _30;
  bool _32;
  unsigned int _33;
  bool _34;
  bool _35;
  bool _70;
  bool _80;
  int _91;
  int _102;
  struct outBuff * _109;
  int _111;
  void * _114;
  int _123;
  bool _128;
  bool _131;
  char * _132;
  int _167;
  struct FILE * _238;
  int _241;
  bool _242;
  int _244;
  struct FILE * _246;
  struct FILE * _248;
  union pthread_mutex_t * _249;
  union pthread_mutex_t * _251;
  struct FILE * _253;
  struct FILE * _255;
  struct FILE * _257;
  struct ElementType * * _258;
  long int _259;
  long unsigned int _260;
  long unsigned int _261;
  bool _263;
  bool _264;
  struct outBuff * _265;
  int _266;
  struct ElementType * _267;
  bool _268;
  long int _269;
  long int _270;
  long int _271;
  long int _272;
  long int _273;
  long int _274;
  int _275;
  struct FILE * _277;
  struct FILE * _279;
  union pthread_mutex_t * _280;
  union pthread_mutex_t * _282;
  struct FILE * _284;
  struct FILE * _286;
  struct FILE * _288;
  struct FILE * _290;
  struct FILE * _292;
  struct FILE * _294;
  int _295;
  struct FILE * _296;
  union pthread_mutex_t * _297;
  int _298;
  int _299;
  union pthread_cond_t * _301;
  union pthread_mutex_t * _302;
  struct queue * _303;
  union pthread_mutex_t * _304;
  struct queue * _305;
  union pthread_cond_t * _306;
  struct queue * _307;
  union pthread_cond_t * _308;
  struct queue * _309;
  union pthread_mutex_t * _310;
  union pthread_mutex_t * _311;
  struct FILE * _313;
  struct FILE * _315;
  struct FILE * _317;
  struct FILE * _319;
  struct FILE * _321;
  struct FILE * _323;
  struct FILE * _325;
  union pthread_mutex_t * _326;
  long unsigned int _327;
  long unsigned int _328;
  char * _329;
  union pthread_mutex_t * _331;
  int _332;
  int _333;
  union pthread_mutex_t * _334;
  union pthread_mutex_t * _335;
  union pthread_cond_t * _336;
  long unsigned int _337;
  char * _338;
  union pthread_mutex_t * _339;
  bool _340;
  struct FILE * _342;
  struct FILE * _344;
  struct FILE * _346;
  int _347;
  struct FILE * _348;
  struct FILE * _350;
  struct FILE * _352;
  unsigned int _353;
  bool _354;
  bool _355;
  bool _356;
  bool _357;
  int _358;
  int _359;
  unsigned int _360;
  int _361;
  bool _362;
  int _363;
  int _364;
  bool _374;
  bool _382;
  bool _392;
  bool _490;

  <bb 2> [7.94%]:
  strm.bzalloc = 0B;
  strm.bzfree = 0B;
  strm.opaque = 0B;

  <bb 3> [8.70%]:
  # DecompressedData_38 = PHI <0B(2), DecompressedData_442(143)>
  # prevOutBlockInSequence_41 = PHI <0B(2), prevOutBlockInSequence_43(143)>
  # outSequenceNumber_44 = PHI <-1(2), outSequenceNumber_47(143)>
  # processedIn_48 = PHI <0(2), processedIn_36(143)>
  # lastFileData_157 = PHI <0B(2), fileData_493(143)>
  _1 = MEM[(struct queue *)q_71(D)].mut;
  ret_237 = pthread_mutex_lock (_1);
  if (ret_237 != 0)
    goto <bb 4>; [0.04%]
  else
    goto <bb 5>; [99.96%]

  <bb 4> [0.00%]:
  _238 = stderr;
  __fprintf_chk (_238, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_237);
  cleanupAndQuit (-5);

  <bb 5> [100.00%]:
  # fileData_491 = PHI <lastFileData_157(3), fileData_494(55)>
  ret_247 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_247 != 0)
    goto <bb 6>; [0.04%]
  else
    goto <bb 7>; [99.96%]

  <bb 6> [0.04%]:
  _248 = stderr;
  __fprintf_chk (_248, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_247);
  cleanupAndQuit (-5);

  <bb 7> [99.96%]:
  ret_239 = terminateFlag;
  ret_245 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_245 != 0)
    goto <bb 8>; [0.04%]
  else
    goto <bb 9>; [99.96%]

  <bb 8> [0.04%]:
  _246 = stderr;
  __fprintf_chk (_246, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_245);
  cleanupAndQuit (-6);

  <bb 9> [99.92%]:
  _5 = ret_239 != 0;
  _249 = OutMutex;
  ret_254 = pthread_mutex_lock (_249);
  if (ret_254 != 0)
    goto <bb 10>; [0.04%]
  else
    goto <bb 11>; [99.96%]

  <bb 10> [0.04%]:
  _255 = stderr;
  __fprintf_chk (_255, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_254);
  cleanupAndQuit (-5);

  <bb 11> [99.88%]:
  ret_250 = MinErrorBlock;
  _251 = OutMutex;
  ret_252 = pthread_mutex_unlock (_251);
  if (ret_252 != 0)
    goto <bb 12>; [0.04%]
  else
    goto <bb 13>; [99.96%]

  <bb 12> [0.04%]:
  _253 = stderr;
  __fprintf_chk (_253, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_252);
  cleanupAndQuit (-6);

  <bb 13> [99.84%]:
  if (ret_250 != -1)
    goto <bb 14>; [66.00%]
  else
    goto <bb 17>; [34.00%]

  <bb 14> [65.89%]:
  if (fileData_491 == 0B)
    goto <bb 18>; [26.74%]
  else
    goto <bb 15>; [73.26%]

  <bb 15> [48.27%]:
  _241 = MEM[(const struct outBuff *)fileData_491].blockNumber;
  if (_241 >= ret_250)
    goto <bb 18>; [46.00%]
  else
    goto <bb 16>; [54.00%]

  <bb 16> [26.07%]:
  _242 = MEM[(const struct outBuff *)fileData_491].isLastInSequence;
  if (_242 != 0)
    goto <bb 18>; [46.00%]
  else
    goto <bb 17>; [54.00%]

  <bb 17> [48.02%]:
  _128 = _5;
  _167 = (int) _128;

  <bb 18> [99.84%]:
  # _244 = PHI <1(14), 1(15), 1(16), _167(17)>
  if (_244 != 0)
    goto <bb 19>; [3.75%]
  else
    goto <bb 21>; [96.25%]

  <bb 19> [3.74%]:
  _3 = MEM[(struct queue *)q_71(D)].mut;
  ret_256 = pthread_mutex_unlock (_3);
  if (ret_256 != 0)
    goto <bb 20>; [0.04%]
  else
    goto <bb 144>; [99.96%]

  <bb 20> [0.00%]:
  _257 = stderr;
  __fprintf_chk (_257, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_256);
  cleanupAndQuit (-6);

  <bb 21> [96.10%]:
  _4 = MEM[(struct queue *)q_71(D)].empty;
  if (_4 == 0)
    goto <bb 22>; [33.00%]
  else
    goto <bb 40>; [67.00%]

  <bb 22> [31.71%]:
  _258 = MEM[(struct queue *)q_71(D)].qData;
  _259 = MEM[(struct queue *)q_71(D)].head;
  _260 = (long unsigned int) _259;
  _261 = _260 * 8;
  headElem_262 = _258 + _261;
  if (fileData_491 != 0B)
    goto <bb 23>; [70.00%]
  else
    goto <bb 25>; [30.00%]

  <bb 23> [22.20%]:
  _264 = MEM[(struct ElementType *)fileData_491].isLastInSequence;
  if (_264 != 0)
    goto <bb 25>; [50.00%]
  else
    goto <bb 24>; [50.00%]

  <bb 24> [11.10%]:
  _265 = MEM[(struct ElementType *)fileData_491].next;
  if (_265 != 0B)
    goto <bb 31>; [73.26%]
  else
    goto <bb 34>; [26.74%]

  <bb 25> [20.61%]:
  _266 = MEM[(struct queue *)q_71(D)].topLevelEmpty;
  if (_266 != 0)
    goto <bb 34>; [46.00%]
  else
    goto <bb 26>; [54.00%]

  <bb 26> [11.13%]:
  _267 = *headElem_262;
  _269 = _259 + 1;
  MEM[(struct queue *)q_71(D)].head = _269;
  _270 = MEM[(struct queue *)q_71(D)].size;
  if (_269 == _270)
    goto <bb 27>; [34.00%]
  else
    goto <bb 28>; [66.00%]

  <bb 27> [3.78%]:
  MEM[(struct queue *)q_71(D)].head = 0;

  <bb 28> [11.13%]:
  _271 = MEM[(struct queue *)q_71(D)].head;
  _272 = MEM[(struct queue *)q_71(D)].tail;
  if (_271 == _272)
    goto <bb 29>; [34.00%]
  else
    goto <bb 30>; [66.00%]

  <bb 29> [3.78%]:
  MEM[(struct queue *)q_71(D)].topLevelEmpty = 1;

  <bb 30> [11.13%]:
  MEM[(struct queue *)q_71(D)].topLevelFull = 0;

  <bb 31> [19.26%]:
  # fileData_492 = PHI <_265(24), _267(30)>
  _273 = MEM[(struct queue *)q_71(D)].count;
  _274 = _273 + -1;
  MEM[(struct queue *)q_71(D)].count = _274;
  if (_274 == 0)
    goto <bb 32>; [50.00%]
  else
    goto <bb 33>; [50.00%]

  <bb 32> [9.63%]:
  MEM[(struct queue *)q_71(D)].empty = 1;

  <bb 33> [19.26%]:
  MEM[(struct queue *)q_71(D)].full = 0;

  <bb 34> [31.71%]:
  # _275 = PHI <0(24), 0(25), 1(33)>
  # fileData_493 = PHI <fileData_491(24), fileData_491(25), fileData_492(33)>
  if (_275 == 1)
    goto <bb 35>; [3.75%]
  else
    goto <bb 40>; [96.25%]

  <bb 35> [1.19%]:
  _12 = MEM[(struct queue *)q_71(D)].notFull;
  ret_278 = pthread_cond_signal (_12);
  if (ret_278 != 0)
    goto <bb 36>; [0.04%]
  else
    goto <bb 37>; [99.96%]

  <bb 36> [0.00%]:
  _279 = stderr;
  __fprintf_chk (_279, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_278);
  cleanupAndQuit (-7);

  <bb 37> [1.19%]:
  _13 = MEM[(struct queue *)q_71(D)].mut;
  ret_276 = pthread_mutex_unlock (_13);
  if (ret_276 != 0)
    goto <bb 38>; [0.04%]
  else
    goto <bb 39>; [99.96%]

  <bb 38> [0.00%]:
  _277 = stderr;
  __fprintf_chk (_277, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_276);
  cleanupAndQuit (-6);

  <bb 39> [1.19%]:
  if (lastFileData_157 != 0B)
    goto <bb 61>; [53.47%]
  else
    goto <bb 62>; [46.53%]

  <bb 40> [94.91%]:
  # fileData_494 = PHI <fileData_491(21), fileData_493(34)>
  _6 = MEM[(struct queue *)q_71(D)].empty;
  if (_6 != 0)
    goto <bb 41>; [50.00%]
  else
    goto <bb 55>; [50.00%]

  <bb 41> [47.45%]:
  _280 = ProducerDoneMutex;
  ret_285 = pthread_mutex_lock (_280);
  if (ret_285 != 0)
    goto <bb 42>; [0.04%]
  else
    goto <bb 43>; [99.96%]

  <bb 42> [0.02%]:
  _286 = stderr;
  __fprintf_chk (_286, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_285);
  cleanupAndQuit (-5);

  <bb 43> [47.43%]:
  ret_281 = producerDone;
  _282 = ProducerDoneMutex;
  ret_283 = pthread_mutex_unlock (_282);
  if (ret_283 != 0)
    goto <bb 44>; [0.04%]
  else
    goto <bb 45>; [99.96%]

  <bb 44> [0.02%]:
  _284 = stderr;
  __fprintf_chk (_284, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_283);
  cleanupAndQuit (-6);

  <bb 45> [47.42%]:
  if (ret_281 == 1)
    goto <bb 51>; [3.75%]
  else
    goto <bb 46>; [96.25%]

  <bb 46> [45.64%]:
  ret_289 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_289 != 0)
    goto <bb 47>; [0.04%]
  else
    goto <bb 48>; [99.96%]

  <bb 47> [0.02%]:
  _290 = stderr;
  __fprintf_chk (_290, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_289);
  cleanupAndQuit (-5);

  <bb 48> [45.62%]:
  ret_106 = terminateFlag;
  ret_287 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_287 != 0)
    goto <bb 49>; [0.04%]
  else
    goto <bb 50>; [99.96%]

  <bb 49> [0.02%]:
  _288 = stderr;
  __fprintf_chk (_288, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_287);
  cleanupAndQuit (-6);

  <bb 50> [45.60%]:
  if (ret_106 != 0)
    goto <bb 51>; [3.75%]
  else
    goto <bb 55>; [96.25%]

  <bb 51> [3.49%]:
  _7 = MEM[(struct queue *)q_71(D)].mut;
  pthread_mutex_unlock (_7);
  ret_293 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_293 != 0)
    goto <bb 52>; [0.04%]
  else
    goto <bb 53>; [99.96%]

  <bb 52> [0.00%]:
  _294 = stderr;
  __fprintf_chk (_294, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_293);
  cleanupAndQuit (-5);

  <bb 53> [3.49%]:
  ret_130 = terminateFlag;
  ret_291 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_291 != 0)
    goto <bb 54>; [0.04%]
  else
    goto <bb 57>; [99.96%]

  <bb 54> [0.00%]:
  _292 = stderr;
  __fprintf_chk (_292, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_291);
  cleanupAndQuit (-6);

  <bb 55> [91.34%]:
  _10 = MEM[(struct queue *)q_71(D)].mut;
  _11 = MEM[(struct queue *)q_71(D)].notEmpty;
  _295 = pthread_cond_wait (_11, _10);
  if (_295 != 0)
    goto <bb 56>; [0.04%]
  else
    goto <bb 5>; [99.96%]

  <bb 56> [0.04%]:
  _296 = stderr;
  __fprintf_chk (_296, 1, "pthread_cond_wait error [%d]! Aborting immediately!\n", _295);
  pthread_mutex_unlock (_10);
  cleanupAndQuit (-8);

  <bb 57> [3.49%]:
  _70 = outSequenceNumber_44 != -1;
  _374 = ret_130 == 0;
  _490 = _70 & _374;
  if (_490 != 0)
    goto <bb 58>; [24.44%]
  else
    goto <bb 59>; [75.56%]

  <bb 58> [0.85%]:
  _8 = lastFileData_157->sequenceNumber;
  _9 = lastFileData_157->blockNumber;
  handle_error (1, -1, "pbzip2: *ERROR on decompress - premature end of archive stream (block=%d; seq=%d; outseq=%d)!\n", _9, _8, outSequenceNumber_44);

  <bb 59> [3.49%]:
  if (lastFileData_157 != 0B)
    goto <bb 60>; [53.47%]
  else
    goto <bb 144>; [46.53%]

  <bb 60> [1.86%]:
  operator delete (lastFileData_157, 40);
  goto <bb 144>; [100.00%]

  <bb 61> [0.64%]:
  operator delete (lastFileData_157, 40);

  <bb 62> [1.19%]:
  _15 = fileData_493->sequenceNumber;
  if (_15 <= 1)
    goto <bb 63>; [21.69%]
  else
    goto <bb 65>; [78.31%]

  <bb 63> [0.26%]:
  _91 = BZ2_bzDecompressInit (&strm, 0, 0);
  if (_91 != 0)
    goto <bb 64>; [2.00%]
  else
    goto <bb 65>; [98.00%]

  <bb 64> [0.01%]:
  handle_error (1, -1, "pbzip2: *ERROR during BZ2_bzDecompressInit: %d\n", _91);
  goto <bb 144>; [100.00%]

  <bb 65> [1.18%]:
  # bzret_51 = PHI <0(62), _91(63)>
  _18 = fileData_493->bufSize;
  strm.avail_in = _18;
  _19 = fileData_493->buf;
  strm.next_in = _19;
  if (bzret_51 != 0)
    goto <bb 127>; [3.00%]
  else
    goto <bb 66>; [97.00%]

  <bb 66> [1.15%]:
  _29 = strm.avail_in;
  if (_29 == 0)
    goto <bb 127>; [3.00%]
  else
    goto <bb 69>; [97.00%]

  <bb 67> [11.96%]:
  # DecompressedData_54 = PHI <0B(125), DecompressedData_40(75)>
  # prevOutBlockInSequence_243 = PHI <addret_53(125), prevOutBlockInSequence_116(75)>
  # outSequenceNumber_129 = PHI <outSequenceNumber_46(125), outSequenceNumber_31(75)>
  # processedIn_495 = PHI <processedIn_103(125), processedIn_103(75)>
  # bzret_381 = PHI <_102(125), _102(75)>
  if (bzret_381 != 0)
    goto <bb 127>; [3.00%]
  else
    goto <bb 68>; [97.00%]

  <bb 68> [11.59%]:
  _20 = strm.avail_in;
  if (_20 == 0)
    goto <bb 127>; [3.00%]
  else
    goto <bb 69>; [97.00%]

  <bb 69> [12.36%]:
  # DecompressedData_27 = PHI <DecompressedData_54(68), DecompressedData_38(66)>
  # prevOutBlockInSequence_116 = PHI <prevOutBlockInSequence_243(68), prevOutBlockInSequence_41(66)>
  # outSequenceNumber_31 = PHI <outSequenceNumber_129(68), outSequenceNumber_44(66)>
  # processedIn_121 = PHI <processedIn_495(68), processedIn_48(66)>
  if (DecompressedData_27 == 0B)
    goto <bb 70>; [30.00%]
  else
    goto <bb 73>; [70.00%]

  <bb 70> [3.71%]:
  DecompressedData_96 = operator new [] (900000, &nothrow);
  if (DecompressedData_96 == 0B)
    goto <bb 71>; [3.00%]
  else
    goto <bb 72>; [97.00%]

  <bb 71> [0.11%]:
  handle_error (1, -1, " *ERROR: Could not allocate memory (DecompressedData)!  Aborting...\n");
  goto <bb 144>; [100.00%]

  <bb 72> [3.60%]:
  strm.avail_out = 900000;
  strm.next_out = DecompressedData_96;

  <bb 73> [12.25%]:
  # DecompressedData_40 = PHI <DecompressedData_27(69), DecompressedData_96(72)>
  # processedIn_50 = PHI <processedIn_121(69), 0(72)>
  availIn_100 = strm.avail_in;
  _102 = BZ2_bzDecompress (&strm);
  _21 = strm.avail_in;
  _22 = availIn_100 - _21;
  processedIn_103 = _22 + processedIn_50;
  if (_102 == 0)
    goto <bb 74>; [50.00%]
  else
    goto <bb 75>; [50.00%]

  <bb 74> [6.12%]:
  _23 = strm.avail_out;
  if (_23 == 0)
    goto <bb 76>; [50.00%]
  else
    goto <bb 75>; [50.00%]

  <bb 75> [9.19%]:
  if (_102 == 4)
    goto <bb 76>; [34.00%]
  else
    goto <bb 67>; [66.00%]

  <bb 76> [6.19%]:
  _24 = strm.avail_out;
  len_104 = 900000 - _24;
  isLast_105 = _102 == 4;
  if (_102 == 4)
    goto <bb 77>; [34.00%]
  else
    goto <bb 99>; [66.00%]

  <bb 77> [2.10%]:
  if (_21 != 0)
    goto <bb 79>; [33.00%]
  else
    goto <bb 78>; [67.00%]

  <bb 78> [1.41%]:
  _131 = MEM[(const struct outBuff *)fileData_493].isLastInSequence;
  if (_131 != 0)
    goto <bb 99>; [50.00%]
  else
    goto <bb 79>; [50.00%]

  <bb 79> [1.40%]:
  _26 = fileData_493->blockNumber;
  _297 = OutMutex;
  ret_312 = pthread_mutex_lock (_297);
  if (ret_312 != 0)
    goto <bb 80>; [0.04%]
  else
    goto <bb 81>; [99.96%]

  <bb 80> [0.00%]:
  _313 = stderr;
  __fprintf_chk (_313, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_312);
  cleanupAndQuit (-5);

  <bb 81> [1.40%]:
  _298 = LastGoodBlock;
  _2 = _26 < _298;
  _382 = _298 == -1;
  _80 = _2 | _382;
  if (_80 != 0)
    goto <bb 82>; [67.00%]
  else
    goto <bb 83>; [33.00%]

  <bb 82> [0.94%]:
  LastGoodBlock = _26;

  <bb 83> [1.40%]:
  # changed_300 = PHI <0(81), 1(82)>
  _299 = MinErrorBlock;
  _263 = _26 < _299;
  _268 = _299 == -1;
  _392 = _263 | _268;
  if (_392 != 0)
    goto <bb 84>; [67.00%]
  else
    goto <bb 85>; [33.00%]

  <bb 84> [0.94%]:
  MinErrorBlock = _26;
  goto <bb 86>; [100.00%]

  <bb 85> [0.46%]:
  if (changed_300 != 0)
    goto <bb 86>; [67.00%]
  else
    goto <bb 97>; [33.00%]

  <bb 86> [1.25%]:
  ret_318 = pthread_cond_signal (&ErrStateChangeCond);
  if (ret_318 != 0)
    goto <bb 87>; [0.04%]
  else
    goto <bb 88>; [99.96%]

  <bb 87> [0.00%]:
  _319 = stderr;
  __fprintf_chk (_319, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_318);
  cleanupAndQuit (-7);

  <bb 88> [1.25%]:
  ret_316 = pthread_cond_signal (&OutBufferHeadNotEmpty);
  if (ret_316 != 0)
    goto <bb 89>; [0.04%]
  else
    goto <bb 90>; [99.96%]

  <bb 89> [0.00%]:
  _317 = stderr;
  __fprintf_chk (_317, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_316);
  cleanupAndQuit (-7);

  <bb 90> [1.24%]:
  _301 = notTooMuchNumBuffered;
  pthread_cond_broadcast (_301);
  _302 = OutMutex;
  ret_314 = pthread_mutex_unlock (_302);
  if (ret_314 != 0)
    goto <bb 91>; [0.04%]
  else
    goto <bb 92>; [99.96%]

  <bb 91> [0.00%]:
  _315 = stderr;
  __fprintf_chk (_315, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_314);
  cleanupAndQuit (-6);

  <bb 92> [1.24%]:
  _303 = FifoQueue;
  if (_303 != 0B)
    goto <bb 93>; [53.47%]
  else
    goto <bb 99>; [46.53%]

  <bb 93> [0.67%]:
  _304 = _303->mut;
  ret_322 = pthread_mutex_lock (_304);
  if (ret_322 != 0)
    goto <bb 94>; [0.04%]
  else
    goto <bb 95>; [99.96%]

  <bb 94> [0.00%]:
  _323 = stderr;
  __fprintf_chk (_323, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_322);
  cleanupAndQuit (-5);

  <bb 95> [0.66%]:
  _305 = FifoQueue;
  _306 = _305->notFull;
  pthread_cond_broadcast (_306);
  _307 = FifoQueue;
  _308 = _307->notEmpty;
  pthread_cond_broadcast (_308);
  _309 = FifoQueue;
  _310 = _309->mut;
  ret_320 = pthread_mutex_unlock (_310);
  if (ret_320 != 0)
    goto <bb 96>; [0.04%]
  else
    goto <bb 99>; [99.96%]

  <bb 96> [0.00%]:
  _321 = stderr;
  __fprintf_chk (_321, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_320);
  cleanupAndQuit (-6);

  <bb 97> [0.15%]:
  _311 = OutMutex;
  ret_324 = pthread_mutex_unlock (_311);
  if (ret_324 != 0)
    goto <bb 98>; [0.04%]
  else
    goto <bb 99>; [99.96%]

  <bb 98> [0.00%]:
  _325 = stderr;
  __fprintf_chk (_325, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_324);
  cleanupAndQuit (-6);

  <bb 99> [6.18%]:
  if (outSequenceNumber_31 > 0)
    goto <bb 100>; [46.68%]
  else
    goto <bb 124>; [53.32%]

  <bb 100> [2.89%]:
  outSequenceNumber_112 = outSequenceNumber_31 + 1;
  _114 = operator new (40, &nothrow);
  if (_114 != 0B)
    goto <bb 101>; [70.00%]
  else
    goto <bb 102>; [30.00%]

  <bb 101> [2.02%]:
  _28 = fileData_493->blockNumber;
  MEM[(struct  &)_114] ={v} {CLOBBER};
  MEM[(struct outBuff *)_114].buf = DecompressedData_40;
  MEM[(struct outBuff *)_114].bufSize = len_104;
  MEM[(struct outBuff *)_114].blockNumber = _28;
  MEM[(struct outBuff *)_114].sequenceNumber = outSequenceNumber_112;
  MEM[(struct outBuff *)_114].inSize = processedIn_103;
  MEM[(struct outBuff *)_114].isLastInSequence = isLast_105;
  MEM[(struct outBuff *)_114].next = 0B;

  <bb 102> [2.89%]:
  if (_114 == 0B)
    goto <bb 103>; [3.00%]
  else
    goto <bb 104>; [97.00%]

  <bb 103> [0.09%]:
  BZ2_bzDecompressEnd (&strm);
  handle_error (1, -1, " *ERROR: Could not allocate memory (nextOutBlock)!  Aborting...\n");
  goto <bb 144>; [100.00%]

  <bb 104> [2.80%]:
  _326 = OutMutex;
  ret_341 = pthread_mutex_lock (_326);
  if (ret_341 != 0)
    goto <bb 105>; [0.04%]
  else
    goto <bb 106>; [99.96%]

  <bb 105> [0.00%]:
  _342 = stderr;
  __fprintf_chk (_342, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_341);
  cleanupAndQuit (-5);

  <bb 106> [20.57%]:
  _327 = NumBufferedTailBlocks;
  _328 = NumBufferedBlocksMax;
  if (_327 < _328)
    goto <bb 119>; [3.75%]
  else
    goto <bb 107>; [96.25%]

  <bb 107> [19.80%]:
  _329 = prevOutBlockInSequence_116->buf;
  if (_329 == 0B)
    goto <bb 119>; [3.75%]
  else
    goto <bb 108>; [96.25%]

  <bb 108> [19.06%]:
  ret_345 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_345 != 0)
    goto <bb 109>; [0.04%]
  else
    goto <bb 110>; [99.96%]

  <bb 109> [0.01%]:
  _346 = stderr;
  __fprintf_chk (_346, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_345);
  cleanupAndQuit (-5);

  <bb 110> [19.05%]:
  ret_330 = terminateFlag;
  ret_343 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_343 != 0)
    goto <bb 111>; [0.04%]
  else
    goto <bb 112>; [99.96%]

  <bb 111> [0.01%]:
  _344 = stderr;
  __fprintf_chk (_344, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_343);
  cleanupAndQuit (-6);

  <bb 112> [19.04%]:
  if (ret_330 != 0)
    goto <bb 113>; [3.75%]
  else
    goto <bb 114>; [96.25%]

  <bb 113> [0.71%]:
  _331 = OutMutex;
  pthread_mutex_unlock (_331);
  goto <bb 125>; [100.00%]

  <bb 114> [18.33%]:
  _332 = LastGoodBlock;
  if (_332 != -1)
    goto <bb 115>; [79.76%]
  else
    goto <bb 117>; [20.24%]

  <bb 115> [14.62%]:
  _333 = MEM[(struct outBuff *)_114].blockNumber;
  if (_332 < _333)
    goto <bb 116>; [3.75%]
  else
    goto <bb 117>; [96.25%]

  <bb 116> [0.55%]:
  _334 = OutMutex;
  pthread_mutex_unlock (_334);
  goto <bb 125>; [100.00%]

  <bb 117> [17.78%]:
  _335 = OutMutex;
  _336 = notTooMuchNumBuffered;
  _347 = pthread_cond_wait (_336, _335);
  if (_347 != 0)
    goto <bb 118>; [0.04%]
  else
    goto <bb 106>; [99.96%]

  <bb 118> [0.01%]:
  _348 = stderr;
  __fprintf_chk (_348, 1, "pthread_cond_wait error [%d]! Aborting immediately!\n", _347);
  pthread_mutex_unlock (_335);
  cleanupAndQuit (-8);

  <bb 119> [1.51%]:
  prevOutBlockInSequence_116->next = _114;
  _337 = _327 + 1;
  NumBufferedTailBlocks = _337;
  _338 = prevOutBlockInSequence_116->buf;
  if (_338 == 0B)
    goto <bb 120>; [17.43%]
  else
    goto <bb 122>; [82.57%]

  <bb 120> [0.26%]:
  ret_349 = pthread_cond_signal (&OutBufferHeadNotEmpty);
  if (ret_349 != 0)
    goto <bb 121>; [0.04%]
  else
    goto <bb 122>; [99.96%]

  <bb 121> [0.00%]:
  _350 = stderr;
  __fprintf_chk (_350, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_349);
  cleanupAndQuit (-7);

  <bb 122> [1.51%]:
  _339 = OutMutex;
  ret_351 = pthread_mutex_unlock (_339);
  if (ret_351 != 0)
    goto <bb 123>; [0.04%]
  else
    goto <bb 125>; [99.96%]

  <bb 123> [0.00%]:
  _352 = stderr;
  __fprintf_chk (_352, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_351);
  cleanupAndQuit (-6);

  <bb 124> [3.30%]:
  _340 = _102 == 0;
  _111 = (int) _340;
  _30 = fileData_493->blockNumber;
  MEM[(struct  &)&D.5237] ={v} {CLOBBER};
  D.5237.buf = DecompressedData_40;
  D.5237.bufSize = len_104;
  D.5237.blockNumber = _30;
  D.5237.sequenceNumber = _111;
  D.5237.inSize = processedIn_103;
  D.5237.isLastInSequence = isLast_105;
  D.5237.next = 0B;
  _109 = outputBufferAdd (&D.5237, "consumer_decompress");
  D.5237 ={v} {CLOBBER};

  <bb 125> [6.07%]:
  # outSequenceNumber_46 = PHI <outSequenceNumber_112(116), _111(124), outSequenceNumber_112(113), outSequenceNumber_112(122)>
  # addret_53 = PHI <0B(116), _109(124), 0B(113), _114(122)>
  if (addret_53 == 0B)
    goto <bb 126>; [3.00%]
  else
    goto <bb 67>; [97.00%]

  <bb 126> [0.18%]:
  BZ2_bzDecompressEnd (&strm);
  goto <bb 144>; [100.00%]

  <bb 127> [0.78%]:
  # DecompressedData_442 = PHI <DecompressedData_54(67), DecompressedData_54(68), DecompressedData_38(65), DecompressedData_38(66)>
  # prevOutBlockInSequence_72 = PHI <prevOutBlockInSequence_243(67), prevOutBlockInSequence_243(68), prevOutBlockInSequence_41(65), prevOutBlockInSequence_41(66)>
  # outSequenceNumber_449 = PHI <outSequenceNumber_129(67), outSequenceNumber_129(68), outSequenceNumber_44(65), outSequenceNumber_44(66)>
  # processedIn_36 = PHI <processedIn_495(67), processedIn_495(68), processedIn_48(65), processedIn_48(66)>
  # bzret_456 = PHI <bzret_381(67), bzret_381(68), bzret_51(65), bzret_51(66)>
  if (bzret_456 == 4)
    goto <bb 128>; [34.00%]
  else
    goto <bb 130>; [66.00%]

  <bb 128> [0.26%]:
  _353 = strm.avail_in;
  if (_353 != 0)
    goto <bb 131>; [29.56%]
  else
    goto <bb 129>; [70.44%]

  <bb 129> [0.19%]:
  _354 = MEM[(const struct outBuff *)fileData_493].isLastInSequence;
  if (_354 != 0)
    goto <bb 130>; [54.00%]
  else
    goto <bb 131>; [46.00%]

  <bb 130> [0.61%]:
  _355 = bzret_456 != 4;
  _356 = bzret_456 != 0;
  _357 = _355 & _356;
  if (_357 != 0)
    goto <bb 132>; [29.56%]
  else
    goto <bb 133>; [70.44%]

  <bb 131> [0.16%]:
  _358 = decompressErrCheckSingle (bzret_456, fileData_493, outSequenceNumber_449, &strm, "*ERROR during BZ2_bzDecompress - trailing garbage", 1);
  goto <bb 138>; [100.00%]

  <bb 132> [0.18%]:
  _359 = decompressErrCheckSingle (bzret_456, fileData_493, outSequenceNumber_449, &strm, "*ERROR during BZ2_bzDecompress - failure exit code", 0);
  goto <bb 138>; [100.00%]

  <bb 133> [0.43%]:
  _360 = strm.avail_in;
  if (_360 != 0)
    goto <bb 134>; [29.56%]
  else
    goto <bb 135>; [70.44%]

  <bb 134> [0.13%]:
  _361 = decompressErrCheckSingle (bzret_456, fileData_493, outSequenceNumber_449, &strm, "*ERROR unconsumed in after BZ2_bzDecompress loop", 0);
  goto <bb 138>; [100.00%]

  <bb 135> [0.30%]:
  if (bzret_456 != 4)
    goto <bb 136>; [69.50%]
  else
    goto <bb 138>; [30.50%]

  <bb 136> [0.21%]:
  _362 = MEM[(const struct outBuff *)fileData_493].isLastInSequence;
  if (_362 != 0)
    goto <bb 137>; [36.64%]
  else
    goto <bb 138>; [63.36%]

  <bb 137> [0.08%]:
  _363 = decompressErrCheckSingle (bzret_456, fileData_493, outSequenceNumber_449, &strm, "*ERROR on decompress - last in segment reached before BZ_STREAM_END", 0);

  <bb 138> [0.78%]:
  # _364 = PHI <_358(131), _359(132), _361(134), 0(135), 0(136), _363(137)>
  if (bzret_456 == 4)
    goto <bb 139>; [20.24%]
  else
    goto <bb 141>; [79.76%]

  <bb 139> [0.16%]:
  _123 = BZ2_bzDecompressEnd (&strm);
  _32 = _123 != 0;
  _33 = (unsigned int) _364;
  _34 = _33 <= 1;
  _35 = _32 & _34;
  if (_35 != 0)
    goto <bb 140>; [2.00%]
  else
    goto <bb 141>; [98.00%]

  <bb 140> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR during BZ2_bzDecompressEnd: %d\n", _123);
  goto <bb 144>; [100.00%]

  <bb 141> [0.77%]:
  # prevOutBlockInSequence_43 = PHI <prevOutBlockInSequence_72(138), 0B(139)>
  # outSequenceNumber_47 = PHI <outSequenceNumber_449(138), -1(139)>
  _132 = MEM[(char * &)fileData_493];
  if (_132 != 0B)
    goto <bb 142>; [53.47%]
  else
    goto <bb 143>; [46.53%]

  <bb 142> [0.41%]:
  operator delete [] (_132);
  MEM[(char * &)fileData_493] = 0B;

  <bb 143> [0.77%]:
  if (_364 != 0)
    goto <bb 144>; [2.00%]
  else
    goto <bb 3>; [98.00%]

  <bb 144> [7.63%]:
  strm ={v} {CLOBBER};
  return 0B;

}



;; Function _GLOBAL__sub_I__Z20safe_cond_timed_waitP14pthread_cond_tP15pthread_mutex_tiPKc (_GLOBAL__sub_I__Z20safe_cond_timed_waitP14pthread_cond_tP15pthread_mutex_tiPKc, funcdef_no=84, decl_uid=4125, cgraph_uid=34, symbol_order=1959) (executed once)

_GLOBAL__sub_I__Z20safe_cond_timed_waitP14pthread_cond_tP15pthread_mutex_tiPKc ()
{
  <bb 2> [100.00%]:
  MEM[(struct  &)&OutputBuffer] ={v} {CLOBBER};
  MEM[(struct _Vector_impl *)&OutputBuffer]._M_start = 0B;
  MEM[(struct _Vector_impl *)&OutputBuffer]._M_finish = 0B;
  MEM[(struct _Vector_impl *)&OutputBuffer]._M_end_of_storage = 0B;
  __cxa_atexit (__comp_dtor , &OutputBuffer, &__dso_handle);
  return;

}



;; Function main (main, funcdef_no=46, decl_uid=4172, cgraph_uid=43, symbol_order=796) (executed once)

Created a replacement for emptyElement offset: 0, size: 64: emptyElement$buf
Created a replacement for emptyElement offset: 64, size: 32: emptyElement$bufSize
Created a replacement for emptyElement offset: 96, size: 32: emptyElement$blockNumber
Created a replacement for emptyElement offset: 128, size: 32: emptyElement$sequenceNumber
Created a replacement for emptyElement offset: 160, size: 32: emptyElement$inSize
Created a replacement for emptyElement offset: 192, size: 8: emptyElement$24
Created a replacement for emptyElement offset: 256, size: 64: emptyElement$next
Created a replacement for D.5565 offset: 0, size: 64: SR.134
Created a replacement for D.5566 offset: 0, size: 64: SR.135
Created a replacement for __first offset: 0, size: 64: __first$_M_current
Created a replacement for __last offset: 0, size: 64: __last$_M_current
Removing load: *__cur_1714 = emptyElement;
Removing load: *__first_1722 = emptyElement;
Removing load: *__cur_1702 = emptyElement;
Removing load: *__first_1706 = emptyElement;

Symbols to be put in SSA form
{ D.5407 D.6413 D.6414 D.6415 D.6416 D.6417 D.6418 D.6419 D.6420 D.6421 D.6422 D.6423 }
Incremental SSA update started at block: 0
Number of blocks in CFG: 974
Number of blocks to update: 973 (100%)


main (int argc, char * * argv)
{
  struct outBuff * __last$_M_current;
  struct outBuff * __first$_M_current;
  struct outBuff * SR.135;
  struct outBuff * SR.134;
  struct outBuff * emptyElement$next;
  unsigned char emptyElement$24;
  unsigned int emptyElement$inSize;
  int emptyElement$sequenceNumber;
  int emptyElement$blockNumber;
  unsigned int emptyElement$bufSize;
  char * emptyElement$buf;
  int ret;
  int ret;
  int ret;
  int ret;
  char * FileData;
  char * CompressedData;
  off_t CompressedSize;
  off_t bytesLeft;
  off_t inSize;
  unsigned int outSize;
  int percentComplete;
  int hOutfile;
  int currBlock;
  int rret;
  int ret;
  int ret;
  int D.5799;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  int ret;
  size_t bufr;
  int ret;
  int rsize;
  ssize_t D.5735;
  char * buf;
  int ret;
  int ret;
  int ret;
  int ret;
  char * FileData;
  size_t inSize;
  int ret;
  int ret;
  int ret;
  int D.5712;
  int D.5693;
  int D.5691;
  int D.5686;
  struct FILE * stream;
  struct FILE * zStream;
  unsigned char obuf[5000];
  unsigned char unused[5000];
  unsigned char * unusedTmp;
  int bzerr;
  int nread;
  int streamNo;
  int nUnused;
  int i;
  int ret;
  int ret;
  int D.5681;
  int ret;
  CharType bz2header[11];
  CharType bz2ZeroHeader[11];
  int D.5645;
  size_t inBuffCapacity;
  struct BZ2StreamScanner bz2StreamScanner;
  struct outBuff * fileData;
  int D.5621;
  int ret;
  struct outBuff * D.5606;
  struct __normal_iterator __it;
  struct outBuff * D.5604;
  struct __normal_iterator __it;
  struct outBuff * __first;
  struct __normal_iterator __first;
  struct __normal_iterator __last;
  struct outBuff * D.5589;
  struct outBuff * __cur;
  long unsigned int __n;
  struct __normal_iterator D.5566;
  struct __normal_iterator D.5565;
  struct vector __tmp;
  long unsigned int __n;
  struct outBuff * __cur;
  long unsigned int __niter;
  struct outBuff * __first;
  int ret;
  struct outBuff emptyElement;
  bool D.5534;
  int D.5529;
  unsigned char * __p1;
  unsigned char * __p;
  unsigned char * __current;
  struct _Iter_equals_iter __pred;
  struct random_access_iterator_tag D.5524;
  struct _Iter_equals_iter __pred;
  struct random_access_iterator_tag D.5523;
  unsigned char * D.5522;
  unsigned char * __first1;
  struct _Iter_equal_to_iter __predicate;
  unsigned char * searchBufEnd;
  struct _Iter_equal_to_iter D.5497;
  unsigned char * D.5496;
  int D.5482;
  int D.5479;
  int D.5476;
  int D.5473;
  int D.5470;
  int D.5467;
  struct FILE * zStream;
  unsigned char obuf[5000];
  unsigned char unused[5000];
  unsigned char * unusedTmp;
  int bzerr;
  int streamNo;
  int nUnused;
  int i;
  int D.5460;
  struct queue * D.5435;
  int D.5430;
  int D.5427;
  int ret;
  int D.5424;
  int ret;
  int D.5419;
  int ret;
  int D.5414;
  int ncpu;
  int D.5409;
  int ret;
  int ret;
  int ret;
  struct stat statbuf;
  struct stat statbuf;
  int close_ret;
  struct stat statbuf;
  struct string bz2Tail;
  struct string tbz2Tail;
  struct string bz2Tail;
  size_t size;
  int errLevelCurrentFile;
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
  pthread_t output;
  struct allocator D.5398;
  struct string D.5397;
  struct allocator D.5396;
  struct allocator D.5395;
  struct allocator D.5394;
  struct allocator D.5393;
  struct string D.5392;
  char _1;
  int _2;
  int _3;
  int _4;
  int _5;
  int _6;
  int _7;
  void * _8;
  struct FILE * _9;
  int _11;
  long unsigned int _12;
  char * * _13;
  char * _14;
  char _15;
  char _16;
  int _17;
  long unsigned int _18;
  long unsigned int _19;
  char * * _20;
  int _21;
  int _22;
  int _23;
  int _24;
  int _25;
  int _26;
  int _27;
  char * _28;
  int _29;
  int _30;
  char * _31;
  int _32;
  int _33;
  int _34;
  int _35;
  int _36;
  int _37;
  int _38;
  int _39;
  int _40;
  int _41;
  char * _42;
  char * _43;
  char _44;
  char * _45;
  char _46;
  unsigned char _47;
  unsigned char _48;
  char * _49;
  long unsigned int _50;
  char * _51;
  struct FILE * _52;
  struct FILE * _53;
  char * _54;
  char _55;
  unsigned char _56;
  unsigned char _57;
  char * _58;
  long unsigned int _59;
  char * _60;
  unsigned int _61;
  unsigned int _62;
  struct FILE * _63;
  char * _64;
  char _65;
  unsigned char _66;
  unsigned char _67;
  char * _68;
  long unsigned int _69;
  char * _70;
  unsigned int _71;
  unsigned int _72;
  struct FILE * _73;
  char * _74;
  char _75;
  unsigned char _76;
  unsigned char _77;
  char * _78;
  long unsigned int _79;
  char * _80;
  int _81;
  struct FILE * _82;
  struct FILE * _83;
  char * _84;
  int _85;
  long unsigned int _86;
  long unsigned int _87;
  char * * _88;
  int _89;
  int _90;
  unsigned char _91;
  unsigned char _92;
  char _93;
  bool _94;
  bool _95;
  bool _96;
  struct FILE * _97;
  char * _98;
  struct FILE * _99;
  int _100;
  struct FILE * _101;
  int _102;
  int _103;
  struct FILE * _104;
  char * _105;
  struct FILE * _106;
  int _107;
  long unsigned int _108;
  long unsigned int _109;
  char * * _110;
  int _111;
  struct FILE * _112;
  int _113;
  int _114;
  struct FILE * _115;
  char * _116;
  struct FILE * _117;
  int _118;
  long unsigned int _119;
  long unsigned int _120;
  char * * _121;
  int _122;
  int _123;
  int _124;
  struct FILE * _125;
  struct FILE * _126;
  int _127;
  int _128;
  struct FILE * _129;
  char * _130;
  struct FILE * _131;
  int _132;
  long unsigned int _133;
  long unsigned int _134;
  char * * _135;
  int _136;
  int _137;
  int _141;
  struct FILE * _142;
  double _143;
  int _144;
  int _145;
  int _146;
  struct FILE * _147;
  struct FILE * _148;
  int _149;
  int _150;
  int _151;
  struct FILE * _152;
  struct FILE * _153;
  int _154;
  struct FILE * _155;
  struct FILE * _156;
  int _157;
  struct FILE * _158;
  int _159;
  struct FILE * _160;
  int _161;
  int _162;
  struct FILE * _163;
  int _164;
  struct FILE * _165;
  struct FILE * _166;
  int _167;
  struct FILE * _168;
  int _169;
  long unsigned int _170;
  long unsigned int _171;
  char * * _172;
  int _173;
  int _174;
  int _175;
  struct FILE * _176;
  int _177;
  struct FILE * _178;
  struct FILE * _179;
  int _180;
  struct FILE * _181;
  int _182;
  struct FILE * _183;
  int _184;
  int * _185;
  int _186;
  struct FILE * _187;
  struct FILE * _188;
  struct FILE * _189;
  long unsigned int _190;
  long unsigned int _191;
  long unsigned int _192;
  long unsigned int _193;
  struct FILE * _194;
  struct FILE * _195;
  struct FILE * _196;
  struct FILE * _197;
  struct FILE * _198;
  long unsigned int _199;
  unsigned int _200;
  unsigned int _201;
  int _202;
  unsigned int _203;
  unsigned int _204;
  int _205;
  long unsigned int _206;
  unsigned int _207;
  unsigned int _208;
  int _209;
  struct FILE * _210;
  struct FILE * _211;
  unsigned char _212;
  unsigned char _213;
  char _214;
  struct FILE * _215;
  struct FILE * _216;
  long unsigned int _217;
  long unsigned int _218;
  struct FILE * _219;
  struct FILE * _220;
  int _221;
  struct FILE * _222;
  struct FILE * _223;
  unsigned int _224;
  unsigned int _225;
  struct FILE * _226;
  struct FILE * _227;
  struct FILE * _228;
  struct FILE * _229;
  long int _230;
  struct FILE * _231;
  struct FILE * _232;
  char * _233;
  struct FILE * _234;
  struct FILE * _235;
  int _236;
  struct FILE * _237;
  struct FILE * _238;
  int _239;
  struct FILE * _240;
  long int _241;
  int _242;
  long int _243;
  long int _244;
  int _245;
  int _246;
  int _247;
  struct FILE * _248;
  struct FILE * _249;
  int _250;
  struct FILE * _251;
  struct FILE * _252;
  char _253;
  int _254;
  struct FILE * _255;
  int _256;
  long int _257;
  long unsigned int _258;
  struct FILE * _259;
  int _260;
  long int _261;
  int _262;
  long int _263;
  long int _264;
  long int _265;
  long int _266;
  long int _267;
  int _268;
  int _269;
  int _270;
  bool _271;
  bool _272;
  bool _273;
  struct FILE * _274;
  int _275;
  struct FILE * _276;
  struct FILE * _277;
  int _278;
  int _279;
  long unsigned int _280;
  int _281;
  int _282;
  int _283;
  long unsigned int _284;
  long unsigned int _285;
  long unsigned int _286;
  long unsigned int _287;
  int _288;
  long unsigned int _289;
  long unsigned int _290;
  long unsigned int _291;
  long unsigned int _292;
  long unsigned int _293;
  struct FILE * _294;
  int _295;
  struct FILE * _296;
  long unsigned int _297;
  int _298;
  struct FILE * _299;
  int _300;
  int _301;
  pthread_t * _302;
  long unsigned int _303;
  pthread_t * _304;
  long unsigned int _305;
  long unsigned int _306;
  bool _307;
  long unsigned int _308;
  long unsigned int _309;
  bool _310;
  bool _311;
  bool _312;
  int _313;
  struct FILE * _314;
  int _315;
  struct FILE * _316;
  int _317;
  int _318;
  pthread_t * _319;
  long unsigned int _320;
  pthread_t * _321;
  long unsigned int _322;
  long unsigned int _323;
  int _324;
  struct FILE * _325;
  long int _326;
  long unsigned int _327;
  int * _328;
  int _329;
  struct FILE * _330;
  bool _331;
  bool _332;
  int _333;
  int _334;
  pthread_t * _335;
  long unsigned int _336;
  pthread_t * _337;
  long unsigned int _338;
  int * _339;
  int _340;
  struct FILE * _341;
  int _342;
  bool _343;
  bool _344;
  bool _345;
  int _346;
  int _347;
  struct FILE * _348;
  int _349;
  long unsigned int _351;
  struct FILE * _352;
  long unsigned int _353;
  struct FILE * _354;
  long unsigned int _355;
  struct FILE * _356;
  long int _357;
  double _358;
  long int _359;
  double _360;
  double _361;
  long int _362;
  double _363;
  long int _364;
  double _365;
  double _366;
  int _367;
  struct FILE * _368;
  bool _371;
  bool _381;
  bool _412;
  bool _417;
  bool _447;
  int _454;
  int _479;
  const char * _480;
  const char * _481;
  struct ErrorContext * _558;
  sizetype _569;
  long unsigned int _571;
  struct ErrorContext * _683;
  long int _691;
  unsigned char * _695;
  unsigned char * _697;
  bool _705;
  int _706;
  bool _707;
  long int _708;
  char * _711;
  char * _713;
  struct ErrorContext * _716;
  bool _726;
  int _728;
  char * _732;
  char * _734;
  long int _739;
  bool _749;
  int _754;
  int _755;
  long int _758;
  int _761;
  long int _762;
  int _764;
  int _767;
  int _780;
  bool _782;
  struct ErrorContext * _787;
  char _794;
  struct ErrorContext * _801;
  char * _808;
  int _810;
  struct ErrorContext * _812;
  char _820;
  int _822;
  int _825;
  long int _828;
  int _830;
  int _833;
  int _847;
  struct ErrorContext * _849;
  int _856;
  int _859;
  int _878;
  int _879;
  int _880;
  int _881;
  char * _882;
  long unsigned int _883;
  long unsigned int _884;
  long unsigned int _885;
  long unsigned int _886;
  char * _887;
  char * _888;
  char * _889;
  char * _890;
  long unsigned int _891;
  char * _892;
  char * _893;
  char * _894;
  char * _895;
  char * _896;
  char * _897;
  char * _898;
  char * _899;
  char * _900;
  char * _901;
  char * _902;
  char * _903;
  char * _904;
  char * _905;
  char * _906;
  char * _907;
  char * _908;
  char * _910;
  char * _911;
  char * _912;
  struct outBuff * _913;
  char * _916;
  char * _917;
  char * _918;
  char * _919;
  char * _920;
  unsigned char _1003;
  bool _1071;
  bool _1076;
  bool _1096;
  bool _1119;
  bool _1122;
  long int _1520;
  struct FILE * _1524;
  int _1525;
  long unsigned int _1526;
  struct FILE * _1528;
  bool _1529;
  int _1534;
  int _1536;
  int _1538;
  int _1541;
  int _1543;
  int _1547;
  void * _1548;
  struct FILE * _1549;
  void * _1550;
  struct FILE * _1551;
  int _1552;
  void * _1553;
  sizetype _1554;
  long unsigned int _1555;
  void * _1556;
  long int _1557;
  void * _1558;
  void * _1559;
  void * _1560;
  void * _1561;
  void * _1562;
  struct queue * _1563;
  int _1564;
  struct FILE * _1565;
  struct ErrorContext * _1566;
  int _1569;
  struct FILE * _1570;
  int _1571;
  char _1572;
  BZFILE * _1573;
  int _1574;
  int _1575;
  int _1577;
  struct FILE * _1578;
  int _1580;
  int _1581;
  int _1582;
  struct FILE * _1583;
  int _1584;
  struct FILE * _1585;
  int _1586;
  struct FILE * _1587;
  int _1588;
  unsigned char * _1590;
  sizetype _1591;
  unsigned char * _1592;
  unsigned char _1593;
  int _1595;
  struct FILE * _1596;
  int _1597;
  int _1598;
  int _1599;
  struct ErrorContext * _1600;
  struct FILE * _1601;
  struct FILE * _1603;
  int _1604;
  int _1606;
  int * _1607;
  int _1608;
  struct FILE * _1609;
  struct FILE * _1610;
  int _1611;
  int * _1612;
  int _1613;
  struct FILE * _1614;
  struct FILE * _1615;
  int _1616;
  int * _1617;
  int _1618;
  struct FILE * _1619;
  struct FILE * _1620;
  int _1621;
  int * _1622;
  int _1623;
  struct FILE * _1624;
  struct FILE * _1625;
  int _1626;
  int * _1627;
  int _1628;
  struct FILE * _1629;
  struct FILE * _1630;
  int _1631;
  int * _1632;
  int _1633;
  struct FILE * _1634;
  struct FILE * _1635;
  sizetype _1636;
  sizetype _1638;
  unsigned char * _1639;
  char * _1640;
  unsigned char * _1641;
  bool _1642;
  bool _1643;
  bool _1644;
  unsigned char * _1646;
  unsigned char * _1648;
  unsigned char _1651;
  unsigned char _1653;
  unsigned char * _1656;
  int _1657;
  int _1658;
  struct ErrorContext * _1659;
  bool _1660;
  union pthread_mutex_t * _1661;
  struct outBuff * _1662;
  long int _1663;
  struct outBuff * _1664;
  long int _1665;
  long int _1666;
  long int _1667;
  long unsigned int _1668;
  struct FILE * _1669;
  union pthread_mutex_t * _1670;
  union pthread_mutex_t * _1671;
  struct FILE * _1673;
  struct outBuff * _1674;
  long int _1675;
  struct outBuff * _1676;
  long int _1677;
  long int _1678;
  long int _1679;
  long unsigned int _1680;
  struct outBuff * _1683;
  struct outBuff * _1685;
  struct outBuff * _1687;
  struct outBuff * _1688;
  long int _1689;
  long int _1690;
  long int _1691;
  long unsigned int _1692;
  struct outBuff * _1693;
  long int _1694;
  struct outBuff * _1695;
  long int _1696;
  long int _1697;
  long int _1698;
  long unsigned int _1699;
  long unsigned int _1700;
  char _1710;
  long unsigned int _1711;
  struct outBuff * _1712;
  long unsigned int _1717;
  struct outBuff * _1718;
  struct outBuff * _1719;
  bool _1720;
  unsigned char _1721;
  struct FILE * _1727;
  struct outBuff * _1728;
  unsigned int _1729;
  struct outBuff * _1730;
  unsigned int _1731;
  int _1732;
  int _1733;
  union pthread_mutex_t * _1734;
  union pthread_cond_t * _1735;
  union pthread_mutex_t * _1736;
  int _1737;
  struct outBuff * _1738;
  unsigned int _1739;
  long unsigned int _1740;
  struct FILE * _1741;
  struct FILE * _1742;
  union pthread_mutex_t * _1743;
  int _1744;
  union pthread_mutex_t * _1745;
  union pthread_cond_t * _1746;
  int _1747;
  int _1748;
  union pthread_cond_t * _1749;
  union pthread_mutex_t * _1750;
  struct outBuff * _1751;
  union pthread_cond_t * _1752;
  unsigned int _1754;
  long int _1755;
  long int _1756;
  long int _1757;
  int _1758;
  int _1759;
  union pthread_mutex_t * _1760;
  struct outBuff * _1761;
  struct outBuff * _1762;
  unsigned int _1763;
  bool _1764;
  bool _1765;
  union pthread_mutex_t * _1766;
  union pthread_cond_t * _1767;
  union pthread_mutex_t * _1768;
  int _1769;
  bool _1770;
  bool _1771;
  unsigned char * _1772;
  unsigned char * _1773;
  unsigned char _1776;
  unsigned char _1777;
  long unsigned int _1778;
  long unsigned int _1779;
  void * _1781;
  unsigned int _1782;
  unsigned int _1783;
  int _1785;
  struct FILE * _1786;
  struct FILE * _1788;
  int _1789;
  struct FILE * _1790;
  int _1793;
  struct FILE * _1794;
  int _1795;
  struct FILE * _1796;
  int _1799;
  struct FILE * _1800;
  struct FILE * _1802;
  struct FILE * _1803;
  struct FILE * _1804;
  int _1805;
  char _1806;
  BZFILE * _1807;
  int _1808;
  int _1809;
  int _1811;
  struct FILE * _1812;
  struct FILE * _1813;
  int _1815;
  struct FILE * _1817;
  struct FILE * _1818;
  struct FILE * _1819;
  int _1820;
  int _1821;
  int _1822;
  int _1823;
  long unsigned int _1824;
  int _1826;
  int _1827;
  int _1828;
  struct FILE * _1829;
  struct FILE * _1830;
  long unsigned int _1831;
  int _1832;
  int _1833;
  int _1834;
  struct FILE * _1835;
  struct FILE * _1836;
  long unsigned int _1837;
  int _1838;
  int _1839;
  int _1840;
  struct FILE * _1841;
  struct FILE * _1842;
  int _1843;
  struct FILE * _1844;
  struct FILE * _1845;
  int _1846;
  int _1847;
  unsigned char * _1849;
  sizetype _1850;
  unsigned char * _1851;
  unsigned char _1852;
  int _1854;
  int _1855;
  int _1856;
  int _1857;
  struct FILE * _1858;
  struct FILE * _1859;
  bool _1860;
  int _1861;
  struct FILE * _1862;
  unsigned char _1863;
  struct FILE * _1864;
  struct FILE * _1865;
  int _1867;
  int _1869;
  struct ErrorContext * _1870;
  int _1871;
  struct ErrorContext * _1872;
  int _1873;
  struct ErrorContext * _1874;
  union pthread_mutex_t * _1880;
  union pthread_cond_t * _1881;
  union pthread_mutex_t * _1882;
  long unsigned int _1883;
  union pthread_mutex_t * _1884;
  int _1885;
  union pthread_mutex_t * _1886;
  union pthread_cond_t * _1887;
  union pthread_mutex_t * _1889;
  void * _1890;
  int _1891;
  unsigned int _1893;
  union pthread_cond_t * _1894;
  int _1895;
  int _1896;
  long int _1897;
  long unsigned int _1898;
  long unsigned int _1899;
  long int _1900;
  union pthread_mutex_t * _1901;
  int _1902;
  struct FILE * _1904;
  struct FILE * _1906;
  struct FILE * _1908;
  struct FILE * _1910;
  unsigned char _1911;
  long unsigned int _1913;
  bool _1915;
  long int _1919;
  bool _1920;
  long int _1922;
  long int _1924;
  long unsigned int _1925;
  long int _1927;
  long int _1930;
  union pthread_mutex_t * _1931;
  union pthread_mutex_t * _1932;
  struct FILE * _1934;
  struct FILE * _1936;
  struct FILE * _1938;
  struct FILE * _1940;
  struct FILE * _1942;
  struct FILE * _1944;
  int _1945;
  struct FILE * _1946;
  struct FILE * _1948;
  struct FILE * _1950;
  struct FILE * _1952;
  int _1953;
  struct ElementType * _1954;
  struct ElementType * * _1955;
  long int _1956;
  long unsigned int _1957;
  long unsigned int _1958;
  struct ElementType * * _1959;
  long int _1960;
  long int _1961;
  long int _1962;
  long int _1963;
  long int _1964;
  long int _1965;
  long int _1966;
  long int _1967;
  struct FILE * _1969;
  struct FILE * _1971;
  struct FILE * _1973;
  int _1974;
  int _1975;
  int _1978;
  struct FILE * _1980;
  long int _1981;
  sizetype _1983;
  int _1985;
  long int _1986;
  int _1988;
  long int _1989;
  double _1991;
  double _1992;
  double _1993;
  int _1994;
  unsigned int _1995;
  sizetype _1996;
  bool _1998;
  int _1999;
  unsigned int _2000;
  int _2001;
  unsigned int _2002;
  long unsigned int _2003;
  long int _2004;
  int _2006;
  long int _2007;
  int _2012;
  int _2013;
  int _2015;
  struct FILE * _2017;
  struct FILE * _2018;
  int _2019;
  int _2020;
  long unsigned int _2021;
  struct FILE * _2022;
  int _2023;
  struct FILE * _2025;
  struct FILE * _2027;
  union pthread_mutex_t * _2028;
  union pthread_mutex_t * _2029;
  struct FILE * _2031;
  struct FILE * _2033;
  union pthread_mutex_t * _2034;
  union pthread_mutex_t * _2035;
  union pthread_mutex_t * _2036;
  union pthread_mutex_t * _2037;
  union pthread_mutex_t * _2038;
  union pthread_mutex_t * _2039;
  union pthread_cond_t * _2040;
  union pthread_cond_t * _2041;
  union pthread_cond_t * _2042;
  union pthread_cond_t * _2043;
  pthread_t * _2044;
  struct ElementType * * _2045;
  union pthread_cond_t * _2046;
  union pthread_cond_t * _2047;
  bool _2055;
  bool _2056;
  bool _2058;
  bool _2060;
  bool _2061;
  bool _2063;
  int _2091;
  bool _2122;
  int _2150;
  char * _2219;
  char _2220;
  char _2227;
  bool _2248;
  unsigned char _2250;
  char * _2260;
  bool _2328;
  char * _2414;
  bool _2584;
  bool _2589;
  unsigned char _2593;
  char * _2640;
  char _2650;

  <bb 2> [25.51%]:
  bz2Header = "BZh91AY&SY";
  MEM[(struct  &)&outFilename] ={v} {CLOBBER};
  MEM[(struct  &)&outFilename] ={v} {CLOBBER};
  MEM[(struct _Alloc_hider *)&outFilename]._M_p = &outFilename.D.4158._M_local_buf;
  outFilename._M_string_length = 0;
  MEM[(char_type &)&outFilename + 16] = 0;
  stdinFile = "-";
  _558 = getInstance ();

  <bb 3> [25.51%]:
  if (_558 == 0B)
    goto <bb 959>; [46.53%]
  else
    goto <bb 4>; [53.47%]

  <bb 4> [13.64%]:
  gettimeofday (&tvStartTime, &tz);
  if (argc_560(D) <= 1)
    goto <bb 5>; [36.00%]
  else
    goto <bb 6>; [64.00%]

  <bb 5> [4.91%]:
  OutputStdOut = 1;

  <bb 6> [13.64%]:
  # keep_444 = PHI <0(4), 1(5)>
  progName_563 = *argv_562(D);
  _820 = *progName_563;
  if (_820 == 0)
    goto <bb 10>; [15.00%]
  else
    goto <bb 7>; [85.00%]

  <bb 7> [77.28%]:
  # _1572 = PHI <_1(9), _820(6)>
  # progName_1602 = PHI <progName_140(9), progName_563(6)>
  # progNamePos_2667 = PHI <progNamePos_565(9), progName_563(6)>
  if (_1572 == 47)
    goto <bb 8>; [34.00%]
  else
    goto <bb 9>; [66.00%]

  <bb 8> [26.28%]:
  progName_564 = progNamePos_2667 + 1;

  <bb 9> [77.28%]:
  # progName_140 = PHI <progName_1602(7), progName_564(8)>
  progNamePos_565 = progNamePos_2667 + 1;
  _1 = *progNamePos_565;
  if (_1 == 0)
    goto <bb 10>; [15.00%]
  else
    goto <bb 7>; [85.00%]

  <bb 10> [13.64%]:
  # progName_2658 = PHI <progName_140(9), progName_563(6)>
  _732 = __builtin_strstr (progName_2658, "unzip");
  if (_732 != 0B)
    goto <bb 12>; [70.00%]
  else
    goto <bb 11>; [30.00%]

  <bb 11> [4.09%]:
  _734 = __builtin_strstr (progName_2658, "UNZIP");
  if (_734 != 0B)
    goto <bb 12>; [70.00%]
  else
    goto <bb 13>; [30.00%]

  <bb 12> [12.41%]:

  <bb 13> [13.64%]:
  # decompress_409 = PHI <0(11), 1(12)>
  _711 = __builtin_strstr (progName_2658, "zcat");
  if (_711 != 0B)
    goto <bb 15>; [70.00%]
  else
    goto <bb 14>; [30.00%]

  <bb 14> [4.09%]:
  _713 = __builtin_strstr (progName_2658, "ZCAT");
  if (_713 != 0B)
    goto <bb 15>; [70.00%]
  else
    goto <bb 16>; [30.00%]

  <bb 15> [12.41%]:
  OutputStdOut = 1;

  <bb 16> [13.64%]:
  # decompress_410 = PHI <decompress_409(14), 1(15)>
  # keep_445 = PHI <keep_444(14), 1(15)>
  _2 = strcmp (progName_2658, "bzip2");
  if (_2 == 0)
    goto <bb 22>; [50.00%]
  else
    goto <bb 17>; [50.00%]

  <bb 17> [6.82%]:
  _3 = strcmp (progName_2658, "BZIP2");
  if (_3 == 0)
    goto <bb 22>; [50.00%]
  else
    goto <bb 18>; [50.00%]

  <bb 18> [3.41%]:
  _4 = strcmp (progName_2658, "bunzip2");
  if (_4 == 0)
    goto <bb 22>; [50.00%]
  else
    goto <bb 19>; [50.00%]

  <bb 19> [1.70%]:
  _5 = strcmp (progName_2658, "BUNZIP2");
  if (_5 == 0)
    goto <bb 22>; [50.00%]
  else
    goto <bb 20>; [50.00%]

  <bb 20> [0.85%]:
  _6 = strcmp (progName_2658, "bzcat");
  if (_6 == 0)
    goto <bb 22>; [50.00%]
  else
    goto <bb 21>; [50.00%]

  <bb 21> [0.43%]:
  _7 = strcmp (progName_2658, "BZCAT");
  if (_7 == 0)
    goto <bb 22>; [50.00%]
  else
    goto <bb 23>; [50.00%]

  <bb 22> [13.42%]:
  IgnoreTrailingGarbageFlag = 1;

  <bb 23> [13.64%]:
  FileListCount = 0;
  _569 = (sizetype) argc_560(D);
  if (_569 <= 1152921504606846975)
    goto <bb 24>; [99.96%]
  else
    goto <bb 25>; [0.04%]

  <bb 24> [13.63%]:
  _571 = _569 * 8;
  _8 = operator new [] (_571, &nothrow);
  if (_8 == 0B)
    goto <bb 26>; [30.00%]
  else
    goto <bb 27>; [70.00%]

  <bb 25> [0.01%]:
  __cxa_throw_bad_array_new_length ();

  <bb 26> [4.09%]:
  _9 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Not enough memory!  Aborting...\n", 1, 48, _9);
  goto <bb 959>; [100.00%]

  <bb 27> [9.54%]:
  NumBufferedBlocksMax = 0;
  _1520 = sysconf (84);
  ncpu_1521 = (int) _1520;
  ncpu_1127 = MAX_EXPR <1, ncpu_1521>;
  numCPU = ncpu_1127;
  _2150 = 1;
  if (_2150 >= argc_560(D))
    goto <bb 133>; [2.00%]
  else
    goto <bb 28>; [98.00%]

  <bb 28> [85.64%]:
  # useLoadAverage_2672 = PHI <useLoadAverage_374(132), 0(27)>
  # readEntireFile_1426 = PHI <readEntireFile_384(132), 0(27)>
  # blockSize_1361 = PHI <blockSize_397(132), 900000(27)>
  # maxMemory_1290 = PHI <maxMemory_404(132), 100000000(27)>
  # maxMemorySwitch_1272 = PHI <maxMemorySwitch_408(132), 0(27)>
  # decompress_1173 = PHI <decompress_415(132), decompress_410(27)>
  # compress_2713 = PHI <compress_420(132), 0(27)>
  # testFile_2716 = PHI <testFile_425(132), 0(27)>
  # keep_2719 = PHI <keep_450(132), keep_445(27)>
  # force_2722 = PHI <force_457(132), 0(27)>
  # i_2725 = PHI <i_642(132), 1(27)>
  _12 = i_2725 * 8;
  _13 = argv_562(D) + _12;
  _14 = *_13;
  _15 = *_14;
  if (_15 == 45)
    goto <bb 29>; [34.00%]
  else
    goto <bb 131>; [66.00%]

  <bb 29> [29.12%]:
  _16 = MEM[(char *)_14 + 1B];
  if (_16 == 0)
    goto <bb 30>; [33.00%]
  else
    goto <bb 31>; [67.00%]

  <bb 30> [9.61%]:
  _17 = FileListCount;
  _18 = (long unsigned int) _17;
  _19 = _18 * 8;
  _20 = _8 + _19;
  *_20 = _14;
  _21 = _17 + 1;
  FileListCount = _21;
  goto <bb 132>; [100.00%]

  <bb 31> [19.51%]:
  if (_16 == 45)
    goto <bb 32>; [50.00%]
  else
    goto <bb 63>; [50.00%]

  <bb 32> [9.75%]:
  _22 = strcmp (_14, "--best");
  if (_22 == 0)
    goto <bb 33>; [33.00%]
  else
    goto <bb 34>; [67.00%]

  <bb 33> [3.22%]:
  BWTblockSize = 9;
  goto <bb 132>; [100.00%]

  <bb 34> [6.54%]:
  _23 = strcmp (_14, "--decompress");
  if (_23 == 0)
    goto <bb 132>; [33.00%]
  else
    goto <bb 35>; [67.00%]

  <bb 35> [4.38%]:
  _24 = strcmp (_14, "--compress");
  if (_24 == 0)
    goto <bb 132>; [33.00%]
  else
    goto <bb 36>; [67.00%]

  <bb 36> [2.93%]:
  _25 = strcmp (_14, "--fast");
  if (_25 == 0)
    goto <bb 37>; [33.00%]
  else
    goto <bb 38>; [67.00%]

  <bb 37> [0.97%]:
  BWTblockSize = 1;
  goto <bb 132>; [100.00%]

  <bb 38> [1.97%]:
  _26 = strcmp (_14, "--force");
  if (_26 == 0)
    goto <bb 39>; [33.00%]
  else
    goto <bb 40>; [67.00%]

  <bb 39> [0.65%]:
  ForceOverwrite = 1;
  goto <bb 132>; [100.00%]

  <bb 40> [1.32%]:
  _27 = strcmp (_14, "--help");
  if (_27 == 0)
    goto <bb 41>; [0.04%]
  else
    goto <bb 42>; [99.96%]

  <bb 41> [0.00%]:
  _28 = *argv_562(D);
  usage (_28, "HELP");

  <bb 42> [1.32%]:
  _29 = strcmp (_14, "--keep");
  if (_29 == 0)
    goto <bb 132>; [33.00%]
  else
    goto <bb 43>; [67.00%]

  <bb 43> [0.88%]:
  _30 = strcmp (_14, "--license");
  if (_30 == 0)
    goto <bb 44>; [0.04%]
  else
    goto <bb 45>; [99.96%]

  <bb 44> [0.00%]:
  _31 = *argv_562(D);
  usage (_31, "HELP");

  <bb 45> [0.88%]:
  _32 = strcmp (_14, "--loadavg");
  if (_32 == 0)
    goto <bb 132>; [33.00%]
  else
    goto <bb 46>; [67.00%]

  <bb 46> [0.59%]:
  _33 = strcmp (_14, "--quiet");
  if (_33 == 0)
    goto <bb 47>; [33.00%]
  else
    goto <bb 48>; [67.00%]

  <bb 47> [0.19%]:
  QuietMode = 1;
  goto <bb 132>; [100.00%]

  <bb 48> [0.40%]:
  _34 = strcmp (_14, "--read");
  if (_34 == 0)
    goto <bb 132>; [33.00%]
  else
    goto <bb 49>; [67.00%]

  <bb 49> [0.27%]:
  _35 = strcmp (_14, "--stdout");
  if (_35 == 0)
    goto <bb 50>; [33.00%]
  else
    goto <bb 51>; [67.00%]

  <bb 50> [0.09%]:
  OutputStdOut = 1;
  goto <bb 132>; [100.00%]

  <bb 51> [0.18%]:
  _36 = strcmp (_14, "--test");
  if (_36 == 0)
    goto <bb 132>; [33.00%]
  else
    goto <bb 52>; [67.00%]

  <bb 52> [0.12%]:
  _37 = strcmp (_14, "--verbose");
  if (_37 == 0)
    goto <bb 53>; [33.00%]
  else
    goto <bb 54>; [67.00%]

  <bb 53> [0.04%]:
  QuietMode = 0;
  goto <bb 132>; [100.00%]

  <bb 54> [0.08%]:
  _38 = strcmp (_14, "--version");
  if (_38 == 0)
    goto <bb 55>; [50.00%]
  else
    goto <bb 57>; [50.00%]

  <bb 55> [0.04%]:
  banner ();

  <bb 56> [0.04%]:
  exit (0);

  <bb 57> [0.04%]:
  _39 = strcmp (_14, "--ignore-trailing-garbage");
  if (_39 == 0)
    goto <bb 58>; [50.00%]
  else
    goto <bb 59>; [50.00%]

  <bb 58> [0.02%]:
  IgnoreTrailingGarbageFlag = 1;
  goto <bb 132>; [100.00%]

  <bb 59> [0.02%]:
  _40 = strcmp (_14, "--ignore-trailing-garbage=1");
  if (_40 == 0)
    goto <bb 60>; [50.00%]
  else
    goto <bb 61>; [50.00%]

  <bb 60> [0.01%]:
  IgnoreTrailingGarbageFlag = 1;
  goto <bb 132>; [100.00%]

  <bb 61> [0.01%]:
  _41 = strcmp (_14, "--ignore-trailing-garbage=0");
  if (_41 == 0)
    goto <bb 62>; [50.00%]
  else
    goto <bb 132>; [50.00%]

  <bb 62> [0.00%]:
  IgnoreTrailingGarbageFlag = 0;
  goto <bb 132>; [100.00%]

  <bb 63> [9.80%]:
  # useLoadAverage_10 = PHI <useLoadAverage_2672(31)>
  # readEntireFile_2154 = PHI <readEntireFile_1426(31)>
  # blockSize_1540 = PHI <blockSize_1361(31)>
  # maxMemory_2162 = PHI <maxMemory_1290(31)>
  # maxMemorySwitch_1530 = PHI <maxMemorySwitch_1272(31)>
  # decompress_2164 = PHI <decompress_1173(31)>
  # compress_670 = PHI <compress_2713(31)>
  # testFile_2166 = PHI <testFile_2716(31)>
  # keep_672 = PHI <keep_2719(31)>
  # force_2211 = PHI <force_2722(31)>
  # j_677 = PHI <1(31)>
  _2219 = *_13;
  _1640 = _2219 + j_677;
  _2220 = *_1640;
  if (_2220 == 0)
    goto <bb 132>; [2.00%]
  else
    goto <bb 64>; [98.00%]

  <bb 64> [98.00%]:
  # _2640 = PHI <_42(130), _2219(63)>
  # _2650 = PHI <_44(130), _2220(63)>
  # useLoadAverage_2683 = PHI <useLoadAverage_373(130), useLoadAverage_10(63)>
  # readEntireFile_1372 = PHI <readEntireFile_383(130), readEntireFile_2154(63)>
  # blockSize_1469 = PHI <blockSize_396(130), blockSize_1540(63)>
  # maxMemory_1283 = PHI <maxMemory_403(130), maxMemory_2162(63)>
  # maxMemorySwitch_1268 = PHI <maxMemorySwitch_407(130), maxMemorySwitch_1530(63)>
  # decompress_2712 = PHI <decompress_414(130), decompress_2164(63)>
  # compress_2715 = PHI <compress_419(130), compress_670(63)>
  # testFile_2718 = PHI <testFile_424(130), testFile_2166(63)>
  # keep_2721 = PHI <keep_449(130), keep_672(63)>
  # force_2724 = PHI <force_456(130), force_2211(63)>
  # j_2726 = PHI <j_626(130), j_677(63)>
  switch (_2650) <default: <L26> [3.85%], case 49: <L17> [3.85%], case 50: <L18> [3.85%], case 51: <L19> [3.85%], case 52: <L20> [3.85%], case 53: <L21> [3.85%], case 54: <L22> [3.85%], case 55: <L23> [3.85%], case 56: <L24> [3.85%], case 57: <L25> [3.85%], case 76: <L10> [3.85%], case 83: <L3> [3.85%], case 86: <L15> [3.85%], case 98: <L1> [3.85%], case 99: <L5> [3.85%], case 100: <L4> [3.85%], case 102: <L6> [3.85%], case 104: <L7> [0.04%], case 107: <L8> [3.85%], case 108: <L9> [3.85%], case 109: <L2> [3.85%], case 112: <L0> [3.85%], case 113: <L11> [3.85%], case 114: <L12> [3.85%], case 116: <L13> [3.85%], case 118: <L14> [3.85%], case 122: <L16> [3.85%]>

<L0> [3.78%]:
  k_580 = j_2726 + 1;
  __builtin_memcpy (&cmdLineTemp, "2", 2);
  _808 = _2640 + k_580;
  _1806 = *_808;
  _2584 = k_580 > 2047;
  _1860 = _1806 == 0;
  _2589 = _2584 | _1860;
  if (_2589 != 0)
    goto <bb 69>; [9.75%]
  else
    goto <bb 66>; [90.25%]

  <bb 66> [3.41%]:
  _1863 = (unsigned char) _1806;
  _2593 = _1863 + 208;
  if (_2593 > 9)
    goto <bb 69>; [5.00%]
  else
    goto <bb 68>; [95.00%]

  <bb 67> [20.46%]:
  _47 = (unsigned char) _46;
  _48 = _47 + 208;
  if (_48 > 9)
    goto <bb 69>; [5.00%]
  else
    goto <bb 68>; [95.00%]

  <bb 68> [22.68%]:
  # cmdLineTempCount_844 = PHI <cmdLineTempCount_582(67), 0(66)>
  # k_2727 = PHI <k_581(67), k_580(66)>
  k_581 = k_2727 + 1;
  cmdLineTempCount_582 = cmdLineTempCount_844 + 1;
  _45 = _2640 + k_581;
  _46 = *_45;
  _1076 = k_581 > 2047;
  _1920 = _46 == 0;
  _447 = _1076 | _1920;
  if (_447 != 0)
    goto <bb 69>; [9.75%]
  else
    goto <bb 67>; [90.25%]

  <bb 69> [3.77%]:
  # cmdLineTempCount_350 = PHI <cmdLineTempCount_582(67), cmdLineTempCount_582(68), 0(65), 0(66)>
  if (cmdLineTempCount_350 == 0)
    goto <bb 70>; [0.04%]
  else
    goto <bb 71>; [99.96%]

  <bb 70> [0.00%]:
  _49 = *argv_562(D);
  usage (_49, "Cannot parse -p argument");

  <bb 71> [3.77%]:
  _50 = (long unsigned int) cmdLineTempCount_350;
  _51 = _2640 + k_580;
  __builtin___strncpy_chk (&cmdLineTemp, _51, _50, 2048);
  cmdLineTemp[cmdLineTempCount_350] = 0;
  _828 = strtol (&cmdLineTemp, 0B, 10);
  _780 = (int) _828;
  numCPU = _780;
  if (_780 > 4096)
    goto <bb 72>; [2.00%]
  else
    goto <bb 73>; [98.00%]

  <bb 72> [0.08%]:
  _52 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Maximal number of supported processors is 4096!  Aborting...\n", 1, 77, _52);
  goto <bb 959>; [100.00%]

  <bb 73> [3.70%]:
  if (_780 <= 0)
    goto <bb 74>; [2.00%]
  else
    goto <bb 75>; [98.00%]

  <bb 74> [0.07%]:
  _53 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Minimum number of supported processors is 1!  Aborting...\n", 1, 74, _53);
  goto <bb 959>; [100.00%]

  <bb 75> [3.62%]:
  j_586 = _50 + j_2726;
  goto <bb 130> (<L26>); [100.00%]

<L1> [3.78%]:
  k_597 = j_2726 + 1;
  __builtin_memcpy (&cmdLineTemp, "9", 2);
  _2260 = _2640 + k_597;
  _1710 = *_2260;
  _1998 = k_597 > 2047;
  _2328 = _1710 == 0;
  _782 = _1998 | _2328;
  if (_782 != 0)
    goto <bb 80>; [9.75%]
  else
    goto <bb 77>; [90.25%]

  <bb 77> [3.41%]:
  _1003 = (unsigned char) _1710;
  _1911 = _1003 + 208;
  if (_1911 > 9)
    goto <bb 80>; [5.00%]
  else
    goto <bb 79>; [95.00%]

  <bb 78> [20.46%]:
  _56 = (unsigned char) _55;
  _57 = _56 + 208;
  if (_57 > 9)
    goto <bb 80>; [5.00%]
  else
    goto <bb 79>; [95.00%]

  <bb 79> [22.68%]:
  # cmdLineTempCount_1485 = PHI <cmdLineTempCount_599(78), 0(77)>
  # k_2728 = PHI <k_598(78), k_597(77)>
  k_598 = k_2728 + 1;
  cmdLineTempCount_599 = cmdLineTempCount_1485 + 1;
  _54 = _2640 + k_598;
  _55 = *_54;
  _1529 = k_598 > 2047;
  _2122 = _55 == 0;
  _1915 = _1529 | _2122;
  if (_1915 != 0)
    goto <bb 80>; [9.75%]
  else
    goto <bb 78>; [90.25%]

  <bb 80> [3.77%]:
  # cmdLineTempCount_1486 = PHI <cmdLineTempCount_599(78), cmdLineTempCount_599(79), 0(76), 0(77)>
  if (cmdLineTempCount_1486 == 0)
    goto <bb 81>; [0.04%]
  else
    goto <bb 82>; [99.96%]

  <bb 81> [0.00%]:
  _58 = *argv_562(D);
  usage (_58, "Cannot parse file block size");

  <bb 82> [3.77%]:
  _59 = (long unsigned int) cmdLineTempCount_1486;
  _60 = _2640 + k_597;
  __builtin___strncpy_chk (&cmdLineTemp, _60, _59, 2048);
  cmdLineTemp[cmdLineTempCount_1486] = 0;
  _762 = strtol (&cmdLineTemp, 0B, 10);
  _755 = (int) _762;
  blockSize_602 = _755 * 100000;
  _61 = (unsigned int) blockSize_602;
  _62 = _61 + 4294867296;
  if (_62 > 999900000)
    goto <bb 83>; [2.00%]
  else
    goto <bb 84>; [98.00%]

  <bb 83> [0.08%]:
  _63 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: File block size Min: 100k and Max: 1000000k!  Aborting...\n", 1, 74, _63);
  goto <bb 959>; [100.00%]

  <bb 84> [3.70%]:
  j_603 = _59 + j_2726;
  goto <bb 130> (<L26>); [100.00%]

<L2> [3.78%]:
  k_587 = j_2726 + 1;
  __builtin_memcpy (&cmdLineTemp, "1", 2);
  _2414 = _2640 + k_587;
  _794 = *_2414;
  _1071 = k_587 > 2047;
  _1770 = _794 == 0;
  _1771 = _1770 | _1071;
  if (_1771 != 0)
    goto <bb 89>; [9.75%]
  else
    goto <bb 86>; [90.25%]

  <bb 86> [3.41%]:
  _1776 = (unsigned char) _794;
  _1777 = _1776 + 208;
  if (_1777 > 9)
    goto <bb 89>; [5.00%]
  else
    goto <bb 88>; [95.00%]

  <bb 87> [20.46%]:
  _66 = (unsigned char) _65;
  _67 = _66 + 208;
  if (_67 > 9)
    goto <bb 89>; [5.00%]
  else
    goto <bb 88>; [95.00%]

  <bb 88> [22.68%]:
  # cmdLineTempCount_1482 = PHI <cmdLineTempCount_589(87), 0(86)>
  # k_2729 = PHI <k_588(87), k_587(86)>
  k_588 = k_2729 + 1;
  cmdLineTempCount_589 = cmdLineTempCount_1482 + 1;
  _64 = _2640 + k_588;
  _65 = *_64;
  _417 = k_588 > 2047;
  _412 = _65 == 0;
  _381 = _412 | _417;
  if (_381 != 0)
    goto <bb 89>; [9.75%]
  else
    goto <bb 87>; [90.25%]

  <bb 89> [3.77%]:
  # cmdLineTempCount_1408 = PHI <cmdLineTempCount_589(87), cmdLineTempCount_589(88), 0(85), 0(86)>
  if (cmdLineTempCount_1408 == 0)
    goto <bb 90>; [0.04%]
  else
    goto <bb 91>; [99.96%]

  <bb 90> [0.00%]:
  _68 = *argv_562(D);
  usage (_68, "Cannot parse -m argument");

  <bb 91> [3.77%]:
  _69 = (long unsigned int) cmdLineTempCount_1408;
  _70 = _2640 + k_587;
  __builtin___strncpy_chk (&cmdLineTemp, _70, _69, 2048);
  cmdLineTemp[cmdLineTempCount_1408] = 0;
  _739 = strtol (&cmdLineTemp, 0B, 10);
  _728 = (int) _739;
  maxMemory_592 = _728 * 1000000;
  _71 = (unsigned int) maxMemory_592;
  _72 = _71 + 4293967296;
  if (_72 > 1999000000)
    goto <bb 92>; [2.00%]
  else
    goto <bb 93>; [98.00%]

  <bb 92> [0.08%]:
  _73 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Memory usage size Min: 1MB and Max: 2000MB!  Aborting...\n", 1, 73, _73);
  goto <bb 959>; [100.00%]

  <bb 93> [3.70%]:
  j_593 = _69 + j_2726;
  goto <bb 130> (<L26>); [100.00%]

<L3> [3.78%]:
  k_606 = j_2726 + 1;
  __builtin_memcpy (&cmdLineTemp, "0", 2);
  ChildThreadStackSize = -1;
  _233 = _2640 + k_606;
  _2227 = *_233;
  _749 = k_606 > 2047;
  _2248 = _2227 == 0;
  _1720 = _749 | _2248;
  if (_1720 != 0)
    goto <bb 98>; [9.75%]
  else
    goto <bb 95>; [90.25%]

  <bb 95> [3.41%]:
  _2250 = (unsigned char) _2227;
  _1721 = _2250 + 208;
  if (_1721 > 9)
    goto <bb 98>; [5.00%]
  else
    goto <bb 97>; [95.00%]

  <bb 96> [20.46%]:
  _76 = (unsigned char) _75;
  _77 = _76 + 208;
  if (_77 > 9)
    goto <bb 98>; [5.00%]
  else
    goto <bb 97>; [95.00%]

  <bb 97> [22.68%]:
  # cmdLineTempCount_1416 = PHI <cmdLineTempCount_609(96), 0(95)>
  # k_2730 = PHI <k_608(96), k_606(95)>
  k_608 = k_2730 + 1;
  cmdLineTempCount_609 = cmdLineTempCount_1416 + 1;
  _74 = _2640 + k_608;
  _75 = *_74;
  _371 = k_608 > 2047;
  _2055 = _75 == 0;
  _2056 = _371 | _2055;
  if (_2056 != 0)
    goto <bb 98>; [9.75%]
  else
    goto <bb 96>; [90.25%]

  <bb 98> [3.77%]:
  # cmdLineTempCount_1436 = PHI <cmdLineTempCount_609(96), cmdLineTempCount_609(97), 0(94), 0(95)>
  if (cmdLineTempCount_1436 == 0)
    goto <bb 99>; [0.04%]
  else
    goto <bb 100>; [99.96%]

  <bb 99> [0.00%]:
  _78 = *argv_562(D);
  usage (_78, "Cannot parse -S argument");

  <bb 100> [3.77%]:
  _79 = (long unsigned int) cmdLineTempCount_1436;
  _80 = _2640 + k_606;
  __builtin___strncpy_chk (&cmdLineTemp, _80, _79, 2048);
  cmdLineTemp[cmdLineTempCount_1436] = 0;
  _708 = strtol (&cmdLineTemp, 0B, 10);
  _706 = (int) _708;
  _81 = _706 * 1024;
  ChildThreadStackSize = _81;
  if (_81 < 0)
    goto <bb 101>; [21.69%]
  else
    goto <bb 102>; [78.31%]

  <bb 101> [0.82%]:
  _82 = stderr;
  __fprintf_chk (_82, 1, "pbzip2: *ERROR: Parsing -S: invalid stack size specified [%d]!  Ignoring...\n", _81);
  goto <bb 105>; [100.00%]

  <bb 102> [2.95%]:
  if (_81 <= 16383)
    goto <bb 103>; [33.00%]
  else
    goto <bb 105>; [67.00%]

  <bb 103> [0.97%]:
  _83 = stderr;
  __fprintf_chk (_83, 1, "pbzip2: *WARNING: Stack size %d bytes less than minumum - adjusting to %d bytes.\n", _81, 16384);

  <bb 104> [0.97%]:
  ChildThreadStackSize = 16384;

  <bb 105> [3.77%]:
  j_614 = _79 + j_2726;
  goto <bb 130> (<L26>); [100.00%]

<L4> [3.77%]:
  goto <bb 130> (<L26>); [100.00%]

<L5> [3.77%]:
  OutputStdOut = 1;
  goto <bb 130> (<L26>); [100.00%]

<L6> [3.77%]:
  ForceOverwrite = 1;
  goto <bb 130> (<L26>); [100.00%]

<L7> [0.04%]:
  _84 = *argv_562(D);
  usage (_84, "HELP");

<L8> [3.77%]:
  goto <bb 130> (<L26>); [100.00%]

<L9> [3.77%]:
  goto <bb 130> (<L26>); [100.00%]

<L10> [3.77%]:
  banner ();

  <bb 113> [3.77%]:
  exit (0);

<L11> [3.77%]:
  QuietMode = 1;
  goto <bb 130> (<L26>); [100.00%]

<L12> [3.77%]:
  goto <bb 130> (<L26>); [100.00%]

<L13> [3.77%]:
  goto <bb 130> (<L26>); [100.00%]

<L14> [3.77%]:
  QuietMode = 0;
  goto <bb 130> (<L26>); [100.00%]

<L15> [3.77%]:
  banner ();

  <bb 119> [3.77%]:
  exit (0);

<L16> [3.77%]:
  goto <bb 130> (<L26>); [100.00%]

<L17> [3.77%]:
  BWTblockSize = 1;
  goto <bb 130> (<L26>); [100.00%]

<L18> [3.77%]:
  BWTblockSize = 2;
  goto <bb 130> (<L26>); [100.00%]

<L19> [3.77%]:
  BWTblockSize = 3;
  goto <bb 130> (<L26>); [100.00%]

<L20> [3.77%]:
  BWTblockSize = 4;
  goto <bb 130> (<L26>); [100.00%]

<L21> [3.77%]:
  BWTblockSize = 5;
  goto <bb 130> (<L26>); [100.00%]

<L22> [3.77%]:
  BWTblockSize = 6;
  goto <bb 130> (<L26>); [100.00%]

<L23> [3.77%]:
  BWTblockSize = 7;
  goto <bb 130> (<L26>); [100.00%]

<L24> [3.77%]:
  BWTblockSize = 8;
  goto <bb 130> (<L26>); [100.00%]

<L25> [3.77%]:
  BWTblockSize = 9;

  # useLoadAverage_373 = PHI <useLoadAverage_2683(64), useLoadAverage_2683(75), useLoadAverage_2683(84), useLoadAverage_2683(93), useLoadAverage_2683(105), useLoadAverage_2683(106), useLoadAverage_2683(107), useLoadAverage_2683(108), useLoadAverage_2683(110), 1(111), useLoadAverage_2683(114), useLoadAverage_2683(115), useLoadAverage_2683(116), useLoadAverage_2683(117), useLoadAverage_2683(120), useLoadAverage_2683(121), useLoadAverage_2683(122), useLoadAverage_2683(123), useLoadAverage_2683(124), useLoadAverage_2683(125), useLoadAverage_2683(126), useLoadAverage_2683(127), useLoadAverage_2683(128), useLoadAverage_2683(129)>
  # readEntireFile_383 = PHI <readEntireFile_1372(64), readEntireFile_1372(75), readEntireFile_1372(84), readEntireFile_1372(93), readEntireFile_1372(105), readEntireFile_1372(106), readEntireFile_1372(107), readEntireFile_1372(108), readEntireFile_1372(110), readEntireFile_1372(111), readEntireFile_1372(114), 1(115), readEntireFile_1372(116), readEntireFile_1372(117), readEntireFile_1372(120), readEntireFile_1372(121), readEntireFile_1372(122), readEntireFile_1372(123), readEntireFile_1372(124), readEntireFile_1372(125), readEntireFile_1372(126), readEntireFile_1372(127), readEntireFile_1372(128), readEntireFile_1372(129)>
  # blockSize_396 = PHI <blockSize_1469(64), blockSize_1469(75), blockSize_602(84), blockSize_1469(93), blockSize_1469(105), blockSize_1469(106), blockSize_1469(107), blockSize_1469(108), blockSize_1469(110), blockSize_1469(111), blockSize_1469(114), blockSize_1469(115), blockSize_1469(116), blockSize_1469(117), blockSize_1469(120), blockSize_1469(121), blockSize_1469(122), blockSize_1469(123), blockSize_1469(124), blockSize_1469(125), blockSize_1469(126), blockSize_1469(127), blockSize_1469(128), blockSize_1469(129)>
  # maxMemory_403 = PHI <maxMemory_1283(64), maxMemory_1283(75), maxMemory_1283(84), maxMemory_592(93), maxMemory_1283(105), maxMemory_1283(106), maxMemory_1283(107), maxMemory_1283(108), maxMemory_1283(110), maxMemory_1283(111), maxMemory_1283(114), maxMemory_1283(115), maxMemory_1283(116), maxMemory_1283(117), maxMemory_1283(120), maxMemory_1283(121), maxMemory_1283(122), maxMemory_1283(123), maxMemory_1283(124), maxMemory_1283(125), maxMemory_1283(126), maxMemory_1283(127), maxMemory_1283(128), maxMemory_1283(129)>
  # maxMemorySwitch_407 = PHI <maxMemorySwitch_1268(64), maxMemorySwitch_1268(75), maxMemorySwitch_1268(84), 1(93), maxMemorySwitch_1268(105), maxMemorySwitch_1268(106), maxMemorySwitch_1268(107), maxMemorySwitch_1268(108), maxMemorySwitch_1268(110), maxMemorySwitch_1268(111), maxMemorySwitch_1268(114), maxMemorySwitch_1268(115), maxMemorySwitch_1268(116), maxMemorySwitch_1268(117), maxMemorySwitch_1268(120), maxMemorySwitch_1268(121), maxMemorySwitch_1268(122), maxMemorySwitch_1268(123), maxMemorySwitch_1268(124), maxMemorySwitch_1268(125), maxMemorySwitch_1268(126), maxMemorySwitch_1268(127), maxMemorySwitch_1268(128), maxMemorySwitch_1268(129)>
  # decompress_414 = PHI <decompress_2712(64), decompress_2712(75), decompress_2712(84), decompress_2712(93), decompress_2712(105), 1(106), decompress_2712(107), decompress_2712(108), decompress_2712(110), decompress_2712(111), decompress_2712(114), decompress_2712(115), decompress_2712(116), decompress_2712(117), decompress_2712(120), decompress_2712(121), decompress_2712(122), decompress_2712(123), decompress_2712(124), decompress_2712(125), decompress_2712(126), decompress_2712(127), decompress_2712(128), decompress_2712(129)>
  # compress_419 = PHI <compress_2715(64), compress_2715(75), compress_2715(84), compress_2715(93), compress_2715(105), compress_2715(106), compress_2715(107), compress_2715(108), compress_2715(110), compress_2715(111), compress_2715(114), compress_2715(115), compress_2715(116), compress_2715(117), 1(120), compress_2715(121), compress_2715(122), compress_2715(123), compress_2715(124), compress_2715(125), compress_2715(126), compress_2715(127), compress_2715(128), compress_2715(129)>
  # testFile_424 = PHI <testFile_2718(64), testFile_2718(75), testFile_2718(84), testFile_2718(93), testFile_2718(105), testFile_2718(106), testFile_2718(107), testFile_2718(108), testFile_2718(110), testFile_2718(111), testFile_2718(114), testFile_2718(115), 1(116), testFile_2718(117), testFile_2718(120), testFile_2718(121), testFile_2718(122), testFile_2718(123), testFile_2718(124), testFile_2718(125), testFile_2718(126), testFile_2718(127), testFile_2718(128), testFile_2718(129)>
  # keep_449 = PHI <keep_2721(64), keep_2721(75), keep_2721(84), keep_2721(93), keep_2721(105), keep_2721(106), 1(107), keep_2721(108), 1(110), keep_2721(111), keep_2721(114), keep_2721(115), keep_2721(116), keep_2721(117), keep_2721(120), keep_2721(121), keep_2721(122), keep_2721(123), keep_2721(124), keep_2721(125), keep_2721(126), keep_2721(127), keep_2721(128), keep_2721(129)>
  # force_456 = PHI <force_2724(64), force_2724(75), force_2724(84), force_2724(93), force_2724(105), force_2724(106), force_2724(107), 1(108), force_2724(110), force_2724(111), force_2724(114), force_2724(115), force_2724(116), force_2724(117), force_2724(120), force_2724(121), force_2724(122), force_2724(123), force_2724(124), force_2724(125), force_2724(126), force_2724(127), force_2724(128), force_2724(129)>
  # j_464 = PHI <j_2726(64), j_586(75), j_603(84), j_593(93), j_614(105), j_2726(106), j_2726(107), j_2726(108), j_2726(110), j_2726(111), j_2726(114), j_2726(115), j_2726(116), j_2726(117), j_2726(120), j_2726(121), j_2726(122), j_2726(123), j_2726(124), j_2726(125), j_2726(126), j_2726(127), j_2726(128), j_2726(129)>
<L26> [90.25%]:
  j_626 = j_464 + 1;
  _42 = *_13;
  _43 = _42 + j_626;
  _44 = *_43;
  if (_44 == 0)
    goto <bb 132>; [2.00%]
  else
    goto <bb 64>; [98.00%]

  <bb 131> [56.52%]:
  _85 = FileListCount;
  _86 = (long unsigned int) _85;
  _87 = _86 * 8;
  _88 = _8 + _87;
  *_88 = _14;
  _89 = _85 + 1;
  FileListCount = _89;

  <bb 132> [77.86%]:
  # useLoadAverage_374 = PHI <useLoadAverage_2672(30), useLoadAverage_2672(34), useLoadAverage_373(130), useLoadAverage_2672(131), useLoadAverage_2672(33), useLoadAverage_2672(62), useLoadAverage_2672(61), useLoadAverage_2672(60), useLoadAverage_2672(58), useLoadAverage_2672(53), useLoadAverage_2672(51), useLoadAverage_2672(50), useLoadAverage_2672(48), useLoadAverage_2672(47), 1(45), useLoadAverage_2672(42), useLoadAverage_2672(39), useLoadAverage_2672(37), useLoadAverage_2672(35), useLoadAverage_10(63)>
  # readEntireFile_384 = PHI <readEntireFile_1426(30), readEntireFile_1426(34), readEntireFile_383(130), readEntireFile_1426(131), readEntireFile_1426(33), readEntireFile_1426(62), readEntireFile_1426(61), readEntireFile_1426(60), readEntireFile_1426(58), readEntireFile_1426(53), readEntireFile_1426(51), readEntireFile_1426(50), 1(48), readEntireFile_1426(47), readEntireFile_1426(45), readEntireFile_1426(42), readEntireFile_1426(39), readEntireFile_1426(37), readEntireFile_1426(35), readEntireFile_2154(63)>
  # blockSize_397 = PHI <blockSize_1361(30), blockSize_1361(34), blockSize_396(130), blockSize_1361(131), blockSize_1361(33), blockSize_1361(62), blockSize_1361(61), blockSize_1361(60), blockSize_1361(58), blockSize_1361(53), blockSize_1361(51), blockSize_1361(50), blockSize_1361(48), blockSize_1361(47), blockSize_1361(45), blockSize_1361(42), blockSize_1361(39), blockSize_1361(37), blockSize_1361(35), blockSize_1540(63)>
  # maxMemory_404 = PHI <maxMemory_1290(30), maxMemory_1290(34), maxMemory_403(130), maxMemory_1290(131), maxMemory_1290(33), maxMemory_1290(62), maxMemory_1290(61), maxMemory_1290(60), maxMemory_1290(58), maxMemory_1290(53), maxMemory_1290(51), maxMemory_1290(50), maxMemory_1290(48), maxMemory_1290(47), maxMemory_1290(45), maxMemory_1290(42), maxMemory_1290(39), maxMemory_1290(37), maxMemory_1290(35), maxMemory_2162(63)>
  # maxMemorySwitch_408 = PHI <maxMemorySwitch_1272(30), maxMemorySwitch_1272(34), maxMemorySwitch_407(130), maxMemorySwitch_1272(131), maxMemorySwitch_1272(33), maxMemorySwitch_1272(62), maxMemorySwitch_1272(61), maxMemorySwitch_1272(60), maxMemorySwitch_1272(58), maxMemorySwitch_1272(53), maxMemorySwitch_1272(51), maxMemorySwitch_1272(50), maxMemorySwitch_1272(48), maxMemorySwitch_1272(47), maxMemorySwitch_1272(45), maxMemorySwitch_1272(42), maxMemorySwitch_1272(39), maxMemorySwitch_1272(37), maxMemorySwitch_1272(35), maxMemorySwitch_1530(63)>
  # decompress_415 = PHI <decompress_1173(30), 1(34), decompress_414(130), decompress_1173(131), decompress_1173(33), decompress_1173(62), decompress_1173(61), decompress_1173(60), decompress_1173(58), decompress_1173(53), decompress_1173(51), decompress_1173(50), decompress_1173(48), decompress_1173(47), decompress_1173(45), decompress_1173(42), decompress_1173(39), decompress_1173(37), decompress_1173(35), decompress_2164(63)>
  # compress_420 = PHI <compress_2713(30), compress_2713(34), compress_419(130), compress_2713(131), compress_2713(33), compress_2713(62), compress_2713(61), compress_2713(60), compress_2713(58), compress_2713(53), compress_2713(51), compress_2713(50), compress_2713(48), compress_2713(47), compress_2713(45), compress_2713(42), compress_2713(39), compress_2713(37), 1(35), compress_670(63)>
  # testFile_425 = PHI <testFile_2716(30), testFile_2716(34), testFile_424(130), testFile_2716(131), testFile_2716(33), testFile_2716(62), testFile_2716(61), testFile_2716(60), testFile_2716(58), testFile_2716(53), 1(51), testFile_2716(50), testFile_2716(48), testFile_2716(47), testFile_2716(45), testFile_2716(42), testFile_2716(39), testFile_2716(37), testFile_2716(35), testFile_2166(63)>
  # keep_450 = PHI <keep_2719(30), keep_2719(34), keep_449(130), keep_2719(131), keep_2719(33), keep_2719(62), keep_2719(61), keep_2719(60), keep_2719(58), keep_2719(53), keep_2719(51), 1(50), keep_2719(48), keep_2719(47), keep_2719(45), 1(42), keep_2719(39), keep_2719(37), keep_2719(35), keep_672(63)>
  # force_457 = PHI <force_2722(30), force_2722(34), force_456(130), force_2722(131), force_2722(33), force_2722(62), force_2722(61), force_2722(60), force_2722(58), force_2722(53), force_2722(51), force_2722(50), force_2722(48), force_2722(47), force_2722(45), force_2722(42), 1(39), force_2722(37), force_2722(35), force_2211(63)>
  i_642 = i_2725 + 1;
  _11 = (int) i_642;
  if (_11 >= argc_560(D))
    goto <bb 133>; [2.00%]
  else
    goto <bb 28>; [98.00%]

  <bb 133> [1.75%]:
  # useLoadAverage_2677 = PHI <useLoadAverage_374(132), 0(27)>
  # readEntireFile_1406 = PHI <readEntireFile_384(132), 0(27)>
  # blockSize_1302 = PHI <blockSize_397(132), 900000(27)>
  # maxMemory_1284 = PHI <maxMemory_404(132), 100000000(27)>
  # maxMemorySwitch_1271 = PHI <maxMemorySwitch_408(132), 0(27)>
  # decompress_2711 = PHI <decompress_415(132), decompress_410(27)>
  # compress_2714 = PHI <compress_420(132), 0(27)>
  # testFile_2717 = PHI <testFile_425(132), 0(27)>
  # keep_2720 = PHI <keep_450(132), keep_445(27)>
  # force_2723 = PHI <force_457(132), 0(27)>
  _90 = BWTblockSize;
  _91 = (unsigned char) _90;
  _92 = _91 + 48;
  Bz2HeaderZero[3] = _92;
  _93 = (char) _92;
  bz2Header[3] = _93;
  _94 = compress_2714 == 1;
  _95 = decompress_2711 == 1;
  _96 = _94 & _95;
  if (_96 != 0)
    goto <bb 134>; [33.00%]
  else
    goto <bb 136>; [67.00%]

  <bb 134> [0.58%]:
  _97 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Can\'t compress and uncompress data at same time.  Aborting!\n", 1, 76, _97);

  <bb 135> [0.58%]:
  _98 = *argv_562(D);
  _99 = stderr;
  __fprintf_chk (_99, 1, "pbzip2: For help type: %s -h\n", _98);
  goto <bb 959>; [100.00%]

  <bb 136> [1.17%]:
  _100 = FileListCount;
  if (_100 == 0)
    goto <bb 137>; [50.00%]
  else
    goto <bb 155>; [50.00%]

  <bb 137> [0.59%]:
  if (testFile_2717 == 1)
    goto <bb 138>; [20.24%]
  else
    goto <bb 142>; [79.76%]

  <bb 138> [0.12%]:
  _101 = stdin;
  _102 = fileno (_101);
  _103 = isatty (_102);
  if (_103 != 0)
    goto <bb 139>; [33.00%]
  else
    goto <bb 141>; [67.00%]

  <bb 139> [0.04%]:
  _104 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Won\'t read compressed data from terminal.  Aborting!\n", 1, 69, _104);

  <bb 140> [0.04%]:
  _105 = *argv_562(D);
  _106 = stderr;
  __fprintf_chk (_106, 1, "pbzip2: For help type: %s -h\n", _105);
  goto <bb 959>; [100.00%]

  <bb 141> [0.08%]:
  _107 = FileListCount;
  _108 = (long unsigned int) _107;
  _109 = _108 * 8;
  _110 = _8 + _109;
  *_110 = &stdinFile;
  _111 = _107 + 1;
  FileListCount = _111;
  goto <bb 155>; [100.00%]

  <bb 142> [0.47%]:
  if (decompress_2711 == 1)
    goto <bb 143>; [20.24%]
  else
    goto <bb 147>; [79.76%]

  <bb 143> [0.09%]:
  _112 = stdin;
  _113 = fileno (_112);
  _114 = isatty (_113);
  if (_114 != 0)
    goto <bb 144>; [33.00%]
  else
    goto <bb 146>; [67.00%]

  <bb 144> [0.03%]:
  _115 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Won\'t read compressed data from terminal.  Aborting!\n", 1, 69, _115);

  <bb 145> [0.03%]:
  _116 = *argv_562(D);
  _117 = stderr;
  __fprintf_chk (_117, 1, "pbzip2: For help type: %s -h\n", _116);
  goto <bb 959>; [100.00%]

  <bb 146> [0.06%]:
  OutputStdOut = 1;
  _118 = FileListCount;
  _119 = (long unsigned int) _118;
  _120 = _119 * 8;
  _121 = _8 + _120;
  *_121 = &stdinFile;
  _122 = _118 + 1;
  FileListCount = _122;
  goto <bb 155>; [100.00%]

  <bb 147> [0.37%]:
  _123 = OutputStdOut;
  if (_123 == 0)
    goto <bb 148>; [67.00%]
  else
    goto <bb 151>; [33.00%]

  <bb 148> [0.25%]:
  _124 = QuietMode;
  if (_124 != 1)
    goto <bb 149>; [48.88%]
  else
    goto <bb 150>; [51.12%]

  <bb 149> [0.12%]:
  _125 = stderr;
  __builtin_fwrite ("pbzip2: Assuming input data coming from stdin...\n\n", 1, 50, _125);

  <bb 150> [0.25%]:
  OutputStdOut = 1;

  <bb 151> [0.37%]:
  # keep_451 = PHI <keep_2720(147), 1(150)>
  _126 = stdout;
  _127 = fileno (_126);
  _128 = isatty (_127);
  if (_128 != 0)
    goto <bb 152>; [33.00%]
  else
    goto <bb 154>; [67.00%]

  <bb 152> [0.12%]:
  _129 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Won\'t write compressed data to terminal.  Aborting!\n", 1, 68, _129);

  <bb 153> [0.12%]:
  _130 = *argv_562(D);
  _131 = stderr;
  __fprintf_chk (_131, 1, "pbzip2: For help type: %s -h\n", _130);
  goto <bb 959>; [100.00%]

  <bb 154> [0.25%]:
  _132 = FileListCount;
  _133 = (long unsigned int) _132;
  _134 = _133 * 8;
  _135 = _8 + _134;
  *_135 = &stdinFile;
  _136 = _132 + 1;
  FileListCount = _136;

  <bb 155> [0.98%]:
  # keep_452 = PHI <keep_2720(136), keep_2720(141), 1(146), keep_451(154)>
  _137 = QuietMode;
  if (_137 != 1)
    goto <bb 156>; [48.88%]
  else
    goto <bb 157>; [51.12%]

  <bb 156> [0.48%]:
  banner ();

  <bb 157> [0.98%]:
  if (useLoadAverage_2677 == 1)
    goto <bb 158>; [34.00%]
  else
    goto <bb 165>; [66.00%]

  <bb 158> [0.33%]:
  ret_661 = getloadavg (&loadAvgArray, 3);
  if (ret_661 != 3)
    goto <bb 159>; [66.00%]
  else
    goto <bb 161>; [34.00%]

  <bb 159> [0.22%]:
  _141 = QuietMode;
  if (_141 != 1)
    goto <bb 160>; [48.88%]
  else
    goto <bb 165>; [51.12%]

  <bb 160> [0.11%]:
  _142 = stderr;
  __builtin_fwrite ("pbzip2:  *WARNING: Could not get load average!  Using requested processors...\n", 1, 78, _142);
  goto <bb 165>; [100.00%]

  <bb 161> [0.11%]:
  loadAverage_662 = loadAvgArray[0];
  _143 = loadAverage_662 + 5.0e-1;
  _144 = (int) _143;
  numCPUidle_663 = ncpu_1127 - _144;
  _145 = numCPU;
  if (_145 > numCPUidle_663)
    goto <bb 162>; [50.00%]
  else
    goto <bb 163>; [50.00%]

  <bb 162> [0.06%]:
  numCPU = numCPUidle_663;

  <bb 163> [0.11%]:
  _146 = numCPU;
  if (_146 <= 0)
    goto <bb 164>; [36.00%]
  else
    goto <bb 165>; [64.00%]

  <bb 164> [0.04%]:
  numCPU = 1;

  <bb 165> [0.98%]:
  # loadAverage_369 = PHI <0.0(157), 0.0(159), 0.0(160), loadAverage_662(163), loadAverage_662(164)>
  # useLoadAverage_375 = PHI <useLoadAverage_2677(157), 0(159), 0(160), useLoadAverage_2677(163), useLoadAverage_2677(164)>
  ret_1523 = pthread_attr_init (&ChildThreadAttributes);
  if (ret_1523 < 0)
    goto <bb 166>; [0.04%]
  else
    goto <bb 168>; [99.96%]

  <bb 166> [0.00%]:
  _1524 = stderr;
  __fprintf_chk (_1524, 1, "Can\'t initialize thread atrributes [err=%d]! Aborting...\n", ret_1523);

  <bb 167> [0.00%]:
  exit (-1);

  <bb 168> [0.98%]:
  _1525 = ChildThreadStackSize;
  if (_1525 > 0)
    goto <bb 169>; [50.69%]
  else
    goto <bb 171>; [49.31%]

  <bb 169> [0.50%]:
  _1526 = (long unsigned int) _1525;
  ret_1527 = pthread_attr_setstacksize (&ChildThreadAttributes, _1526);
  if (ret_1527 != 0)
    goto <bb 170>; [36.64%]
  else
    goto <bb 171>; [63.36%]

  <bb 170> [0.18%]:
  _1528 = stderr;
  __fprintf_chk (_1528, 1, "Can\'t set thread stacksize [err=%d]! Countinue with default one.\n", ret_1527);

  <bb 171> [0.98%]:
  sigInFilename = 0B;
  sigOutFilename = 0B;
  ret_1533 = sigemptyset (&SignalMask);
  _1534 = sigaddset (&SignalMask, 2);
  ret_1535 = ret_1533 | _1534;
  _1536 = sigaddset (&SignalMask, 15);
  ret_1537 = ret_1535 | _1536;
  _1538 = sigaddset (&SignalMask, 6);
  ret_1539 = ret_1537 | _1538;
  _1541 = sigaddset (&SignalMask, 10);
  ret_1542 = ret_1539 | _1541;
  _1543 = sigaddset (&SignalMask, 1);
  ret_1544 = ret_1542 | _1543;
  if (ret_1544 == 0)
    goto <bb 172>; [36.64%]
  else
    goto <bb 173>; [63.36%]

  <bb 172> [0.36%]:
  ret_1545 = pthread_sigmask (0, &SignalMask, 0B);

  <bb 173> [0.98%]:
  # ret_1546 = PHI <ret_1544(171), ret_1545(172)>
  if (ret_1546 == 0)
    goto <bb 174>; [36.64%]
  else
    goto <bb 175>; [63.36%]

  <bb 174> [0.36%]:
  ret_1531 = pthread_create (&SignalHandlerThread, &ChildThreadAttributes, signalHandlerProc, 0B);

  <bb 175> [0.98%]:
  # ret_1532 = PHI <ret_1546(173), ret_1531(174)>
  if (ret_1532 != 0)
    goto <bb 176>; [50.00%]
  else
    goto <bb 177>; [50.00%]

  <bb 176> [0.49%]:
  _147 = stderr;
  __fprintf_chk (_147, 1, "pbzip2: *ERROR: Can\'t setup signal handling [%d]. Aborting!\n", ret_1532);
  goto <bb 959>; [100.00%]

  <bb 177> [0.49%]:
  _1547 = pthread_create (&TerminatorThread, &ChildThreadAttributes, terminatorThreadProc, 0B);
  if (_1547 != 0)
    goto <bb 178>; [33.00%]
  else
    goto <bb 179>; [67.00%]

  <bb 178> [0.16%]:
  _148 = stderr;
  __fprintf_chk (_148, 1, "pbzip2: *ERROR: Can\'t setup terminator thread [%d]. Aborting!\n", _1547);
  goto <bb 959>; [100.00%]

  <bb 179> [0.33%]:
  _149 = numCPU;
  if (_149 <= 0)
    goto <bb 180>; [36.00%]
  else
    goto <bb 181>; [64.00%]

  <bb 180> [0.12%]:
  numCPU = 1;

  <bb 181> [0.33%]:
  _150 = QuietMode;
  if (_150 != 1)
    goto <bb 182>; [79.76%]
  else
    goto <bb 199>; [20.24%]

  <bb 182> [0.26%]:
  if (testFile_2717 != 1)
    goto <bb 183>; [66.00%]
  else
    goto <bb 198>; [34.00%]

  <bb 183> [0.17%]:
  _151 = numCPU;
  _152 = stderr;
  __fprintf_chk (_152, 1, "\n         # CPUs: %d\n", _151);

  <bb 184> [0.17%]:
  if (useLoadAverage_375 == 1)
    goto <bb 185>; [20.24%]
  else
    goto <bb 186>; [79.76%]

  <bb 185> [0.03%]:
  _153 = stderr;
  __fprintf_chk (_153, 1, "   Load Average: %.2f\n", loadAverage_369);

  <bb 186> [0.17%]:
  if (decompress_2711 != 1)
    goto <bb 187>; [66.00%]
  else
    goto <bb 191>; [34.00%]

  <bb 187> [0.11%]:
  _154 = BWTblockSize;
  _155 = stderr;
  __fprintf_chk (_155, 1, " BWT Block Size: %d00 KB\n", _154);

  <bb 188> [0.11%]:
  if (blockSize_1302 <= 99999)
    goto <bb 189>; [50.00%]
  else
    goto <bb 190>; [50.00%]

  <bb 189> [0.06%]:
  _156 = stderr;
  __fprintf_chk (_156, 1, "File Block Size: %d bytes\n", blockSize_1302);
  goto <bb 191>; [100.00%]

  <bb 190> [0.06%]:
  _157 = blockSize_1302 / 1000;
  _158 = stderr;
  __fprintf_chk (_158, 1, "File Block Size: %d KB\n", _157);

  <bb 191> [0.17%]:
  _159 = maxMemory_1284 / 1000000;
  _160 = stderr;
  __fprintf_chk (_160, 1, " Maximum Memory: %d MB\n", _159);

  <bb 192> [0.17%]:
  _161 = ChildThreadStackSize;
  if (_161 > 0)
    goto <bb 193>; [46.68%]
  else
    goto <bb 194>; [53.32%]

  <bb 193> [0.08%]:
  _162 = _161 / 1024;
  _163 = stderr;
  __fprintf_chk (_163, 1, "     Stack Size: %d KB\n", _162);

  <bb 194> [0.17%]:
  if (decompress_2711 == 1)
    goto <bb 195>; [34.00%]
  else
    goto <bb 198>; [66.00%]

  <bb 195> [0.06%]:
  _164 = IgnoreTrailingGarbageFlag;
  if (_164 == 1)
    goto <bb 197>; [34.00%]
  else
    goto <bb 196>; [66.00%]

  <bb 196> [0.04%]:

  <bb 197> [0.06%]:
  # _480 = PHI <"on"(195), "off"(196)>
  _165 = stderr;
  __fprintf_chk (_165, 1, " Ignore Trailing Garbage: %s\n", _480);

  <bb 198> [0.26%]:
  _166 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _166);

  <bb 199> [0.33%]:
  _1548 = operator new (40, &nothrow);
  OutMutex = _1548;
  if (_1548 == 0B)
    goto <bb 200>; [26.74%]
  else
    goto <bb 201>; [73.26%]

  <bb 200> [0.09%]:
  _1549 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Could not allocate memory (OutMutex)!  Aborting...\n", 1, 67, _1549);
  goto <bb 204>; [100.00%]

  <bb 201> [0.24%]:
  pthread_mutex_init (_1548, 0B);
  _1550 = operator new (40, &nothrow);
  ProducerDoneMutex = _1550;
  if (_1550 == 0B)
    goto <bb 202>; [26.74%]
  else
    goto <bb 203>; [73.26%]

  <bb 202> [0.06%]:
  _1551 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Could not allocate memory (ProducerDoneMutex)!  Aborting...\n", 1, 76, _1551);
  goto <bb 204>; [100.00%]

  <bb 203> [0.18%]:
  pthread_mutex_init (_1550, 0B);

  <bb 204> [0.33%]:
  # _1552 = PHI <1(200), 1(202), 0(203)>
  if (_1552 != 0)
    goto <bb 959>; [67.00%]
  else
    goto <bb 205>; [33.00%]

  <bb 205> [0.11%]:
  _167 = numCPU;
  _1553 = operator new (96, &nothrow);
  if (_1553 != 0B)
    goto <bb 206>; [70.00%]
  else
    goto <bb 207>; [30.00%]

  <bb 206> [0.08%]:
  MEM[(struct  &)_1553] ={v} {CLOBBER};
  MEM[(struct queue *)_1553].count = 0;
  MEM[(struct queue *)_1553].lastElement = 0B;

  <bb 207> [0.11%]:
  if (_1553 == 0B)
    goto <bb 217>; [4.07%]
  else
    goto <bb 208>; [95.93%]

  <bb 208> [0.10%]:
  _1554 = (sizetype) _167;
  if (_1554 <= 1152921504606846975)
    goto <bb 209>; [99.96%]
  else
    goto <bb 210>; [0.04%]

  <bb 209> [0.10%]:
  _1555 = _1554 * 8;
  _1556 = operator new [] (_1555, &nothrow);
  MEM[(struct queue *)_1553].qData = _1556;
  if (_1556 == 0B)
    goto <bb 217>; [7.92%]
  else
    goto <bb 211>; [92.08%]

  <bb 210> [0.00%]:
  __cxa_throw_bad_array_new_length ();

  <bb 211> [0.10%]:
  _1557 = (long int) _167;
  MEM[(struct queue *)_1553].size = _1557;
  MEM[(struct queue *)_1553].empty = 1;
  MEM[(struct queue *)_1553].full = 0;
  MEM[(struct queue *)_1553].head = 0;
  MEM[(struct queue *)_1553].tail = 0;
  MEM[(struct queue *)_1553].count = 0;
  MEM[(struct queue *)_1553].lastElement = 0B;
  MEM[(struct queue *)_1553].topLevelFull = 0;
  MEM[(struct queue *)_1553].topLevelEmpty = 1;
  MEM[(struct queue *)_1553].mut = 0B;
  _1558 = operator new (40, &nothrow);
  MEM[(struct queue *)_1553].mut = _1558;
  if (_1558 == 0B)
    goto <bb 217>; [7.92%]
  else
    goto <bb 212>; [92.08%]

  <bb 212> [0.09%]:
  pthread_mutex_init (_1558, 0B);
  MEM[(struct queue *)_1553].notFull = 0B;
  _1559 = operator new (48, &nothrow);
  MEM[(struct queue *)_1553].notFull = _1559;
  if (_1559 == 0B)
    goto <bb 217>; [7.92%]
  else
    goto <bb 213>; [92.08%]

  <bb 213> [0.08%]:
  pthread_cond_init (_1559, 0B);
  MEM[(struct queue *)_1553].notEmpty = 0B;
  _1560 = operator new (48, &nothrow);
  MEM[(struct queue *)_1553].notEmpty = _1560;
  if (_1560 == 0B)
    goto <bb 217>; [7.92%]
  else
    goto <bb 214>; [92.08%]

  <bb 214> [0.07%]:
  pthread_cond_init (_1560, 0B);
  MEM[(struct queue *)_1553].consumers = 0B;
  _1561 = operator new [] (_1555, &nothrow);
  MEM[(struct queue *)_1553].consumers = _1561;
  if (_1561 == 0B)
    goto <bb 217>; [7.92%]
  else
    goto <bb 215>; [92.08%]

  <bb 215> [0.07%]:
  notTooMuchNumBuffered = 0B;
  _1562 = operator new (48, &nothrow);
  notTooMuchNumBuffered = _1562;
  if (_1562 == 0B)
    goto <bb 217>; [7.92%]
  else
    goto <bb 216>; [92.08%]

  <bb 216> [0.06%]:
  pthread_cond_init (_1562, 0B);

  <bb 217> [0.11%]:
  # _1563 = PHI <0B(207), 0B(209), 0B(211), 0B(212), 0B(213), 0B(214), 0B(215), _1553(216)>
  FifoQueue = _1563;
  if (_1563 == 0B)
    goto <bb 218>; [17.43%]
  else
    goto <bb 219>; [82.57%]

  <bb 218> [0.02%]:
  _168 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Queue Init failed.  Aborting...\n", 1, 48, _168);
  goto <bb 959>; [100.00%]

  <bb 219> [1.42%]:
  # zeroByteFile_385 = PHI <0(217), zeroByteFile_389(919)>
  # blockSize_398 = PHI <blockSize_1302(217), blockSize_400(919)>
  # errLevel_426 = PHI <0(217), errLevel_435(919)>
  # noThreads_437 = PHI <0(217), noThreads_443(919)>
  # fileLoop_458 = PHI <0(217), fileLoop_845(919)>
  _169 = FileListCount;
  if (_169 <= fileLoop_458)
    goto <bb 920>; [2.00%]
  else
    goto <bb 220>; [98.00%]

  <bb 220> [1.39%]:
  producerDone = 0;
  InFileSize = 0;
  NumBlocks = 0;
  _683 = getInstance ();

  <bb 221> [1.39%]:
  pthread_mutex_lock (&_err_ctx_mutex);
  _683->_last_kernel_err_no = 0;
  _683->_first_kernel_err_no = 0;
  pthread_mutex_unlock (&_err_ctx_mutex);
  _170 = (long unsigned int) fileLoop_458;
  _171 = _170 * 8;
  _172 = _8 + _171;
  InFilename_685 = *_172;
  if (testFile_2717 != 0)
    goto <bb 222>; [67.00%]
  else
    goto <bb 301>; [33.00%]

  <bb 222> [0.93%]:
  _173 = QuietMode;
  if (_173 != 1)
    goto <bb 223>; [66.00%]
  else
    goto <bb 227>; [34.00%]

  <bb 223> [0.61%]:
  _174 = FileListCount;
  _175 = fileLoop_458 + 1;
  _176 = stderr;
  __fprintf_chk (_176, 1, "      File #: %d of %d\n", _175, _174);

  <bb 224> [0.61%]:
  _177 = strcmp (InFilename_685, "-");
  if (_177 != 0)
    goto <bb 225>; [50.00%]
  else
    goto <bb 226>; [50.00%]

  <bb 225> [0.31%]:
  _178 = stderr;
  __fprintf_chk (_178, 1, "     Testing: %s\n", InFilename_685);
  goto <bb 227>; [100.00%]

  <bb 226> [0.31%]:
  _179 = stderr;
  __builtin_fwrite ("     Testing: <stdin>\n", 1, 22, _179);

  <bb 227> [0.93%]:
  nUnused = 0;
  _1564 = strcmp (InFilename_685, "-");
  if (_1564 != 0)
    goto <bb 228>; [33.00%]
  else
    goto <bb 232>; [67.00%]

  <bb 228> [0.31%]:
  _1565 = fopen (InFilename_685, "rb");

  <bb 229> [0.31%]:
  if (_1565 == 0B)
    goto <bb 230>; [0.43%]
  else
    goto <bb 233>; [99.57%]

  <bb 230> [0.00%]:
  _1566 = getInstance ();

  <bb 231> [0.00%]:
  saveError (_1566);
  handle_error (0, -1, "pbzip2: *ERROR: Could not open input file [%s]!  Skipping...\n", InFilename_685);
  goto <bb 295>; [100.00%]

  <bb 232> [0.62%]:
  zStream_1567 = stdin;

  <bb 233> [0.93%]:
  # zStream_1568 = PHI <_1565(229), zStream_1567(232)>
  _1569 = ferror (zStream_1568);
  if (_1569 != 0)
    goto <bb 234>; [33.00%]
  else
    goto <bb 241>; [67.00%]

  <bb 234> [0.31%]:
  handle_error (0, -1, "pbzip2: *ERROR: Problem with stream of file [%s]!  Skipping...\n", InFilename_685);

  <bb 235> [0.31%]:
  _1570 = stdin;
  if (zStream_1568 != _1570)
    goto <bb 236>; [53.47%]
  else
    goto <bb 295>; [46.53%]

  <bb 236> [0.16%]:
  _1606 = fclose (zStream_1568);

  <bb 237> [0.16%]:
  if (_1606 == -1)
    goto <bb 238>; [22.95%]
  else
    goto <bb 295>; [77.05%]

  <bb 238> [0.04%]:
  _1607 = __errno_location ();
  _1608 = *_1607;
  _1609 = stderr;
  syncPrintErrnoMsg (_1609, _1608);

  <bb 239> [0.04%]:
  _1610 = stderr;
  __fprintf_chk (_1610, 1, "pbzip2: *ERROR: Failed to close file [%s]!\n", InFilename_685);
  goto <bb 295>; [100.00%]

  <bb 240> [0.59%]:
  # streamNo_1982 = PHI <streamNo_1579(280), streamNo_1579(277)>

  <bb 241> [1.20%]:
  # streamNo_1576 = PHI <streamNo_1982(240), 0(233)>
  _1571 = nUnused;
  _1573 = BZ2_bzReadOpen (&bzerr, zStream_1568, 0, 0, &unused, _1571);

  <bb 242> [1.20%]:
  if (_1573 == 0B)
    goto <bb 244>; [2.14%]
  else
    goto <bb 243>; [97.86%]

  <bb 243> [1.18%]:
  _1574 = bzerr;
  if (_1574 != 0)
    goto <bb 244>; [2.14%]
  else
    goto <bb 250>; [97.86%]

  <bb 244> [0.05%]:
  _1575 = bzerr;
  _1577 = testBZ2ErrorHandling (_1575, _1573, streamNo_1576);

  <bb 245> [0.05%]:
  _1578 = stdin;
  if (zStream_1568 != _1578)
    goto <bb 246>; [53.47%]
  else
    goto <bb 295>; [46.53%]

  <bb 246> [0.03%]:
  _1611 = fclose (zStream_1568);

  <bb 247> [0.03%]:
  if (_1611 == -1)
    goto <bb 248>; [22.95%]
  else
    goto <bb 295>; [77.05%]

  <bb 248> [0.01%]:
  _1612 = __errno_location ();
  _1613 = *_1612;
  _1614 = stderr;
  syncPrintErrnoMsg (_1614, _1613);

  <bb 249> [0.01%]:
  _1615 = stderr;
  __fprintf_chk (_1615, 1, "pbzip2: *ERROR: Failed to close file [%s]!\n", InFilename_685);
  goto <bb 295>; [100.00%]

  <bb 250> [1.15%]:
  streamNo_1579 = streamNo_1576 + 1;

  <bb 251> [7.98%]:
  _1580 = bzerr;
  if (_1580 != 0)
    goto <bb 260>; [7.50%]
  else
    goto <bb 252>; [92.50%]

  <bb 252> [7.38%]:
  BZ2_bzRead (&bzerr, _1573, &obuf, 5000);

  <bb 253> [7.38%]:
  _1581 = bzerr;
  if (_1581 == -5)
    goto <bb 254>; [7.50%]
  else
    goto <bb 251>; [92.50%]

  <bb 254> [0.55%]:
  _1582 = testBZ2ErrorHandling (_1581, _1573, streamNo_1579);

  <bb 255> [0.55%]:
  _1583 = stdin;
  if (zStream_1568 != _1583)
    goto <bb 256>; [53.47%]
  else
    goto <bb 295>; [46.53%]

  <bb 256> [0.30%]:
  _1616 = fclose (zStream_1568);

  <bb 257> [0.30%]:
  if (_1616 == -1)
    goto <bb 258>; [22.95%]
  else
    goto <bb 295>; [77.05%]

  <bb 258> [0.07%]:
  _1617 = __errno_location ();
  _1618 = *_1617;
  _1619 = stderr;
  syncPrintErrnoMsg (_1619, _1618);

  <bb 259> [0.07%]:
  _1620 = stderr;
  __fprintf_chk (_1620, 1, "pbzip2: *ERROR: Failed to close file [%s]!\n", InFilename_685);
  goto <bb 295>; [100.00%]

  <bb 260> [0.60%]:
  if (_1580 != 4)
    goto <bb 261>; [2.14%]
  else
    goto <bb 267>; [97.86%]

  <bb 261> [0.01%]:
  _1584 = testBZ2ErrorHandling (_1580, _1573, streamNo_1579);

  <bb 262> [0.01%]:
  _1585 = stdin;
  if (zStream_1568 != _1585)
    goto <bb 263>; [53.47%]
  else
    goto <bb 295>; [46.53%]

  <bb 263> [0.01%]:
  _1621 = fclose (zStream_1568);

  <bb 264> [0.01%]:
  if (_1621 == -1)
    goto <bb 265>; [22.95%]
  else
    goto <bb 295>; [77.05%]

  <bb 265> [0.00%]:
  _1622 = __errno_location ();
  _1623 = *_1622;
  _1624 = stderr;
  syncPrintErrnoMsg (_1624, _1623);

  <bb 266> [0.00%]:
  _1625 = stderr;
  __fprintf_chk (_1625, 1, "pbzip2: *ERROR: Failed to close file [%s]!\n", InFilename_685);
  goto <bb 295>; [100.00%]

  <bb 267> [0.59%]:
  BZ2_bzReadGetUnused (&bzerr, _1573, &unusedTmp, &nUnused);

  <bb 268> [0.59%]:
  _1586 = bzerr;
  if (_1586 != 0)
    goto <bb 269>; [0.04%]
  else
    goto <bb 271>; [99.96%]

  <bb 269> [0.00%]:
  _1587 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Unexpected error. Aborting!\n", 1, 44, _1587);

  <bb 270> [0.00%]:
  exit (3);

  <bb 271> [3.90%]:
  # i_1589 = PHI <0(268), i_1594(272)>
  _1588 = nUnused;
  if (_1588 <= i_1589)
    goto <bb 273>; [15.00%]
  else
    goto <bb 272>; [85.00%]

  <bb 272> [3.32%]:
  _1590 = unusedTmp;
  _1591 = (sizetype) i_1589;
  _1592 = _1590 + _1591;
  _1593 = *_1592;
  unused[i_1589] = _1593;
  i_1594 = i_1589 + 1;
  goto <bb 271>; [100.00%]

  <bb 273> [0.59%]:
  BZ2_bzReadClose (&bzerr, _1573);

  <bb 274> [0.59%]:
  _1595 = bzerr;
  if (_1595 != 0)
    goto <bb 275>; [0.04%]
  else
    goto <bb 277>; [99.96%]

  <bb 275> [0.00%]:
  _1596 = stderr;
  __builtin_fwrite ("pbzip2: *ERROR: Unexpected error. Aborting!\n", 1, 44, _1596);

  <bb 276> [0.00%]:
  exit (3);

  <bb 277> [0.59%]:
  _1597 = nUnused;
  if (_1597 == 0)
    goto <bb 278>; [33.00%]
  else
    goto <bb 240>; [67.00%]

  <bb 278> [0.19%]:
  _1598 = fgetc (zStream_1568);

  <bb 279> [0.19%]:
  if (_1598 == -1)
    goto <bb 281>; [2.14%]
  else
    goto <bb 280>; [97.86%]

  <bb 280> [0.19%]:
  ungetc (_1598, zStream_1568);
  goto <bb 240>; [100.00%]

  <bb 281> [0.00%]:
  _1599 = ferror (zStream_1568);
  if (_1599 != 0)
    goto <bb 282>; [50.00%]
  else
    goto <bb 289>; [50.00%]

  <bb 282> [0.00%]:
  _1600 = getInstance ();

  <bb 283> [0.00%]:
  saveError (_1600);
  handle_error (0, -1, "pbzip2: *ERROR: Problem with stream of file [%s]!  Skipping...\n", InFilename_685);

  <bb 284> [0.00%]:
  _1601 = stdin;
  if (zStream_1568 != _1601)
    goto <bb 285>; [53.47%]
  else
    goto <bb 295>; [46.53%]

  <bb 285> [0.00%]:
  _1626 = fclose (zStream_1568);

  <bb 286> [0.00%]:
  if (_1626 == -1)
    goto <bb 287>; [22.95%]
  else
    goto <bb 295>; [77.05%]

  <bb 287> [0.00%]:
  _1627 = __errno_location ();
  _1628 = *_1627;
  _1629 = stderr;
  syncPrintErrnoMsg (_1629, _1628);

  <bb 288> [0.00%]:
  _1630 = stderr;
  __fprintf_chk (_1630, 1, "pbzip2: *ERROR: Failed to close file [%s]!\n", InFilename_685);
  goto <bb 295>; [100.00%]

  <bb 289> [0.00%]:
  _1631 = fclose (zStream_1568);

  <bb 290> [0.00%]:
  if (_1631 == -1)
    goto <bb 291>; [22.95%]
  else
    goto <bb 293>; [77.05%]

  <bb 291> [0.00%]:
  _1632 = __errno_location ();
  _1633 = *_1632;
  _1634 = stderr;
  syncPrintErrnoMsg (_1634, _1633);

  <bb 292> [0.00%]:
  _1635 = stderr;
  __fprintf_chk (_1635, 1, "pbzip2: *ERROR: Failed to close file [%s]!\n", InFilename_685);

  <bb 293> [0.00%]:
  if (_1631 == -1)
    goto <bb 294>; [0.52%]
  else
    goto <bb 295>; [99.48%]

  <bb 294> [0.00%]:
  _1603 = stderr;
  __fprintf_chk (_1603, 1, "pbzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", InFilename_685);

  <bb 295> [0.93%]:
  # _1604 = PHI <-1(231), -1(235), -1(239), _1577(245), _1577(249), _1582(255), _1582(259), _1584(262), _1584(266), -1(284), -1(288), 0(293), -1(294), -1(286), _1584(264), _1582(257), _1577(247), -1(237)>
  obuf ={v} {CLOBBER};
  unused ={v} {CLOBBER};
  unusedTmp ={v} {CLOBBER};
  bzerr ={v} {CLOBBER};
  nUnused ={v} {CLOBBER};
  if (_1604 > 0)
    goto <bb 959>; [2.00%]
  else
    goto <bb 296>; [98.00%]

  <bb 296> [0.91%]:
  if (_1604 == 0)
    goto <bb 297>; [50.00%]
  else
    goto <bb 299>; [50.00%]

  <bb 297> [0.46%]:
  _180 = QuietMode;
  if (_180 != 1)
    goto <bb 298>; [48.88%]
  else
    goto <bb 299>; [51.12%]

  <bb 298> [0.22%]:
  _181 = stderr;
  __builtin_fwrite ("        Test: OK\n", 1, 17, _181);

  <bb 299> [0.91%]:
  # errLevel_427 = PHI <2(296), errLevel_426(297), errLevel_426(298)>
  _182 = QuietMode;
  if (_182 != 1)
    goto <bb 300>; [48.88%]
  else
    goto <bb 919>; [51.12%]

  <bb 300> [0.45%]:
  _183 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _183);
  goto <bb 919>; [100.00%]

  <bb 301> [0.46%]:
  __comp_ctor  (&D.5392, InFilename_685, &D.5393);

  <bb 302> [0.46%]:
  operator= (&outFilename, &D.5392);
  _882 = D.5392._M_dataplus._M_p;
  if (&D.5392.D.4158._M_local_buf != _882)
    goto <bb 303>; [53.47%]
  else
    goto <bb 304>; [46.53%]

  <bb 303> [0.24%]:
  operator delete (_882);

  <bb 304> [0.46%]:
  D.5392 ={v} {CLOBBER};
  D.5393 ={v} {CLOBBER};
  if (decompress_2711 == 1)
    goto <bb 305>; [51.12%]
  else
    goto <bb 353>; [48.88%]

  <bb 305> [0.23%]:
  _184 = strcmp (InFilename_685, "-");
  if (_184 != 0)
    goto <bb 306>; [33.00%]
  else
    goto <bb 353>; [67.00%]

  <bb 306> [0.08%]:
  _878 = __open_alias (InFilename_685, 0);

  <bb 307> [0.08%]:
  if (_878 == -1)
    goto <bb 308>; [20.24%]
  else
    goto <bb 311>; [79.76%]

  <bb 308> [0.02%]:
  _185 = __errno_location ();
  _186 = *_185;
  _187 = stderr;
  printErrnoMsg (_187, _186);

  <bb 309> [0.02%]:
  _188 = stderr;
  __fprintf_chk (_188, 1, "pbzip2: *ERROR: File [%s] NOT found!  Skipping...\n", InFilename_685);

  <bb 310> [0.02%]:
  _189 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _189);
  goto <bb 352>; [100.00%]

  <bb 311> [0.06%]:
  __builtin_memset (&tmpBuff, 0, 50);
  _190 = __builtin_strlen (&bz2Header);
  _191 = _190 + 1;
  _691 = do_read (_878, &tmpBuff, _191);

  <bb 312> [0.06%]:
  size_692 = (size_t) _691;
  do_close (_878);

  <bb 313> [0.06%]:
  if (size_692 == 18446744073709551615)
    goto <bb 315>; [11.11%]
  else
    goto <bb 314>; [88.89%]

  <bb 314> [0.05%]:
  _192 = __builtin_strlen (&bz2Header);
  _193 = _192 + 1;
  if (_193 > size_692)
    goto <bb 315>; [50.00%]
  else
    goto <bb 319>; [50.00%]

  <bb 315> [0.03%]:
  _716 = getInstance ();

  <bb 316> [0.03%]:
  _194 = stderr;
  printErrorMessages (_716, _194);

  <bb 317> [0.03%]:
  _195 = stderr;
  __fprintf_chk (_195, 1, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename_685);

  <bb 318> [0.03%]:
  _196 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _196);
  goto <bb 352>; [100.00%]

  <bb 319> [0.03%]:
  _695 = memstr (&tmpBuff, 4, &bz2Header, 3);
  if (_695 == 0B)
    goto <bb 320>; [17.43%]
  else
    goto <bb 322>; [82.57%]

  <bb 320> [0.00%]:
  _197 = stderr;
  __fprintf_chk (_197, 1, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename_685);

  <bb 321> [0.00%]:
  _198 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _198);
  goto <bb 352>; [100.00%]

  <bb 322> [0.02%]:
  _199 = __builtin_strlen (&bz2Header);
  _200 = (unsigned int) _199;
  _201 = _200 + 4294967292;
  _202 = (int) _201;
  _203 = (unsigned int) _691;
  _204 = _203 + 4294967292;
  _205 = (int) _204;
  _697 = memstr (&MEM[(void *)&tmpBuff + 4B], _205, &MEM[(void *)&bz2Header + 4B], _202);
  if (_697 == 0B)
    goto <bb 323>; [17.43%]
  else
    goto <bb 336>; [82.57%]

  <bb 323> [0.00%]:
  _206 = __builtin_strlen (&bz2Header);
  _207 = (unsigned int) _206;
  _208 = _207 + 4294967292;
  _209 = (int) _208;
  _1636 = (sizetype) _205;
  searchBufEnd_1637 = &MEM[(void *)&tmpBuff + 4B] + _1636;
  _1638 = (sizetype) _209;
  _1639 = &MEM[(void *)&Bz2HeaderZero + 4B] + _1638;
  __predicate = D.5497;
  _1642 = &MEM[(void *)&tmpBuff + 4B] == searchBufEnd_1637;
  _1643 = _1639 == &MEM[(void *)&Bz2HeaderZero + 4B];
  _1644 = _1642 | _1643;
  if (_1644 != 0)
    goto <bb 331>; [46.00%]
  else
    goto <bb 324>; [54.00%]

  <bb 324> [0.00%]:
  if (_1639 == &MEM[(void *)&Bz2HeaderZero + 5B])
    goto <bb 325>; [26.74%]
  else
    goto <bb 326>; [73.26%]

  <bb 325> [0.00%]:
  MEM[(struct _Iter_equals_iter *)&__pred] = &MEM[(void *)&Bz2HeaderZero + 4B];
  _1646 = __find_if (&MEM[(void *)&tmpBuff + 4B], searchBufEnd_1637, __pred, D.5524);
  goto <bb 331>; [100.00%]

  <bb 326> [0.00%]:
  # __first1_1647 = PHI <&MEM[(void *)&tmpBuff + 4B](324), __current_1649(328)>
  MEM[(struct _Iter_equals_iter *)&__pred] = &MEM[(void *)&Bz2HeaderZero + 4B];
  _1648 = __find_if (__first1_1647, searchBufEnd_1637, __pred, D.5523);
  if (searchBufEnd_1637 == _1648)
    goto <bb 331>; [3.75%]
  else
    goto <bb 327>; [96.25%]

  <bb 327> [0.00%]:
  __current_1649 = _1648 + 1;
  if (searchBufEnd_1637 == __current_1649)
    goto <bb 331>; [3.75%]
  else
    goto <bb 328>; [96.25%]

  <bb 328> [0.01%]:
  # __p_1652 = PHI <&MEM[(void *)&Bz2HeaderZero + 5B](327), __p_1654(330)>
  # __current_1650 = PHI <__current_1649(327), __current_1655(330)>
  _1651 = *__current_1650;
  _1653 = *__p_1652;
  if (_1651 == _1653)
    goto <bb 329>; [95.00%]
  else
    goto <bb 326>; [5.00%]

  <bb 329> [0.01%]:
  __p_1654 = __p_1652 + 1;
  if (_1639 == __p_1654)
    goto <bb 331>; [5.00%]
  else
    goto <bb 330>; [95.00%]

  <bb 330> [0.01%]:
  __current_1655 = __current_1650 + 1;
  if (searchBufEnd_1637 == __current_1655)
    goto <bb 331>; [5.00%]
  else
    goto <bb 328>; [95.00%]

  <bb 331> [0.00%]:
  # _1656 = PHI <&MEM[(void *)&tmpBuff + 4B](323), _1646(325), searchBufEnd_1637(326), searchBufEnd_1637(327), _1648(329), searchBufEnd_1637(330)>
  if (searchBufEnd_1637 != _1656)
    goto <bb 333>; [95.93%]
  else
    goto <bb 332>; [4.07%]

  <bb 332> [0.00%]:

  <bb 333> [0.00%]:
  # _1641 = PHI <_1656(331), 0B(332)>
  if (_1641 == 0B)
    goto <bb 334>; [9.42%]
  else
    goto <bb 336>; [90.58%]

  <bb 334> [0.00%]:
  _210 = stderr;
  __fprintf_chk (_210, 1, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename_685);

  <bb 335> [0.00%]:
  _211 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _211);
  goto <bb 352>; [100.00%]

  <bb 336> [0.02%]:
  _212 = tmpBuff[3];
  _213 = _212 + 207;
  if (_213 <= 8)
    goto <bb 337>; [67.00%]
  else
    goto <bb 338>; [33.00%]

  <bb 337> [0.01%]:
  _214 = (char) _212;
  BWTblockSizeChar = _214;
  __comp_ctor  (&bz2Tail, ".bz2", &D.5394);
  goto <bb 340>; [100.00%]

  <bb 338> [0.01%]:
  _215 = stderr;
  __fprintf_chk (_215, 1, "pbzip2: *ERROR: File [%s] is NOT a valid bzip2!  Skipping...\n", InFilename_685);

  <bb 339> [0.01%]:
  _216 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _216);
  goto <bb 352>; [100.00%]

  <bb 340> [0.01%]:
  D.5394 ={v} {CLOBBER};
  __comp_ctor  (&tbz2Tail, ".tbz2", &D.5395);

  <bb 341> [0.01%]:
  D.5395 ={v} {CLOBBER};
  _705 = ends_with_icase (&outFilename, &bz2Tail);
  if (_705 != 0)
    goto <bb 342>; [33.00%]
  else
    goto <bb 343>; [67.00%]

  <bb 342> [0.00%]:
  _884 = outFilename._M_string_length;
  _883 = bz2Tail._M_string_length;
  _217 = _884 - _883;
  resize (&outFilename, _217, 0);
  goto <bb 347>; [100.00%]

  <bb 343> [0.01%]:
  _707 = ends_with_icase (&outFilename, &tbz2Tail);
  if (_707 != 0)
    goto <bb 344>; [50.00%]
  else
    goto <bb 346>; [50.00%]

  <bb 344> [0.00%]:
  _886 = outFilename._M_string_length;
  _885 = tbz2Tail._M_string_length;
  _218 = _886 - _885;
  resize (&outFilename, _218, 0);

  <bb 345> [0.00%]:
  append (&outFilename, ".tar");
  goto <bb 347>; [100.00%]

  <bb 346> [0.00%]:
  append (&outFilename, ".out");

  <bb 347> [0.01%]:
  _888 = tbz2Tail._M_dataplus._M_p;
  if (&tbz2Tail.D.4158._M_local_buf != _888)
    goto <bb 348>; [53.47%]
  else
    goto <bb 349>; [46.53%]

  <bb 348> [0.01%]:
  operator delete (_888);

  <bb 349> [0.01%]:
  MEM[(struct  &)&tbz2Tail] ={v} {CLOBBER};
  _887 = bz2Tail._M_dataplus._M_p;
  if (&bz2Tail.D.4158._M_local_buf != _887)
    goto <bb 350>; [53.47%]
  else
    goto <bb 351>; [46.53%]

  <bb 350> [0.01%]:
  operator delete (_887);

  <bb 351> [0.01%]:
  bz2Tail ={v} {CLOBBER};
  tbz2Tail ={v} {CLOBBER};
  goto <bb 367>; [100.00%]

  <bb 352> [0.06%]:
  bz2Tail ={v} {CLOBBER};
  tbz2Tail ={v} {CLOBBER};
  goto <bb 919>; [100.00%]

  <bb 353> [0.38%]:
  __comp_ctor  (&bz2Tail, ".bz2", &D.5396);

  <bb 354> [0.38%]:
  D.5396 ={v} {CLOBBER};
  __comp_ctor  (&D.5397, InFilename_685, &D.5398);

  <bb 355> [0.38%]:
  _726 = ends_with_icase (&D.5397, &bz2Tail);
  _889 = D.5397._M_dataplus._M_p;
  if (&D.5397.D.4158._M_local_buf != _889)
    goto <bb 356>; [53.47%]
  else
    goto <bb 357>; [46.53%]

  <bb 356> [0.20%]:
  operator delete (_889);

  <bb 357> [0.38%]:
  D.5397 ={v} {CLOBBER};
  D.5398 ={v} {CLOBBER};
  if (_726 != 0)
    goto <bb 358>; [33.00%]
  else
    goto <bb 363>; [67.00%]

  <bb 358> [0.13%]:
  _219 = stderr;
  __fprintf_chk (_219, 1, "pbzip2: *ERROR: Input file [%s] already has a .bz2 extension!  Skipping...\n", InFilename_685);

  <bb 359> [0.13%]:
  _220 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _220);

  <bb 360> [0.13%]:
  _890 = bz2Tail._M_dataplus._M_p;
  if (&bz2Tail.D.4158._M_local_buf != _890)
    goto <bb 361>; [53.47%]
  else
    goto <bb 362>; [46.53%]

  <bb 361> [0.07%]:
  operator delete (_890);

  <bb 362> [0.13%]:
  bz2Tail ={v} {CLOBBER};
  goto <bb 919>; [100.00%]

  <bb 363> [0.26%]:
  _891 = bz2Tail._M_string_length;
  _892 = bz2Tail._M_dataplus._M_p;
  _M_append (&outFilename, _892, _891);

  <bb 364> [0.26%]:
  _893 = bz2Tail._M_dataplus._M_p;
  if (&bz2Tail.D.4158._M_local_buf != _893)
    goto <bb 365>; [53.47%]
  else
    goto <bb 366>; [46.53%]

  <bb 365> [0.14%]:
  operator delete (_893);

  <bb 366> [0.26%]:
  bz2Tail ={v} {CLOBBER};

  <bb 367> [0.27%]:
  safe_mutex_lock (&ErrorHandlerMutex);

  <bb 368> [0.27%]:
  sigInFilename = InFilename_685;
  _894 = outFilename._M_dataplus._M_p;
  sigOutFilename = _894;
  safe_mutex_unlock (&ErrorHandlerMutex);

  <bb 369> [0.27%]:
  _221 = strcmp (InFilename_685, "-");
  if (_221 != 0)
    goto <bb 370>; [33.00%]
  else
    goto <bb 389>; [67.00%]

  <bb 370> [0.09%]:
  _879 = __open_alias (InFilename_685, 0);

  <bb 371> [0.09%]:
  if (_879 == -1)
    goto <bb 372>; [20.24%]
  else
    goto <bb 374>; [79.76%]

  <bb 372> [0.02%]:
  _222 = stderr;
  __fprintf_chk (_222, 1, "pbzip2: *ERROR: File [%s] NOT found!  Skipping...\n", InFilename_685);

  <bb 373> [0.02%]:
  _223 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _223);
  goto <bb 388>; [100.00%]

  <bb 374> [0.07%]:
  __fxstat (1, _879, &statbuf);
  _224 = statbuf.st_mode;
  _225 = _224 & 61440;
  if (_225 == 16384)
    goto <bb 375>; [11.11%]
  else
    goto <bb 377>; [88.89%]

  <bb 375> [0.01%]:
  _226 = stderr;
  __fprintf_chk (_226, 1, "pbzip2: *ERROR: File [%s] is a directory!  Skipping...\n", InFilename_685);

  <bb 376> [0.01%]:
  _227 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _227);
  goto <bb 388>; [100.00%]

  <bb 377> [0.06%]:
  if (_225 != 32768)
    goto <bb 378>; [32.02%]
  else
    goto <bb 380>; [67.98%]

  <bb 378> [0.02%]:
  _228 = stderr;
  __fprintf_chk (_228, 1, "pbzip2: *ERROR: File [%s] is not a regular file!  Skipping...\n", InFilename_685);

  <bb 379> [0.02%]:
  _229 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _229);
  goto <bb 388>; [100.00%]

  <bb 380> [0.04%]:
  _230 = statbuf.st_size;
  InFileSize = _230;
  if (_230 == 0)
    goto <bb 381>; [67.00%]
  else
    goto <bb 384>; [33.00%]

  <bb 381> [0.03%]:
  if (decompress_2711 == 1)
    goto <bb 382>; [11.11%]
  else
    goto <bb 384>; [88.89%]

  <bb 382> [0.00%]:
  _231 = stderr;
  __fprintf_chk (_231, 1, "pbzip2: *ERROR: File is of size 0 [%s]!  Skipping...\n", InFilename_685);

  <bb 383> [0.00%]:
  _232 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _232);
  goto <bb 388>; [100.00%]

  <bb 384> [0.04%]:
  # zeroByteFile_386 = PHI <0(380), 1(381)>
  _1657 = __xstat (1, InFilename_685, &fileMetaData);
  if (_1657 != 0)
    goto <bb 385>; [19.52%]
  else
    goto <bb 387>; [80.48%]

  <bb 385> [0.01%]:
  _234 = stderr;
  __fprintf_chk (_234, 1, "pbzip2: *ERROR: Could not get file meta data from [%s]!  Skipping...\n", InFilename_685);

  <bb 386> [0.01%]:
  _235 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _235);
  goto <bb 388>; [100.00%]

  <bb 387> [0.03%]:
  statbuf ={v} {CLOBBER};
  goto <bb 390>; [100.00%]

  <bb 388> [0.06%]:
  # zeroByteFile_387 = PHI <zeroByteFile_385(373), zeroByteFile_385(376), zeroByteFile_385(379), zeroByteFile_385(383), zeroByteFile_386(386)>
  statbuf ={v} {CLOBBER};
  goto <bb 919>; [100.00%]

  <bb 389> [0.18%]:
  InFileSize = -1;

  <bb 390> [0.21%]:
  # zeroByteFile_388 = PHI <zeroByteFile_386(387), zeroByteFile_385(389)>
  # hInfile_390 = PHI <_879(387), 0(389)>
  _236 = OutputStdOut;
  if (_236 == 0)
    goto <bb 391>; [33.00%]
  else
    goto <bb 401>; [67.00%]

  <bb 391> [0.07%]:
  _895 = outFilename._M_dataplus._M_p;
  _1658 = __open_alias (_895, 0);

  <bb 392> [0.07%]:
  if (_1658 == -1)
    goto <bb 393>; [30.50%]
  else
    goto <bb 395>; [69.50%]

  <bb 393> [0.02%]:
  _1659 = getInstance ();

  <bb 394> [0.02%]:
  saveError (_1659);
  goto <bb 396>; [100.00%]

  <bb 395> [0.05%]:
  close (_1658);

  <bb 396> [0.07%]:
  # _1660 = PHI <0(394), 1(395)>
  if (_1660 != 0)
    goto <bb 397>; [50.00%]
  else
    goto <bb 401>; [50.00%]

  <bb 397> [0.04%]:
  if (force_2723 != 1)
    goto <bb 398>; [48.88%]
  else
    goto <bb 400>; [51.12%]

  <bb 398> [0.02%]:
  _896 = outFilename._M_dataplus._M_p;
  _237 = stderr;
  __fprintf_chk (_237, 1, "pbzip2: *ERROR: Output file [%s] already exists!  Use -f to overwrite...\n", _896);

  <bb 399> [0.02%]:
  _238 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _238);
  goto <bb 919>; [100.00%]

  <bb 400> [0.02%]:
  _897 = outFilename._M_dataplus._M_p;
  remove (_897);

  <bb 401> [0.20%]:
  if (readEntireFile_1406 == 1)
    goto <bb 402>; [34.00%]
  else
    goto <bb 406>; [66.00%]

  <bb 402> [0.07%]:
  if (hInfile_390 == 0)
    goto <bb 403>; [50.00%]
  else
    goto <bb 405>; [50.00%]

  <bb 403> [0.03%]:
  _239 = QuietMode;
  if (_239 != 1)
    goto <bb 404>; [48.88%]
  else
    goto <bb 406>; [51.12%]

  <bb 404> [0.02%]:
  _240 = stderr;
  __builtin_fwrite (" *Warning: Ignoring -r switch since input is stdin.\n", 1, 52, _240);
  goto <bb 406>; [100.00%]

  <bb 405> [0.03%]:
  _241 = InFileSize;
  _242 = numCPU;
  _243 = (long int) _242;
  _244 = _241 / _243;
  blockSize_751 = (int) _244;

  <bb 406> [0.20%]:
  # blockSize_399 = PHI <blockSize_398(401), blockSize_398(403), blockSize_398(404), blockSize_751(405)>
  _245 = QuietMode;
  if (_245 != 1)
    goto <bb 407>; [48.88%]
  else
    goto <bb 418>; [51.12%]

  <bb 407> [0.10%]:
  _246 = FileListCount;
  _247 = fileLoop_458 + 1;
  _248 = stderr;
  __fprintf_chk (_248, 1, "         File #: %d of %d\n", _247, _246);

  <bb 408> [0.10%]:
  if (hInfile_390 != 0)
    goto <bb 410>; [50.00%]
  else
    goto <bb 409>; [50.00%]

  <bb 409> [0.05%]:

  <bb 410> [0.10%]:
  # _481 = PHI <InFilename_685(408), "<stdin>"(409)>
  _249 = stderr;
  __fprintf_chk (_249, 1, "     Input Name: %s\n", _481);

  <bb 411> [0.10%]:
  _250 = OutputStdOut;
  if (_250 == 0)
    goto <bb 412>; [50.00%]
  else
    goto <bb 413>; [50.00%]

  <bb 412> [0.05%]:
  _898 = outFilename._M_dataplus._M_p;
  _251 = stderr;
  __fprintf_chk (_251, 1, "    Output Name: %s\n\n", _898);
  goto <bb 414>; [100.00%]

  <bb 413> [0.05%]:
  _252 = stderr;
  __builtin_fwrite ("    Output Name: <stdout>\n\n", 1, 27, _252);

  <bb 414> [0.10%]:
  if (decompress_2711 == 1)
    goto <bb 415>; [20.24%]
  else
    goto <bb 416>; [79.76%]

  <bb 415> [0.02%]:
  _253 = BWTblockSizeChar;
  _254 = (int) _253;
  _255 = stderr;
  __fprintf_chk (_255, 1, " BWT Block Size: %c00k\n", _254);

  <bb 416> [0.10%]:
  _256 = strcmp (InFilename_685, "-");
  if (_256 != 0)
    goto <bb 417>; [33.00%]
  else
    goto <bb 418>; [67.00%]

  <bb 417> [0.03%]:
  _257 = InFileSize;
  _258 = (long unsigned int) _257;
  _259 = stderr;
  __fprintf_chk (_259, 1, "     Input Size: %lu bytes\n", _258);

  <bb 418> [0.20%]:
  if (decompress_2711 == 1)
    goto <bb 419>; [34.00%]
  else
    goto <bb 424>; [66.00%]

  <bb 419> [0.07%]:
  _260 = numCPU;
  if (_260 == 1)
    goto <bb 421>; [34.00%]
  else
    goto <bb 420>; [66.00%]

  <bb 420> [0.04%]:
  _261 = InFileSize;
  _1122 = _261 <= 999999;
  _2091 = (int) _1122;

  <bb 421> [0.07%]:
  # noThreads_438 = PHI <1(419), _2091(420)>
  if (_260 > 1)
    goto <bb 422>; [64.00%]
  else
    goto <bb 451>; [36.00%]

  <bb 422> [0.04%]:
  _262 = strcmp (InFilename_685, "-");
  if (_262 == 0)
    goto <bb 423>; [50.00%]
  else
    goto <bb 451>; [50.00%]

  <bb 423> [0.02%]:
  goto <bb 451>; [100.00%]

  <bb 424> [0.13%]:
  _263 = InFileSize;
  if (_263 > 0)
    goto <bb 425>; [64.00%]
  else
    goto <bb 427>; [36.00%]

  <bb 425> [0.08%]:
  _264 = (long int) blockSize_399;
  _265 = _263 + _264;
  _266 = _265 + -1;
  _267 = _266 / _264;
  numBlocks_752 = (int) _267;
  if (numBlocks_752 == 1)
    goto <bb 427>; [34.00%]
  else
    goto <bb 426>; [66.00%]

  <bb 426> [0.05%]:
  _268 = numCPU;
  _307 = _268 == 1;
  _454 = (int) _307;

  <bb 427> [0.13%]:
  # numBlocks_392 = PHI <10000(424), numBlocks_752(425), numBlocks_752(426)>
  # noThreads_439 = PHI <noThreads_437(424), 1(425), _454(426)>
  if (zeroByteFile_388 == 1)
    goto <bb 428>; [34.00%]
  else
    goto <bb 451>; [66.00%]

  <bb 428> [0.04%]:
  _269 = OutputStdOut;
  if (_269 == 0)
    goto <bb 429>; [50.00%]
  else
    goto <bb 432>; [50.00%]

  <bb 429> [0.02%]:
  _899 = outFilename._M_dataplus._M_p;
  _754 = safe_open_output (_899);

  <bb 430> [0.02%]:
  if (_754 == -1)
    goto <bb 431>; [2.00%]
  else
    goto <bb 432>; [98.00%]

  <bb 431> [0.00%]:
  _900 = outFilename._M_dataplus._M_p;
  handle_error (1, 1, "pbzip2: *ERROR: Could not create output file [%s]!\n", _900);
  goto <bb 920>; [100.00%]

  <bb 432> [0.04%]:
  # hOutfile_391 = PHI <1(428), _754(430)>
  _758 = do_write (hOutfile_391, &Bz2HeaderZero, 14);

  <bb 433> [0.04%]:
  ret_759 = (int) _758;
  _270 = OutputStdOut;
  if (_270 == 0)
    goto <bb 434>; [33.00%]
  else
    goto <bb 438>; [67.00%]

  <bb 434> [0.01%]:
  _761 = do_close (hOutfile_391);

  <bb 435> [0.01%]:
  _901 = outFilename._M_dataplus._M_p;
  _764 = writeFileMetaData (_901);

  <bb 436> [0.01%]:
  if (_764 != 0)
    goto <bb 437>; [33.00%]
  else
    goto <bb 438>; [67.00%]

  <bb 437> [0.00%]:
  _902 = outFilename._M_dataplus._M_p;
  handle_error (0, -1, "pbzip2: *ERROR: Could not write file meta data to [%s]!\n", _902);

  <bb 438> [0.04%]:
  # close_ret_478 = PHI <0(433), _761(436), _761(437)>
  _271 = ret_759 != 14;
  _272 = close_ret_478 == -1;
  _273 = _271 | _272;
  if (_273 != 0)
    goto <bb 439>; [2.00%]
  else
    goto <bb 441>; [98.00%]

  <bb 439> [0.00%]:
  _903 = outFilename._M_dataplus._M_p;
  handle_error (1, 1, "pbzip2: *ERROR: Could not write to file [%s]! Aborting...\n", _903);

  <bb 440> [0.00%]:
  _274 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _274);
  goto <bb 920>; [100.00%]

  <bb 441> [0.04%]:
  _275 = QuietMode;
  if (_275 != 1)
    goto <bb 442>; [48.88%]
  else
    goto <bb 444>; [51.12%]

  <bb 442> [0.02%]:
  _276 = stderr;
  __fprintf_chk (_276, 1, "    Output Size: %u bytes\n", 14);

  <bb 443> [0.02%]:
  _277 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _277);

  <bb 444> [0.04%]:
  if (keep_452 != 1)
    goto <bb 445>; [79.76%]
  else
    goto <bb 919>; [20.24%]

  <bb 445> [0.03%]:
  _278 = OutputStdOut;
  if (_278 != 0)
    goto <bb 447>; [50.00%]
  else
    goto <bb 446>; [50.00%]

  <bb 446> [0.02%]:
  _904 = outFilename._M_dataplus._M_p;
  _880 = __xstat (1, _904, &statbuf);
  if (_880 == 0)
    goto <bb 447>; [67.00%]
  else
    goto <bb 450>; [33.00%]

  <bb 447> [0.03%]:
  _767 = do_remove (InFilename_685);

  <bb 448> [0.03%]:
  if (_767 == -1)
    goto <bb 449>; [20.24%]
  else
    goto <bb 450>; [79.76%]

  <bb 449> [0.01%]:
  handle_error (0, 1, "Can\'t remove input file [%s]!", InFilename_685);

  <bb 450> [0.03%]:
  statbuf ={v} {CLOBBER};
  goto <bb 919>; [100.00%]

  <bb 451> [0.15%]:
  # numBlocks_393 = PHI <0(421), 0(422), 0(423), numBlocks_392(427)>
  # noThreads_440 = PHI <noThreads_438(421), noThreads_438(422), 0(423), noThreads_439(427)>
  NumBlocksEstimated = numBlocks_393;
  _279 = maxMemory_1284 / blockSize_399;
  _280 = (long unsigned int) _279;
  NumBufferedBlocksMax = _280;
  _281 = numCPU;
  _282 = _281 * 2;
  _283 = _279 - _282;
  if (_283 <= 0)
    goto <bb 452>; [36.00%]
  else
    goto <bb 453>; [64.00%]

  <bb 452> [0.05%]:
  NumBufferedBlocksMax = 1;
  goto <bb 454>; [100.00%]

  <bb 453> [0.10%]:
  _284 = (long unsigned int) _282;
  _285 = _280 - _284;
  NumBufferedBlocksMax = _285;

  <bb 454> [0.15%]:
  _286 = (long unsigned int) _281;
  _287 = NumBufferedBlocksMax;
  if (_286 > _287)
    goto <bb 455>; [67.00%]
  else
    goto <bb 460>; [33.00%]

  <bb 455> [0.10%]:
  if (maxMemorySwitch_1271 == 0)
    goto <bb 456>; [50.00%]
  else
    goto <bb 458>; [50.00%]

  <bb 456> [0.05%]:
  NumBufferedBlocksMax = _286;
  _288 = QuietMode;
  if (_288 != 1)
    goto <bb 457>; [48.88%]
  else
    goto <bb 460>; [51.12%]

  <bb 457> [0.02%]:
  _289 = (long unsigned int) _282;
  _290 = _286 + _289;
  _291 = (long unsigned int) blockSize_399;
  _292 = _290 * _291;
  _293 = _292 / 1000000;
  _294 = stderr;
  __fprintf_chk (_294, 1, "*Warning* Max memory limit increased to %lu MB to support %d CPUs\n", _293, _281);
  goto <bb 460>; [100.00%]

  <bb 458> [0.05%]:
  _295 = QuietMode;
  if (_295 != 1)
    goto <bb 459>; [48.88%]
  else
    goto <bb 460>; [51.12%]

  <bb 459> [0.02%]:
  _296 = stderr;
  __builtin_fwrite ("*Warning* CPU usage and performance may be suboptimal due to max memory limit.\n", 1, 79, _296);

  <bb 460> [0.15%]:
  LastGoodBlock = -1;
  MinErrorBlock = -1;
  _297 = NumBufferedBlocksMax;
  _1661 = OutMutex;
  ret_1672 = pthread_mutex_lock (_1661);
  if (ret_1672 != 0)
    goto <bb 461>; [0.04%]
  else
    goto <bb 463>; [99.96%]

  <bb 461> [0.00%]:
  _1673 = stderr;
  __fprintf_chk (_1673, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1672);

  <bb 462> [0.00%]:
  cleanupAndQuit (-5);

  <bb 463> [0.15%]:
  NextBlockToWrite = 0;
  OutBufferPosToWrite = 0;
  NumBufferedBlocks = 0;
  NumBufferedTailBlocks = 0;
  emptyElement$buf_405 = 0B;
  emptyElement$bufSize_401 = 0;
  emptyElement$blockNumber_394 = 0;
  emptyElement$sequenceNumber_380 = 0;
  emptyElement$inSize_370 = 0;
  emptyElement$24_1077 = 1;
  emptyElement$next_463 = 0B;
  _1674 = OutputBuffer.D.4140._M_impl._M_end_of_storage;
  _1675 = (long int) _1674;
  _1676 = OutputBuffer.D.4140._M_impl._M_start;
  _1677 = (long int) _1676;
  _1678 = _1675 - _1677;
  _1679 = _1678 /[ex] 40;
  _1680 = (long unsigned int) _1679;
  if (_297 > _1680)
    goto <bb 464>; [33.00%]
  else
    goto <bb 476>; [67.00%]

  <bb 464> [0.05%]:
  MEM[(struct  &)&__tmp] ={v} {CLOBBER};
  MEM[(struct _Vector_impl *)&__tmp]._M_start = 0B;
  MEM[(struct _Vector_impl *)&__tmp]._M_finish = 0B;
  MEM[(struct _Vector_impl *)&__tmp]._M_end_of_storage = 0B;
  if (_297 != 0)
    goto <bb 465>; [91.00%]
  else
    goto <bb 468>; [9.00%]

  <bb 465> [0.05%]:
  if (_297 > 461168601842738790)
    goto <bb 466>; [0.04%]
  else
    goto <bb 467>; [99.96%]

  <bb 466> [0.00%]:
  __throw_bad_alloc ();

  <bb 467> [0.05%]:
  _1717 = _297 * 40;
  _1718 = operator new (_1717);

  <bb 468> [0.05%]:
  # _1719 = PHI <0B(464), _1718(467)>
  MEM[(struct _Vector_base *)&__tmp]._M_impl._M_start = _1719;
  MEM[(struct _Vector_base *)&__tmp]._M_impl._M_finish = _1719;
  _1711 = _297 * 40;
  _1712 = _1719 + _1711;
  MEM[(struct _Vector_base *)&__tmp]._M_impl._M_end_of_storage = _1712;

  <bb 469> [0.33%]:
  # __n_1713 = PHI <_297(468), __n_1715(472)>
  # __cur_1714 = PHI <_1719(468), __cur_1716(472)>
  if (__n_1713 == 0)
    goto <bb 473>; [15.00%]
  else
    goto <bb 470>; [85.00%]

  <bb 470> [0.28%]:
  if (__cur_1714 != 0B)
    goto <bb 471>; [70.00%]
  else
    goto <bb 472>; [30.00%]

  <bb 471> [0.20%]:
  MEM[(struct outBuff *)__cur_1714] = emptyElement$buf_405;
  MEM[(struct outBuff *)__cur_1714 + 8B] = emptyElement$bufSize_401;
  MEM[(struct outBuff *)__cur_1714 + 12B] = emptyElement$blockNumber_394;
  MEM[(struct outBuff *)__cur_1714 + 16B] = emptyElement$sequenceNumber_380;
  MEM[(struct outBuff *)__cur_1714 + 20B] = emptyElement$inSize_370;
  MEM[(struct outBuff *)__cur_1714 + 24B] = emptyElement$24_1077;
  MEM[(struct outBuff *)__cur_1714 + 32B] = emptyElement$next_463;

  <bb 472> [0.28%]:
  __n_1715 = __n_1713 + 18446744073709551615;
  __cur_1716 = __cur_1714 + 40;
  goto <bb 469>; [100.00%]

  <bb 473> [0.05%]:
  __tmp.D.4140._M_impl._M_finish = __cur_1714;
  _1683 = MEM[(struct outBuff * &)&OutputBuffer];
  MEM[(struct outBuff * &)&__tmp] = _1683;
  MEM[(struct outBuff * &)&OutputBuffer] = _1719;
  _1685 = MEM[(struct outBuff * &)&OutputBuffer + 8];
  MEM[(struct outBuff * &)&__tmp + 8] = _1685;
  MEM[(struct outBuff * &)&OutputBuffer + 8] = __cur_1714;
  _1687 = MEM[(struct outBuff * &)&OutputBuffer + 16];
  MEM[(struct outBuff * &)&__tmp + 16] = _1687;
  MEM[(struct outBuff * &)&OutputBuffer + 16] = _1712;
  if (_1683 != 0B)
    goto <bb 474>; [57.44%]
  else
    goto <bb 475>; [42.56%]

  <bb 474> [0.03%]:
  operator delete (_1683);

  <bb 475> [0.05%]:
  __tmp ={v} {CLOBBER};
  goto <bb 489>; [100.00%]

  <bb 476> [0.10%]:
  _1688 = OutputBuffer.D.4140._M_impl._M_finish;
  _1689 = (long int) _1688;
  _1690 = _1689 - _1677;
  _1691 = _1690 /[ex] 40;
  _1692 = (long unsigned int) _1691;
  if (_297 > _1692)
    goto <bb 477>; [33.00%]
  else
    goto <bb 486>; [67.00%]

  <bb 477> [0.03%]:
  SR.135_395 = _1688;
  SR.134_382 = _1676;
  __first$_M_current_372 = SR.134_382;
  __last$_M_current_468 = SR.135_395;
  MEM[(struct __normal_iterator *)&__it] = __last$_M_current_468;
  MEM[(struct __normal_iterator *)&__it] = __first$_M_current_372;

  <bb 478> [0.22%]:
  # __first_1722 = PHI <_1676(477), __first_1723(479)>
  if (_1688 == __first_1722)
    goto <bb 480>; [15.00%]
  else
    goto <bb 479>; [85.00%]

  <bb 479> [0.19%]:
  MEM[(struct outBuff *)__first_1722] = emptyElement$buf_405;
  MEM[(struct outBuff *)__first_1722 + 8B] = emptyElement$bufSize_401;
  MEM[(struct outBuff *)__first_1722 + 12B] = emptyElement$blockNumber_394;
  MEM[(struct outBuff *)__first_1722 + 16B] = emptyElement$sequenceNumber_380;
  MEM[(struct outBuff *)__first_1722 + 20B] = emptyElement$inSize_370;
  MEM[(struct outBuff *)__first_1722 + 24B] = emptyElement$24_1077;
  MEM[(struct outBuff *)__first_1722 + 32B] = emptyElement$next_463;
  __first_1723 = __first_1722 + 40;
  goto <bb 478>; [100.00%]

  <bb 480> [0.03%]:
  _1693 = OutputBuffer.D.4140._M_impl._M_finish;
  _1694 = (long int) _1693;
  _1695 = OutputBuffer.D.4140._M_impl._M_start;
  _1696 = (long int) _1695;
  _1697 = _1694 - _1696;
  _1698 = _1697 /[ex] 40;
  _1699 = (long unsigned int) _1698;
  _1700 = _297 - _1699;

  <bb 481> [0.22%]:
  # __n_1701 = PHI <_1700(480), __n_1703(484)>
  # __cur_1702 = PHI <_1693(480), __cur_1704(484)>
  if (__n_1701 == 0)
    goto <bb 485>; [15.00%]
  else
    goto <bb 482>; [85.00%]

  <bb 482> [0.19%]:
  if (__cur_1702 != 0B)
    goto <bb 483>; [70.00%]
  else
    goto <bb 484>; [30.00%]

  <bb 483> [0.13%]:
  MEM[(struct outBuff *)__cur_1702] = emptyElement$buf_405;
  MEM[(struct outBuff *)__cur_1702 + 8B] = emptyElement$bufSize_401;
  MEM[(struct outBuff *)__cur_1702 + 12B] = emptyElement$blockNumber_394;
  MEM[(struct outBuff *)__cur_1702 + 16B] = emptyElement$sequenceNumber_380;
  MEM[(struct outBuff *)__cur_1702 + 20B] = emptyElement$inSize_370;
  MEM[(struct outBuff *)__cur_1702 + 24B] = emptyElement$24_1077;
  MEM[(struct outBuff *)__cur_1702 + 32B] = emptyElement$next_463;

  <bb 484> [0.19%]:
  __n_1703 = __n_1701 + 18446744073709551615;
  __cur_1704 = __cur_1702 + 40;
  goto <bb 481>; [100.00%]

  <bb 485> [0.03%]:
  OutputBuffer.D.4140._M_impl._M_finish = __cur_1702;
  goto <bb 489>; [100.00%]

  <bb 486> [0.45%]:
  # __first_1706 = PHI <_1676(476), __first_1708(487)>
  # __niter_1705 = PHI <_297(476), __niter_1707(487)>
  if (__niter_1705 == 0)
    goto <bb 488>; [15.00%]
  else
    goto <bb 487>; [85.00%]

  <bb 487> [0.39%]:
  MEM[(struct outBuff *)__first_1706] = emptyElement$buf_405;
  MEM[(struct outBuff *)__first_1706 + 8B] = emptyElement$bufSize_401;
  MEM[(struct outBuff *)__first_1706 + 12B] = emptyElement$blockNumber_394;
  MEM[(struct outBuff *)__first_1706 + 16B] = emptyElement$sequenceNumber_380;
  MEM[(struct outBuff *)__first_1706 + 20B] = emptyElement$inSize_370;
  MEM[(struct outBuff *)__first_1706 + 24B] = emptyElement$24_1077;
  MEM[(struct outBuff *)__first_1706 + 32B] = emptyElement$next_463;
  __niter_1707 = __niter_1705 + 18446744073709551615;
  __first_1708 = __first_1706 + 40;
  goto <bb 486>; [100.00%]

  <bb 488> [0.07%]:
  OutputBuffer.D.4140._M_impl._M_finish = __first_1706;

  <bb 489> [0.15%]:
  _1662 = OutputBuffer.D.4140._M_impl._M_finish;
  _1663 = (long int) _1662;
  _1664 = OutputBuffer.D.4140._M_impl._M_start;
  _1665 = (long int) _1664;
  _1666 = _1663 - _1665;
  _1667 = _1666 /[ex] 40;
  _1668 = (long unsigned int) _1667;
  if (_297 != _1668)
    goto <bb 490>; [0.04%]
  else
    goto <bb 493>; [99.96%]

  <bb 490> [0.00%]:
  _1669 = stderr;
  __fprintf_chk (_1669, 1, "pbzip2: *ERROR: Could not initialize (OutputBuffer); size=%lu!  Aborting...\n", _297);

  <bb 491> [0.00%]:
  _1670 = OutMutex;
  safe_mutex_unlock (_1670);

  <bb 492> [0.00%]:
  exit (1);

  <bb 493> [0.15%]:
  _1671 = OutMutex;
  ret_1726 = pthread_mutex_unlock (_1671);
  if (ret_1726 != 0)
    goto <bb 494>; [0.04%]
  else
    goto <bb 496>; [99.96%]

  <bb 494> [0.00%]:
  _1727 = stderr;
  __fprintf_chk (_1727, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1726);

  <bb 495> [0.00%]:
  cleanupAndQuit (-6);

  <bb 496> [0.15%]:
  if (decompress_2711 == 1)
    goto <bb 497>; [34.00%]
  else
    goto <bb 718>; [66.00%]

  <bb 497> [0.05%]:
  if (noThreads_440 == 0)
    goto <bb 498>; [50.00%]
  else
    goto <bb 575>; [50.00%]

  <bb 498> [0.03%]:
  _298 = QuietMode;
  if (_298 != 1)
    goto <bb 499>; [48.88%]
  else
    goto <bb 500>; [51.12%]

  <bb 499> [0.01%]:
  _299 = stderr;
  __builtin_fwrite ("Decompressing data...\n", 1, 22, _299);

  <bb 500> [0.03%]:
  # i_2337 = PHI <0(498), 0(499)>

  <bb 501> [0.18%]:
  # i_460 = PHI <i_2337(500), i_799(506)>
  _300 = (int) i_460;
  _301 = numCPU;
  if (_300 >= _301)
    goto <bb 507>; [7.50%]
  else
    goto <bb 502>; [92.50%]

  <bb 502> [0.17%]:
  _302 = _1563->consumers;
  _303 = i_460 * 8;
  _304 = _302 + _303;
  ret_798 = pthread_create (_304, &ChildThreadAttributes, consumer_decompress, _1563);
  if (ret_798 != 0)
    goto <bb 503>; [7.50%]
  else
    goto <bb 506>; [92.50%]

  <bb 503> [0.01%]:
  _801 = getInstance ();

  <bb 504> [0.01%]:
  saveError (_801);
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create consumer thread #%u (code = %d)  Aborting...\n", i_460, ret_798);

  <bb 505> [0.01%]:
  _305 = TerminatorThread;
  pthread_join (_305, 0B);
  goto <bb 959>; [100.00%]

  <bb 506> [0.15%]:
  i_799 = i_460 + 1;
  goto <bb 501>; [100.00%]

  <bb 507> [0.01%]:
  _905 = outFilename._M_dataplus._M_p;
  ret_806 = pthread_create (&output, &ChildThreadAttributes, fileWriter, _905);
  if (ret_806 != 0)
    goto <bb 508>; [2.00%]
  else
    goto <bb 510>; [98.00%]

  <bb 508> [0.00%]:
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create fileWriter thread (code = %d)  Aborting...\n", ret_806);

  <bb 509> [0.00%]:
  _306 = TerminatorThread;
  pthread_join (_306, 0B);
  goto <bb 959>; [100.00%]

  <bb 510> [0.01%]:
  safe_mutex_lock (&ProgressIndicatorsMutex);

  <bb 511> [0.01%]:
  InBytesProduced = 0;
  NumBlocks = 0;
  safe_mutex_unlock (&ProgressIndicatorsMutex);

  <bb 512> [0.01%]:
  MEM[(struct  &)&bz2StreamScanner] ={v} {CLOBBER};
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._vptr.BZ2StreamScanner = &MEM[(void *)&_ZTVN6pbzip216BZ2StreamScannerE + 16B];
  MEM[(struct  &)&bz2StreamScanner + 16] ={v} {CLOBBER};
  MEM[(struct  &)&bz2StreamScanner + 16] ={v} {CLOBBER};
  MEM[(struct _Alloc_hider *)&bz2StreamScanner + 16B]._M_p = &MEM[(struct basic_string *)&bz2StreamScanner + 16B].D.4058._M_local_buf;
  MEM[(struct basic_string *)&bz2StreamScanner + 16B]._M_string_length = 0;
  MEM[(char_type &)&bz2StreamScanner + 32] = 0;
  MEM[(struct  &)&bz2StreamScanner + 48] ={v} {CLOBBER};
  MEM[(struct  &)&bz2StreamScanner + 48] ={v} {CLOBBER};
  MEM[(struct _Alloc_hider *)&bz2StreamScanner + 48B]._M_p = &MEM[(struct basic_string *)&bz2StreamScanner + 48B].D.4058._M_local_buf;
  MEM[(struct basic_string *)&bz2StreamScanner + 48B]._M_string_length = 0;
  MEM[(char_type &)&bz2StreamScanner + 64] = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuff = 0B;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffCapacity = 0;
  MEM[(struct  &)&bz2StreamScanner + 128] ={v} {CLOBBER};
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].buf = 0B;
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].bufSize = 0;
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].blockNumber = 0;
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].sequenceNumber = 0;
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].inSize = 0;
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].isLastInSequence = 1;
  MEM[(struct outBuff *)&bz2StreamScanner + 128B].next = 0B;
  dispose (&bz2StreamScanner);
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
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._hInFile = hInfile_390;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._eof = 0;
  assign (&MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._bz2Header, &bz2header);

  <bb 513> [0.01%]:
  assign (&MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._bz2HeaderZero, &bz2ZeroHeader);

  <bb 514> [0.01%]:
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._bz2HeaderFound = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffCapacity = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._errState = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._searchStatus = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._outBuffCapacityHint = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._outBuffCapacityLimit = 1048576;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._outSequenceNumber = 0;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._streamNumber = 0;
  _1778 = MEM[(const struct basic_string *)&bz2StreamScanner + 16B]._M_string_length;
  _1779 = _1778 * 2;
  inBuffCapacity_422 = MAX_EXPR <_1779, 1048576>;
  _1781 = operator new [] (inBuffCapacity_422, &nothrow);
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuff = _1781;
  if (_1781 == 0B)
    goto <bb 515>; [0.43%]
  else
    goto <bb 516>; [99.57%]

  <bb 515> [0.00%]:
  _1782 = MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._errState;
  _1783 = _1782 | 1;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._errState = _1783;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffEnd = 0B;
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (FileData)!  Aborting...\n");
  goto <bb 517>; [100.00%]

  <bb 516> [0.01%]:
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffCapacity = inBuffCapacity_422;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffEnd = _1781;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffSearchPtr = _1781;
  MEM[(struct BZ2StreamScanner *)&bz2StreamScanner]._inBuffCurrent = _1781;

  <bb 517> [0.01%]:
  bz2header ={v} {CLOBBER};
  bz2ZeroHeader ={v} {CLOBBER};
  _1728 = getNextStream (&bz2StreamScanner);
  goto <bb 524>; [100.00%]

<L32> [0.00%]:
  _1772 = MEM[(const struct basic_string *)&bz2StreamScanner + 48B]._M_dataplus._M_p;
  if (&MEM[(struct basic_string *)&bz2StreamScanner + 48B].D.4058._M_local_buf != _1772)
    goto <bb 519>; [53.47%]
  else
    goto <bb 520>; [46.53%]

  <bb 519> [0.00%]:
  operator delete (_1772);

  <bb 520> [0.00%]:
  MEM[(struct  &)&bz2StreamScanner + 48] ={v} {CLOBBER};
  resx 50

<L33> [0.00%]:
  _1773 = MEM[(const struct basic_string *)&bz2StreamScanner + 16B]._M_dataplus._M_p;
  if (&MEM[(struct basic_string *)&bz2StreamScanner + 16B].D.4058._M_local_buf != _1773)
    goto <bb 522>; [53.47%]
  else
    goto <bb 523>; [46.53%]

  <bb 522> [0.00%]:
  operator delete (_1773);

  <bb 523> [0.00%]:
  MEM[(struct  &)&bz2StreamScanner + 16] ={v} {CLOBBER};
  resx 49

  <bb 524> [0.01%]:
  fileData = _1728;

  <bb 525> [0.02%]:
  _1729 = bz2StreamScanner._errState;
  if (_1729 == 0)
    goto <bb 526>; [96.25%]
  else
    goto <bb 528>; [3.75%]

  <bb 526> [0.02%]:
  _1730 = fileData;
  _1731 = _1730->bufSize;
  if (_1731 != 0)
    goto <bb 527>; [96.25%]
  else
    goto <bb 528>; [3.75%]

  <bb 527> [0.02%]:
  _1732 = NumBlocks;
  _1733 = producerDecompressCheckInterrupt (hInfile_390, &fileData, _1732);
  goto <bb 529>; [100.00%]

  <bb 528> [0.00%]:
  close (hInfile_390);
  goto <bb 557>; [100.00%]

  <bb 529> [0.02%]:
  if (_1733 != 0)
    goto <bb 530>; [3.75%]
  else
    goto <bb 534>; [96.25%]

  <bb 530> [0.00%]:
  _1734 = _1563->mut;
  safe_mutex_lock (_1734);

  <bb 531> [0.00%]:
  _1735 = _1563->notEmpty;
  safe_cond_broadcast (_1735);

  <bb 532> [0.00%]:
  _1736 = _1563->mut;
  safe_mutex_unlock (_1736);

  <bb 533> [0.00%]:
  syncSetProducerDone (1);
  goto <bb 566>; [100.00%]

  <bb 534> [0.02%]:
  _1737 = QuietMode;
  if (_1737 != 1)
    goto <bb 535>; [79.76%]
  else
    goto <bb 538>; [20.24%]

  <bb 535> [0.02%]:
  _1738 = fileData;
  _1739 = _1738->bufSize;
  if (_1739 > 250000000)
    goto <bb 536>; [33.00%]
  else
    goto <bb 538>; [67.00%]

  <bb 536> [0.01%]:
  _1740 = (long unsigned int) _1739;
  _1741 = stderr;
  __fprintf_chk (_1741, 1, "pbzip2:  *WARNING: Compressed block size is large [%lu bytes].\n", _1740);

  <bb 537> [0.01%]:
  _1742 = stderr;
  __builtin_fwrite ("          If program aborts, use regular BZIP2 to decompress.\n", 1, 62, _1742);

  <bb 538> [0.02%]:
  _1743 = _1563->mut;
  safe_mutex_lock (_1743);

  <bb 539> [0.15%]:
  _1744 = _1563->full;
  if (_1744 == 0)
    goto <bb 549>; [7.50%]
  else
    goto <bb 540>; [92.50%]

  <bb 540> [0.14%]:
  _1745 = _1563->mut;
  _1746 = _1563->notFull;
  _1785 = pthread_cond_wait (_1746, _1745);

  <bb 541> [0.14%]:
  if (_1785 != 0)
    goto <bb 542>; [0.04%]
  else
    goto <bb 544>; [99.96%]

  <bb 542> [0.00%]:
  _1786 = stderr;
  __fprintf_chk (_1786, 1, "pthread_cond_wait error [%d]! Aborting immediately!\n", _1785);

  <bb 543> [0.00%]:
  pthread_mutex_unlock (_1745);
  cleanupAndQuit (-8);

  <bb 544> [0.14%]:
  _1747 = NumBlocks;
  _1748 = producerDecompressCheckInterrupt (hInfile_390, &fileData, _1747);

  <bb 545> [0.14%]:
  if (_1748 != 0)
    goto <bb 546>; [7.50%]
  else
    goto <bb 539>; [92.50%]

  <bb 546> [0.01%]:
  _1749 = _1563->notEmpty;
  safe_cond_broadcast (_1749);

  <bb 547> [0.01%]:
  syncSetProducerDone (1);

  <bb 548> [0.01%]:
  _1750 = _1563->mut;
  safe_mutex_unlock (_1750);
  goto <bb 566>; [100.00%]

  <bb 549> [0.01%]:
  _1751 = fileData;
  add (_1563, _1751);
  _1752 = _1563->notEmpty;
  ret_1787 = pthread_cond_signal (_1752);
  if (ret_1787 != 0)
    goto <bb 550>; [0.04%]
  else
    goto <bb 552>; [99.96%]

  <bb 550> [0.00%]:
  _1788 = stderr;
  __fprintf_chk (_1788, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_1787);

  <bb 551> [0.00%]:
  cleanupAndQuit (-7);

  <bb 552> [0.01%]:
  safe_mutex_lock (&ProgressIndicatorsMutex);

  <bb 553> [0.01%]:
  _1754 = _1751->bufSize;
  _1755 = (long int) _1754;
  _1756 = InBytesProduced;
  _1757 = _1755 + _1756;
  InBytesProduced = _1757;
  _1758 = _1751->blockNumber;
  _1759 = _1758 + 1;
  NumBlocks = _1759;
  safe_mutex_unlock (&ProgressIndicatorsMutex);

  <bb 554> [0.01%]:
  _1760 = _1563->mut;
  safe_mutex_unlock (_1760);

  <bb 555> [0.01%]:
  _1761 = getNextStream (&bz2StreamScanner);

  <bb 556> [0.01%]:
  fileData = _1761;
  goto <bb 525>; [100.00%]

  <bb 557> [0.00%]:
  _1762 = fileData;
  operator delete (_1762, 40);
  _1763 = bz2StreamScanner._errState;
  if (_1763 != 0)
    goto <bb 558>; [33.00%]
  else
    goto <bb 559>; [67.00%]

  <bb 558> [0.00%]:
  handle_error (1, 1, "pbzip2: producer_decompress: *ERROR: when reading bzip2 input stream\n");
  goto <bb 566>; [100.00%]

  <bb 559> [0.00%]:
  _1764 = bz2StreamScanner._bz2HeaderFound;
  if (_1764 != 0)
    goto <bb 560>; [67.00%]
  else
    goto <bb 562>; [33.00%]

  <bb 560> [0.00%]:
  _1765 = bz2StreamScanner._eof;
  if (_1765 != 0)
    goto <bb 561>; [33.00%]
  else
    goto <bb 562>; [67.00%]

  <bb 561> [0.00%]:
  syncSetProducerDone (1);
  goto <bb 563>; [100.00%]

  <bb 562> [0.00%]:
  handle_error (1, 1, "pbzip2: producer_decompress: *ERROR: input file is not a valid bzip2 stream\n");
  goto <bb 566>; [100.00%]

  <bb 563> [0.00%]:
  _1766 = _1563->mut;
  safe_mutex_lock (_1766);

  <bb 564> [0.00%]:
  _1767 = _1563->notEmpty;
  safe_cond_broadcast (_1767);

  <bb 565> [0.00%]:
  _1768 = _1563->mut;
  safe_mutex_unlock (_1768);

  <bb 566> [0.01%]:
  # _1769 = PHI <0(533), 0(548), -1(558), -1(562), 0(565)>
  __comp_dtor  (&bz2StreamScanner);
  bz2StreamScanner ={v} {CLOBBER};
  fileData ={v} {CLOBBER};
  if (_1769 == -99)
    goto <bb 568>; [20.24%]
  else
    goto <bb 573>; [79.76%]

<L31> [0.00%]:
  __comp_dtor  (&bz2StreamScanner);
  resx 48

  <bb 568> [0.00%]:
  _308 = output;
  _810 = pthread_join (_308, 0B);

  <bb 569> [0.00%]:
  if (_810 != 0)
    goto <bb 570>; [2.00%]
  else
    goto <bb 575>; [98.00%]

  <bb 570> [0.00%]:
  _812 = getInstance ();

  <bb 571> [0.00%]:
  saveError (_812);
  handle_error (1, 1, "pbzip2: *ERROR: Error joining fileWriter thread (code = %d)  Aborting...\n", _1769);

  <bb 572> [0.00%]:
  _309 = TerminatorThread;
  pthread_join (_309, 0B);
  goto <bb 959>; [100.00%]

  <bb 573> [0.01%]:
  if (_1769 != 0)
    goto <bb 574>; [50.00%]
  else
    goto <bb 575>; [50.00%]

  <bb 574> [0.01%]:

  <bb 575> [0.04%]:
  # errLevel_428 = PHI <errLevel_426(497), errLevel_426(569), errLevel_426(573), 1(574)>
  # noThreads_441 = PHI <noThreads_440(497), 1(569), noThreads_440(573), noThreads_440(574)>
  # switchedMtToSt_469 = PHI <0(497), 1(569), 0(573), 0(574)>
  # errLevelCurrentFile_471 = PHI <0(497), 0(569), 0(573), 1(574)>
  _310 = noThreads_441 == 1;
  _311 = errLevelCurrentFile_471 == 0;
  _312 = _310 & _311;
  if (_312 != 0)
    goto <bb 576>; [50.00%]
  else
    goto <bb 884>; [50.00%]

  <bb 576> [0.02%]:
  _313 = QuietMode;
  if (_313 != 1)
    goto <bb 577>; [48.88%]
  else
    goto <bb 578>; [51.12%]

  <bb 577> [0.01%]:
  _314 = stderr;
  __builtin_fwrite ("Decompressing data (no threads)...\n", 1, 35, _314);

  <bb 578> [0.02%]:
  if (hInfile_390 > 0)
    goto <bb 579>; [64.00%]
  else
    goto <bb 580>; [36.00%]

  <bb 579> [0.01%]:
  close (hInfile_390);

  <bb 580> [0.02%]:
  _906 = outFilename._M_dataplus._M_p;
  nUnused = 0;
  _1789 = strcmp (InFilename_685, "-");
  if (_1789 != 0)
    goto <bb 581>; [33.00%]
  else
    goto <bb 584>; [67.00%]

  <bb 581> [0.01%]:
  _1790 = fopen (InFilename_685, "rb");

  <bb 582> [0.01%]:
  if (_1790 == 0B)
    goto <bb 583>; [0.43%]
  else
    goto <bb 585>; [99.57%]

  <bb 583> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not open input file [%s]!  Aborting...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 584> [0.01%]:
  zStream_1791 = stdin;

  <bb 585> [0.02%]:
  # zStream_1792 = PHI <_1790(582), zStream_1791(584)>
  _1793 = ferror (zStream_1792);
  if (_1793 != 0)
    goto <bb 586>; [2.00%]
  else
    goto <bb 589>; [98.00%]

  <bb 586> [0.00%]:
  _1794 = stdin;
  if (zStream_1792 != _1794)
    goto <bb 587>; [57.44%]
  else
    goto <bb 588>; [42.56%]

  <bb 587> [0.00%]:
  fclose (zStream_1792);

  <bb 588> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with input stream of file [%s]!  Aborting...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 589> [0.02%]:
  _1795 = OutputStdOut;
  if (_1795 == 0)
    goto <bb 590>; [33.00%]
  else
    goto <bb 593>; [67.00%]

  <bb 590> [0.01%]:
  _1796 = safe_fopen_output (_906, "wb");

  <bb 591> [0.01%]:
  if (_1796 == 0B)
    goto <bb 592>; [0.43%]
  else
    goto <bb 594>; [99.57%]

  <bb 592> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not open output file [%s]!  Aborting...\n", _906);
  goto <bb 716>; [100.00%]

  <bb 593> [0.01%]:
  stream_1797 = stdout;

  <bb 594> [0.02%]:
  # stream_1798 = PHI <_1796(591), stream_1797(593)>
  _1799 = ferror (stream_1798);
  if (_1799 != 0)
    goto <bb 595>; [2.00%]
  else
    goto <bb 599>; [98.00%]

  <bb 595> [0.00%]:
  _1800 = stdout;
  if (stream_1798 != _1800)
    goto <bb 596>; [57.44%]
  else
    goto <bb 597>; [42.56%]

  <bb 596> [0.00%]:
  fclose (stream_1798);

  <bb 597> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with output stream of file [%s]!  Aborting...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 598> [0.01%]:
  # streamNo_1522 = PHI <streamNo_1814(683), streamNo_1814(680)>

  <bb 599> [0.03%]:
  # streamNo_1810 = PHI <streamNo_1522(598), 0(594)>
  safe_mutex_lock (&TerminateFlagMutex);

  <bb 600> [0.03%]:
  ret_1801 = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);

  <bb 601> [0.03%]:
  if (ret_1801 != 0)
    goto <bb 602>; [2.00%]
  else
    goto <bb 607>; [98.00%]

  <bb 602> [0.00%]:
  _1802 = stderr;
  __builtin_fwrite ("directdecompress: terminating1 - terminateFlag set\n", 1, 51, _1802);

  <bb 603> [0.00%]:
  _1803 = stdin;
  if (zStream_1792 != _1803)
    goto <bb 604>; [53.47%]
  else
    goto <bb 605>; [46.53%]

  <bb 604> [0.00%]:
  fclose (zStream_1792);

  <bb 605> [0.00%]:
  _1804 = stdout;
  if (stream_1798 != _1804)
    goto <bb 606>; [53.47%]
  else
    goto <bb 716>; [46.53%]

  <bb 606> [0.00%]:
  fclose (stream_1798);
  goto <bb 716>; [100.00%]

  <bb 607> [0.03%]:
  _1805 = nUnused;
  _1807 = BZ2_bzReadOpen (&bzerr, zStream_1792, 0, 0, &unused, _1805);

  <bb 608> [0.03%]:
  if (_1807 == 0B)
    goto <bb 610>; [2.00%]
  else
    goto <bb 609>; [98.00%]

  <bb 609> [0.02%]:
  _1808 = bzerr;
  if (_1808 != 0)
    goto <bb 610>; [2.00%]
  else
    goto <bb 617>; [98.00%]

  <bb 610> [0.00%]:
  _1809 = bzerr;
  _1811 = testBZ2ErrorHandling (_1809, _1807, streamNo_1810);

  <bb 611> [0.00%]:
  _1812 = stdin;
  if (zStream_1792 != _1812)
    goto <bb 612>; [53.47%]
  else
    goto <bb 613>; [46.53%]

  <bb 612> [0.00%]:
  fclose (zStream_1792);

  <bb 613> [0.00%]:
  _1813 = stdout;
  if (stream_1798 != _1813)
    goto <bb 614>; [53.47%]
  else
    goto <bb 615>; [46.53%]

  <bb 614> [0.00%]:
  fclose (stream_1798);

  <bb 615> [0.00%]:
  if (_1811 != 0)
    goto <bb 616>; [33.00%]
  else
    goto <bb 716>; [67.00%]

  <bb 616> [0.00%]:
  syncSetTerminateFlag (1);
  goto <bb 716>; [100.00%]

  <bb 617> [0.02%]:
  streamNo_1814 = streamNo_1810 + 1;

  <bb 618> [0.20%]:
  _1815 = bzerr;
  if (_1815 != 0)
    goto <bb 664>; [3.75%]
  else
    goto <bb 619>; [96.25%]

  <bb 619> [0.19%]:
  safe_mutex_lock (&TerminateFlagMutex);

  <bb 620> [0.19%]:
  ret_1816 = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);

  <bb 621> [0.19%]:
  if (ret_1816 != 0)
    goto <bb 622>; [3.75%]
  else
    goto <bb 627>; [96.25%]

  <bb 622> [0.01%]:
  _1817 = stderr;
  __builtin_fwrite ("directdecompress: terminating2 - terminateFlag set\n", 1, 51, _1817);

  <bb 623> [0.01%]:
  _1818 = stdin;
  if (zStream_1792 != _1818)
    goto <bb 624>; [53.47%]
  else
    goto <bb 625>; [46.53%]

  <bb 624> [0.00%]:
  fclose (zStream_1792);

  <bb 625> [0.01%]:
  _1819 = stdout;
  if (stream_1798 != _1819)
    goto <bb 626>; [53.47%]
  else
    goto <bb 716>; [46.53%]

  <bb 626> [0.00%]:
  fclose (stream_1798);
  goto <bb 716>; [100.00%]

  <bb 627> [0.19%]:
  _1820 = BZ2_bzRead (&bzerr, _1807, &obuf, 5000);

  <bb 628> [0.19%]:
  _1821 = bzerr;
  if (_1821 == -5)
    goto <bb 629>; [34.00%]
  else
    goto <bb 653>; [66.00%]

  <bb 629> [0.06%]:
  _1822 = ForceOverwrite;
  if (_1822 == 1)
    goto <bb 630>; [3.75%]
  else
    goto <bb 653>; [96.25%]

  <bb 630> [0.00%]:
  rewind (zStream_1792);

  <bb 631> [0.02%]:
  _1823 = fgetc (zStream_1792);

  <bb 632> [0.02%]:
  if (_1823 == -1)
    goto <bb 684> (closeok); [5.00%]
  else
    goto <bb 633>; [95.00%]

  <bb 633> [0.02%]:
  ungetc (_1823, zStream_1792);

  <bb 634> [0.02%]:
  _1824 = __fread_alias (&obuf, 1, 5000, zStream_1792);

  <bb 635> [0.02%]:
  nread_1825 = (int) _1824;
  _1826 = ferror (zStream_1792);
  if (_1826 != 0)
    goto <bb 636>; [5.00%]
  else
    goto <bb 643>; [95.00%]

  <bb 636> [0.00%]:
  _1827 = bzerr;
  _1828 = testBZ2ErrorHandling (_1827, _1807, streamNo_1814);

  <bb 637> [0.00%]:
  _1829 = stdin;
  if (zStream_1792 != _1829)
    goto <bb 638>; [53.47%]
  else
    goto <bb 639>; [46.53%]

  <bb 638> [0.00%]:
  fclose (zStream_1792);

  <bb 639> [0.00%]:
  _1830 = stdout;
  if (stream_1798 != _1830)
    goto <bb 640>; [53.47%]
  else
    goto <bb 641>; [46.53%]

  <bb 640> [0.00%]:
  fclose (stream_1798);

  <bb 641> [0.00%]:
  if (_1828 != 0)
    goto <bb 642>; [33.00%]
  else
    goto <bb 716>; [67.00%]

  <bb 642> [0.00%]:
  syncSetTerminateFlag (1);
  goto <bb 716>; [100.00%]

  <bb 643> [0.02%]:
  if (nread_1825 > 0)
    goto <bb 644>; [46.68%]
  else
    goto <bb 645>; [53.32%]

  <bb 644> [0.01%]:
  _1831 = (long unsigned int) nread_1825;
  fwrite (&obuf, 1, _1831, stream_1798);

  <bb 645> [0.02%]:
  _1832 = ferror (stream_1798);
  if (_1832 != 0)
    goto <bb 646>; [5.00%]
  else
    goto <bb 631>; [95.00%]

  <bb 646> [0.00%]:
  _1833 = bzerr;
  _1834 = testBZ2ErrorHandling (_1833, _1807, streamNo_1814);

  <bb 647> [0.00%]:
  _1835 = stdin;
  if (zStream_1792 != _1835)
    goto <bb 648>; [53.47%]
  else
    goto <bb 649>; [46.53%]

  <bb 648> [0.00%]:
  fclose (zStream_1792);

  <bb 649> [0.00%]:
  _1836 = stdout;
  if (stream_1798 != _1836)
    goto <bb 650>; [53.47%]
  else
    goto <bb 651>; [46.53%]

  <bb 650> [0.00%]:
  fclose (stream_1798);

  <bb 651> [0.00%]:
  if (_1834 != 0)
    goto <bb 652>; [33.00%]
  else
    goto <bb 716>; [67.00%]

  <bb 652> [0.00%]:
  syncSetTerminateFlag (1);
  goto <bb 716>; [100.00%]

  <bb 653> [0.18%]:
  _2058 = _1821 == 4;
  _2060 = _1821 == 0;
  _2061 = _2058 | _2060;
  if (_2061 != 0)
    goto <bb 654>; [67.00%]
  else
    goto <bb 656>; [33.00%]

  <bb 654> [0.12%]:
  if (_1820 > 0)
    goto <bb 655>; [46.68%]
  else
    goto <bb 656>; [53.32%]

  <bb 655> [0.06%]:
  _1837 = (long unsigned int) _1820;
  fwrite (&obuf, 1, _1837, stream_1798);

  <bb 656> [0.18%]:
  _1838 = ferror (stream_1798);
  if (_1838 != 0)
    goto <bb 657>; [3.75%]
  else
    goto <bb 618>; [96.25%]

  <bb 657> [0.01%]:
  _1839 = bzerr;
  _1840 = testBZ2ErrorHandling (_1839, _1807, streamNo_1814);

  <bb 658> [0.01%]:
  _1841 = stdin;
  if (zStream_1792 != _1841)
    goto <bb 659>; [53.47%]
  else
    goto <bb 660>; [46.53%]

  <bb 659> [0.00%]:
  fclose (zStream_1792);

  <bb 660> [0.01%]:
  _1842 = stdout;
  if (stream_1798 != _1842)
    goto <bb 661>; [53.47%]
  else
    goto <bb 662>; [46.53%]

  <bb 661> [0.00%]:
  fclose (stream_1798);

  <bb 662> [0.01%]:
  if (_1840 != 0)
    goto <bb 663>; [33.00%]
  else
    goto <bb 716>; [67.00%]

  <bb 663> [0.00%]:
  syncSetTerminateFlag (1);
  goto <bb 716>; [100.00%]

  <bb 664> [0.01%]:
  if (_1815 != 4)
    goto <bb 665>; [2.00%]
  else
    goto <bb 672>; [98.00%]

  <bb 665> [0.00%]:
  _1843 = testBZ2ErrorHandling (_1815, _1807, streamNo_1814);

  <bb 666> [0.00%]:
  _1844 = stdin;
  if (zStream_1792 != _1844)
    goto <bb 667>; [53.47%]
  else
    goto <bb 668>; [46.53%]

  <bb 667> [0.00%]:
  fclose (zStream_1792);

  <bb 668> [0.00%]:
  _1845 = stdout;
  if (stream_1798 != _1845)
    goto <bb 669>; [53.47%]
  else
    goto <bb 670>; [46.53%]

  <bb 669> [0.00%]:
  fclose (stream_1798);

  <bb 670> [0.00%]:
  if (_1843 != 0)
    goto <bb 671>; [33.00%]
  else
    goto <bb 716>; [67.00%]

  <bb 671> [0.00%]:
  syncSetTerminateFlag (1);
  goto <bb 716>; [100.00%]

  <bb 672> [0.01%]:
  BZ2_bzReadGetUnused (&bzerr, _1807, &unusedTmp, &nUnused);

  <bb 673> [0.01%]:
  _1846 = bzerr;
  if (_1846 != 0)
    goto <bb 674>; [2.00%]
  else
    goto <bb 675>; [98.00%]

  <bb 674> [0.00%]:
  handle_error (1, 3, "pbzip2: *ERROR: Unexpected error [bzerr=%d]. Aborting!\n", _1846);
  goto <bb 716>; [100.00%]

  <bb 675> [0.05%]:
  # i_1848 = PHI <0(673), i_1853(676)>
  _1847 = nUnused;
  if (_1847 <= i_1848)
    goto <bb 677>; [15.00%]
  else
    goto <bb 676>; [85.00%]

  <bb 676> [0.04%]:
  _1849 = unusedTmp;
  _1850 = (sizetype) i_1848;
  _1851 = _1849 + _1850;
  _1852 = *_1851;
  unused[i_1848] = _1852;
  i_1853 = i_1848 + 1;
  goto <bb 675>; [100.00%]

  <bb 677> [0.01%]:
  BZ2_bzReadClose (&bzerr, _1807);

  <bb 678> [0.01%]:
  _1854 = bzerr;
  if (_1854 != 0)
    goto <bb 679>; [2.00%]
  else
    goto <bb 680>; [98.00%]

  <bb 679> [0.00%]:
  handle_error (1, 3, "pbzip2: *ERROR: Unexpected error [bzerr=%d]. Aborting!\n", _1854);
  goto <bb 716>; [100.00%]

  <bb 680> [0.01%]:
  _1855 = nUnused;
  if (_1855 == 0)
    goto <bb 681>; [33.00%]
  else
    goto <bb 598>; [67.00%]

  <bb 681> [0.00%]:
  _1856 = fgetc (zStream_1792);

  <bb 682> [0.00%]:
  if (_1856 == -1)
    goto <bb 684> (closeok); [2.00%]
  else
    goto <bb 683>; [98.00%]

  <bb 683> [0.00%]:
  ungetc (_1856, zStream_1792);
  goto <bb 598>; [100.00%]

closeok [0.00%]:
  _1857 = ferror (zStream_1792);
  if (_1857 != 0)
    goto <bb 685>; [3.98%]
  else
    goto <bb 690>; [96.02%]

  <bb 685> [0.00%]:
  _1858 = stdin;
  if (zStream_1792 != _1858)
    goto <bb 686>; [53.47%]
  else
    goto <bb 687>; [46.53%]

  <bb 686> [0.00%]:
  fclose (zStream_1792);

  <bb 687> [0.00%]:
  _1859 = stdout;
  if (stream_1798 != _1859)
    goto <bb 688>; [57.44%]
  else
    goto <bb 689>; [42.56%]

  <bb 688> [0.00%]:
  fclose (stream_1798);

  <bb 689> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with input stream of file [%s]!  Skipping...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 690> [0.00%]:
  _1869 = fclose (zStream_1792);

  <bb 691> [0.00%]:
  if (_1869 == -1)
    goto <bb 692>; [22.95%]
  else
    goto <bb 694>; [77.05%]

  <bb 692> [0.00%]:
  _1870 = getInstance ();

  <bb 693> [0.00%]:
  saveError (_1870);

  <bb 694> [0.00%]:
  if (_1869 == -1)
    goto <bb 695>; [1.04%]
  else
    goto <bb 696>; [98.96%]

  <bb 695> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 696> [0.00%]:
  _1861 = ferror (stream_1798);
  if (_1861 != 0)
    goto <bb 697>; [3.98%]
  else
    goto <bb 700>; [96.02%]

  <bb 697> [0.00%]:
  _1862 = stdout;
  if (stream_1798 != _1862)
    goto <bb 698>; [57.44%]
  else
    goto <bb 699>; [42.56%]

  <bb 698> [0.00%]:
  fclose (stream_1798);

  <bb 699> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with output stream of file [%s]!  Skipping...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 700> [0.00%]:
  _1871 = fflush (stream_1798);

  <bb 701> [0.00%]:
  if (_1871 == -1)
    goto <bb 702>; [22.95%]
  else
    goto <bb 704>; [77.05%]

  <bb 702> [0.00%]:
  _1872 = getInstance ();

  <bb 703> [0.00%]:
  saveError (_1872);

  <bb 704> [0.00%]:
  if (_1871 != 0)
    goto <bb 705>; [2.00%]
  else
    goto <bb 708>; [98.00%]

  <bb 705> [0.00%]:
  _1864 = stdout;
  if (stream_1798 != _1864)
    goto <bb 706>; [57.44%]
  else
    goto <bb 707>; [42.56%]

  <bb 706> [0.00%]:
  fclose (stream_1798);

  <bb 707> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem with output stream of file [%s]!  Skipping...\n", InFilename_685);
  goto <bb 716>; [100.00%]

  <bb 708> [0.00%]:
  _1865 = stdout;
  if (stream_1798 != _1865)
    goto <bb 709>; [70.00%]
  else
    goto <bb 715>; [30.00%]

  <bb 709> [0.00%]:
  _1873 = fclose (stream_1798);

  <bb 710> [0.00%]:
  if (_1873 == -1)
    goto <bb 711>; [22.95%]
  else
    goto <bb 713>; [77.05%]

  <bb 711> [0.00%]:
  _1874 = getInstance ();

  <bb 712> [0.00%]:
  saveError (_1874);

  <bb 713> [0.00%]:
  if (_1873 == -1)
    goto <bb 714>; [0.52%]
  else
    goto <bb 715>; [99.48%]

  <bb 714> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Problem closing file [%s]!  Skipping...\n", _906);
  goto <bb 716>; [100.00%]

  <bb 715> [0.00%]:
  syncSetProducerDone (1);

  <bb 716> [0.02%]:
  # _1867 = PHI <-1(583), -1(588), -1(592), -1(597), -1(605), -1(606), _1811(615), _1811(616), -1(625), -1(626), _1828(641), _1828(642), _1834(651), _1834(652), _1840(662), _1840(663), _1843(670), _1843(671), 3(674), 3(679), -1(689), -1(695), -1(699), -1(707), -1(714), 0(715)>
  obuf ={v} {CLOBBER};
  unused ={v} {CLOBBER};
  unusedTmp ={v} {CLOBBER};
  bzerr ={v} {CLOBBER};
  nUnused ={v} {CLOBBER};
  if (_1867 != 0)
    goto <bb 717>; [50.00%]
  else
    goto <bb 884>; [50.00%]

  <bb 717> [0.01%]:
  goto <bb 884>; [100.00%]

  <bb 718> [0.10%]:
  if (noThreads_440 == 0)
    goto <bb 719>; [50.00%]
  else
    goto <bb 822>; [50.00%]

  <bb 719> [0.05%]:
  _315 = QuietMode;
  if (_315 != 1)
    goto <bb 720>; [48.88%]
  else
    goto <bb 721>; [51.12%]

  <bb 720> [0.02%]:
  _316 = stderr;
  __builtin_fwrite ("Compressing data...\n", 1, 20, _316);

  <bb 721> [0.05%]:
  # i_1916 = PHI <0(719), 0(720)>

  <bb 722> [0.35%]:
  # i_461 = PHI <i_1916(721), i_785(727)>
  _317 = (int) i_461;
  _318 = numCPU;
  if (_317 >= _318)
    goto <bb 728>; [7.50%]
  else
    goto <bb 723>; [92.50%]

  <bb 723> [0.32%]:
  _319 = _1563->consumers;
  _320 = i_461 * 8;
  _321 = _319 + _320;
  ret_784 = pthread_create (_321, &ChildThreadAttributes, consumer, _1563);
  if (ret_784 != 0)
    goto <bb 724>; [7.50%]
  else
    goto <bb 727>; [92.50%]

  <bb 724> [0.02%]:
  _787 = getInstance ();

  <bb 725> [0.02%]:
  saveError (_787);
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create consumer thread #%u (code = %d)  Aborting...\n", i_461, ret_784);

  <bb 726> [0.02%]:
  _322 = TerminatorThread;
  pthread_join (_322, 0B);
  goto <bb 959>; [100.00%]

  <bb 727> [0.30%]:
  i_785 = i_461 + 1;
  goto <bb 722>; [100.00%]

  <bb 728> [0.03%]:
  _907 = outFilename._M_dataplus._M_p;
  ret_792 = pthread_create (&output, &ChildThreadAttributes, fileWriter, _907);
  if (ret_792 != 0)
    goto <bb 729>; [2.00%]
  else
    goto <bb 731>; [98.00%]

  <bb 729> [0.00%]:
  handle_error (1, 1, "pbzip2: *ERROR: Not enough resources to create fileWriter thread (code = %d)  Aborting...\n", ret_792);

  <bb 730> [0.00%]:
  _323 = TerminatorThread;
  pthread_join (_323, 0B);
  goto <bb 959>; [100.00%]

  <bb 731> [0.03%]:
  ret_1903 = pthread_mutex_lock (&ProgressIndicatorsMutex);
  if (ret_1903 != 0)
    goto <bb 732>; [0.04%]
  else
    goto <bb 734>; [99.96%]

  <bb 732> [0.00%]:
  _1904 = stderr;
  __fprintf_chk (_1904, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1903);

  <bb 733> [0.00%]:
  cleanupAndQuit (-5);

  <bb 734> [0.03%]:
  NumBlocks = 0;
  InBytesProduced = 0;
  ret_1905 = pthread_mutex_unlock (&ProgressIndicatorsMutex);
  if (ret_1905 != 0)
    goto <bb 735>; [0.04%]
  else
    goto <bb 737>; [99.96%]

  <bb 735> [0.00%]:
  _1906 = stderr;
  __fprintf_chk (_1906, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1905);

  <bb 736> [0.00%]:
  cleanupAndQuit (-6);

  <bb 737> [0.05%]:
  ret_1907 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_1907 != 0)
    goto <bb 738>; [0.04%]
  else
    goto <bb 740>; [99.96%]

  <bb 738> [0.00%]:
  _1908 = stderr;
  __fprintf_chk (_1908, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1907);

  <bb 739> [0.00%]:
  cleanupAndQuit (-5);

  <bb 740> [0.05%]:
  ret_1875 = terminateFlag;
  ret_1909 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_1909 != 0)
    goto <bb 741>; [0.04%]
  else
    goto <bb 743>; [99.96%]

  <bb 741> [0.00%]:
  _1910 = stderr;
  __fprintf_chk (_1910, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1909);

  <bb 742> [0.00%]:
  cleanupAndQuit (-6);

  <bb 743> [0.05%]:
  if (ret_1875 != 0)
    goto <bb 744>; [2.50%]
  else
    goto <bb 745>; [97.50%]

  <bb 744> [0.00%]:
  close (hInfile_390);
  goto <bb 820>; [100.00%]

  <bb 745> [0.05%]:
  inSize_1876 = (size_t) blockSize_399;
  FileData_1877 = operator new [] (inSize_1876, &nothrow);
  if (FileData_1877 == 0B)
    goto <bb 746>; [2.50%]
  else
    goto <bb 748>; [97.50%]

  <bb 746> [0.00%]:
  close (hInfile_390);

  <bb 747> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (FileData)!  Aborting...\n");
  goto <bb 820>; [100.00%]

  <bb 748> [0.31%]:
  # buf_1914 = PHI <FileData_1877(745), buf_1929(755)>
  # bufr_1923 = PHI <0(745), bufr_1926(755)>
  # rsize_1912 = PHI <blockSize_399(745), rsize_1928(755)>
  _1913 = (long unsigned int) rsize_1912;
  _1919 = __read_alias (hInfile_390, buf_1914, _1913);

  <bb 749> [0.31%]:
  ret_1921 = (int) _1919;
  if (ret_1921 < 0)
    goto <bb 750>; [5.00%]
  else
    goto <bb 751>; [95.00%]

  <bb 750> [0.02%]:
  _1922 = (long int) ret_1921;
  goto <bb 756>; [100.00%]

  <bb 751> [0.29%]:
  if (ret_1921 == 0)
    goto <bb 752>; [5.00%]
  else
    goto <bb 753>; [95.00%]

  <bb 752> [0.01%]:
  _1924 = (long int) bufr_1923;
  goto <bb 756>; [100.00%]

  <bb 753> [0.28%]:
  _1925 = (long unsigned int) ret_1921;
  bufr_1926 = bufr_1923 + _1925;
  if (inSize_1876 == bufr_1926)
    goto <bb 754>; [5.00%]
  else
    goto <bb 755>; [95.00%]

  <bb 754> [0.01%]:
  _1927 = (long int) blockSize_399;
  goto <bb 756>; [100.00%]

  <bb 755> [0.27%]:
  rsize_1928 = rsize_1912 - ret_1921;
  buf_1929 = buf_1914 + _1925;
  goto <bb 748>; [100.00%]

  <bb 756> [0.04%]:
  # _1930 = PHI <_1922(750), _1924(752), _1927(754)>
  ret_1879 = (int) _1930;
  if (ret_1879 == 0)
    goto <bb 757>; [2.50%]
  else
    goto <bb 773>; [97.50%]

  <bb 757> [0.00%]:
  operator delete [] (FileData_1877);
  close (hInfile_390);

  <bb 758> [0.00%]:
  _1931 = ProducerDoneMutex;
  ret_1933 = pthread_mutex_lock (_1931);
  if (ret_1933 != 0)
    goto <bb 759>; [0.04%]
  else
    goto <bb 761>; [99.96%]

  <bb 759> [0.00%]:
  _1934 = stderr;
  __fprintf_chk (_1934, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1933);

  <bb 760> [0.00%]:
  cleanupAndQuit (-5);

  <bb 761> [0.00%]:
  producerDone = 1;
  _1932 = ProducerDoneMutex;
  ret_1935 = pthread_mutex_unlock (_1932);
  if (ret_1935 != 0)
    goto <bb 762>; [0.04%]
  else
    goto <bb 764>; [99.96%]

  <bb 762> [0.00%]:
  _1936 = stderr;
  __fprintf_chk (_1936, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1935);

  <bb 763> [0.00%]:
  cleanupAndQuit (-6);

  <bb 764> [0.00%]:
  _1880 = _1563->mut;
  ret_1937 = pthread_mutex_lock (_1880);
  if (ret_1937 != 0)
    goto <bb 765>; [0.04%]
  else
    goto <bb 767>; [99.96%]

  <bb 765> [0.00%]:
  _1938 = stderr;
  __fprintf_chk (_1938, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1937);

  <bb 766> [0.00%]:
  cleanupAndQuit (-5);

  <bb 767> [0.00%]:
  _1881 = _1563->notEmpty;
  ret_1939 = pthread_cond_broadcast (_1881);
  if (ret_1939 != 0)
    goto <bb 768>; [0.04%]
  else
    goto <bb 770>; [99.96%]

  <bb 768> [0.00%]:
  _1940 = stderr;
  __fprintf_chk (_1940, 1, "pthread_cond_broadcast error [%d]! Aborting immediately!\n", ret_1939);

  <bb 769> [0.00%]:
  cleanupAndQuit (-7);

  <bb 770> [0.00%]:
  _1882 = _1563->mut;
  ret_1941 = pthread_mutex_unlock (_1882);
  if (ret_1941 != 0)
    goto <bb 771>; [0.04%]
  else
    goto <bb 820>; [99.96%]

  <bb 771> [0.00%]:
  _1942 = stderr;
  __fprintf_chk (_1942, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1941);

  <bb 772> [0.00%]:
  cleanupAndQuit (-6);

  <bb 773> [0.04%]:
  if (ret_1879 < 0)
    goto <bb 774>; [2.50%]
  else
    goto <bb 776>; [97.50%]

  <bb 774> [0.00%]:
  close (hInfile_390);

  <bb 775> [0.00%]:
  operator delete [] (FileData_1877);
  handle_error (1, -1, "pbzip2: *ERROR: Could not read from file!  Aborting...\n");
  goto <bb 820>; [100.00%]

  <bb 776> [0.04%]:
  _1883 = (long unsigned int) ret_1879;
  if (blockSize_399 != ret_1879)
    goto <bb 777>; [66.00%]
  else
    goto <bb 778>; [34.00%]

  <bb 777> [0.03%]:

  <bb 778> [0.04%]:
  # inSize_1892 = PHI <inSize_1876(776), _1883(777)>
  _1884 = _1563->mut;
  ret_1943 = pthread_mutex_lock (_1884);
  if (ret_1943 != 0)
    goto <bb 779>; [0.04%]
  else
    goto <bb 781>; [99.96%]

  <bb 779> [0.00%]:
  _1944 = stderr;
  __fprintf_chk (_1944, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1943);

  <bb 780> [0.00%]:
  cleanupAndQuit (-5);

  <bb 781> [0.29%]:
  _1885 = _1563->full;
  if (_1885 == 0)
    goto <bb 794>; [7.50%]
  else
    goto <bb 782>; [92.50%]

  <bb 782> [0.27%]:
  _1886 = _1563->mut;
  _1887 = _1563->notFull;
  _1945 = pthread_cond_wait (_1887, _1886);

  <bb 783> [0.27%]:
  if (_1945 != 0)
    goto <bb 784>; [0.04%]
  else
    goto <bb 786>; [99.96%]

  <bb 784> [0.00%]:
  _1946 = stderr;
  __fprintf_chk (_1946, 1, "pthread_cond_wait error [%d]! Aborting immediately!\n", _1945);

  <bb 785> [0.00%]:
  pthread_mutex_unlock (_1886);
  cleanupAndQuit (-8);

  <bb 786> [0.27%]:
  ret_1947 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_1947 != 0)
    goto <bb 787>; [0.04%]
  else
    goto <bb 789>; [99.96%]

  <bb 787> [0.00%]:
  _1948 = stderr;
  __fprintf_chk (_1948, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1947);

  <bb 788> [0.00%]:
  cleanupAndQuit (-5);

  <bb 789> [0.27%]:
  ret_1888 = terminateFlag;
  ret_1949 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_1949 != 0)
    goto <bb 790>; [0.04%]
  else
    goto <bb 792>; [99.96%]

  <bb 790> [0.00%]:
  _1950 = stderr;
  __fprintf_chk (_1950, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1949);

  <bb 791> [0.00%]:
  cleanupAndQuit (-6);

  <bb 792> [0.27%]:
  if (ret_1888 != 0)
    goto <bb 793>; [7.50%]
  else
    goto <bb 781>; [92.50%]

  <bb 793> [0.02%]:
  _1889 = _1563->mut;
  pthread_mutex_unlock (_1889);
  close (hInfile_390);
  goto <bb 820>; [100.00%]

  <bb 794> [0.02%]:
  _1890 = operator new (40, &nothrow);
  if (_1890 != 0B)
    goto <bb 795>; [70.00%]
  else
    goto <bb 796>; [30.00%]

  <bb 795> [0.02%]:
  _1891 = NumBlocks;
  _1893 = (unsigned int) inSize_1892;
  MEM[(struct  &)_1890] ={v} {CLOBBER};
  MEM[(struct outBuff *)_1890].buf = FileData_1877;
  MEM[(struct outBuff *)_1890].bufSize = _1893;
  MEM[(struct outBuff *)_1890].blockNumber = _1891;
  MEM[(struct outBuff *)_1890].sequenceNumber = 0;
  MEM[(struct outBuff *)_1890].inSize = 0;
  MEM[(struct outBuff *)_1890].isLastInSequence = 1;
  MEM[(struct outBuff *)_1890].next = 0B;

  <bb 796> [0.02%]:
  if (_1890 == 0B)
    goto <bb 797>; [2.50%]
  else
    goto <bb 799>; [97.50%]

  <bb 797> [0.00%]:
  close (hInfile_390);

  <bb 798> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (queueElement)!  Aborting...\n");
  goto <bb 820>; [100.00%]

  <bb 799> [0.02%]:
  _1953 = MEM[(struct ElementType *)_1890].sequenceNumber;
  if (_1953 > 1)
    goto <bb 800>; [64.00%]
  else
    goto <bb 801>; [36.00%]

  <bb 800> [0.01%]:
  _1954 = MEM[(struct queue *)_1563].lastElement;
  _1954->next = _1890;
  goto <bb 806>; [100.00%]

  <bb 801> [0.01%]:
  _1955 = MEM[(struct queue *)_1563].qData;
  _1956 = MEM[(struct queue *)_1563].tail;
  _1957 = (long unsigned int) _1956;
  _1958 = _1957 * 8;
  _1959 = _1955 + _1958;
  *_1959 = _1890;
  _1960 = MEM[(struct queue *)_1563].tail;
  _1961 = _1960 + 1;
  MEM[(struct queue *)_1563].tail = _1961;
  _1962 = MEM[(struct queue *)_1563].size;
  if (_1961 == _1962)
    goto <bb 802>; [34.00%]
  else
    goto <bb 803>; [66.00%]

  <bb 802> [0.00%]:
  MEM[(struct queue *)_1563].tail = 0;

  <bb 803> [0.01%]:
  _1963 = MEM[(struct queue *)_1563].tail;
  _1964 = MEM[(struct queue *)_1563].head;
  if (_1963 == _1964)
    goto <bb 804>; [34.00%]
  else
    goto <bb 805>; [66.00%]

  <bb 804> [0.00%]:
  MEM[(struct queue *)_1563].topLevelFull = 1;

  <bb 805> [0.01%]:
  MEM[(struct queue *)_1563].topLevelEmpty = 0;

  <bb 806> [0.02%]:
  MEM[(struct queue *)_1563].lastElement = _1890;
  _1965 = MEM[(struct queue *)_1563].count;
  _1966 = _1965 + 1;
  MEM[(struct queue *)_1563].count = _1966;
  _1967 = MEM[(struct queue *)_1563].size;
  if (_1966 == _1967)
    goto <bb 807>; [37.68%]
  else
    goto <bb 808>; [62.32%]

  <bb 807> [0.01%]:
  MEM[(struct queue *)_1563].full = 1;

  <bb 808> [0.02%]:
  MEM[(struct queue *)_1563].empty = 0;
  _1894 = _1563->notEmpty;
  ret_1951 = pthread_cond_signal (_1894);
  if (ret_1951 != 0)
    goto <bb 809>; [0.04%]
  else
    goto <bb 811>; [99.96%]

  <bb 809> [0.00%]:
  _1952 = stderr;
  __fprintf_chk (_1952, 1, "pthread_cond_signal error [%d]! Aborting immediately!\n", ret_1951);

  <bb 810> [0.00%]:
  cleanupAndQuit (-7);

  <bb 811> [0.02%]:
  ret_1968 = pthread_mutex_lock (&ProgressIndicatorsMutex);
  if (ret_1968 != 0)
    goto <bb 812>; [0.04%]
  else
    goto <bb 814>; [99.96%]

  <bb 812> [0.00%]:
  _1969 = stderr;
  __fprintf_chk (_1969, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_1968);

  <bb 813> [0.00%]:
  cleanupAndQuit (-5);

  <bb 814> [0.02%]:
  _1895 = NumBlocks;
  _1896 = _1895 + 1;
  NumBlocks = _1896;
  _1897 = InBytesProduced;
  _1898 = (long unsigned int) _1897;
  _1899 = inSize_1892 + _1898;
  _1900 = (long int) _1899;
  InBytesProduced = _1900;
  ret_1970 = pthread_mutex_unlock (&ProgressIndicatorsMutex);
  if (ret_1970 != 0)
    goto <bb 815>; [0.04%]
  else
    goto <bb 817>; [99.96%]

  <bb 815> [0.00%]:
  _1971 = stderr;
  __fprintf_chk (_1971, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1970);

  <bb 816> [0.00%]:
  cleanupAndQuit (-6);

  <bb 817> [0.02%]:
  _1901 = _1563->mut;
  ret_1972 = pthread_mutex_unlock (_1901);
  if (ret_1972 != 0)
    goto <bb 818>; [0.04%]
  else
    goto <bb 737>; [99.96%]

  <bb 818> [0.00%]:
  _1973 = stderr;
  __fprintf_chk (_1973, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_1972);

  <bb 819> [0.00%]:
  cleanupAndQuit (-6);

  <bb 820> [0.03%]:
  # _1902 = PHI <-1(744), -1(747), 0(770), -1(775), -1(793), -1(798)>
  if (_1902 != 0)
    goto <bb 821>; [50.00%]
  else
    goto <bb 884>; [50.00%]

  <bb 821> [0.01%]:
  goto <bb 884>; [100.00%]

  <bb 822> [0.05%]:
  _324 = QuietMode;
  if (_324 != 1)
    goto <bb 823>; [66.00%]
  else
    goto <bb 824>; [34.00%]

  <bb 823> [0.03%]:
  _325 = stderr;
  __builtin_fwrite ("Compressing data (no threads)...\n", 1, 33, _325);

  <bb 824> [0.05%]:
  _908 = outFilename._M_dataplus._M_p;
  _326 = InFileSize;
  outSize = 0;
  _1974 = OutputStdOut;
  if (_1974 == 0)
    goto <bb 825>; [33.00%]
  else
    goto <bb 828>; [67.00%]

  <bb 825> [0.02%]:
  _1975 = safe_open_output (_908);

  <bb 826> [0.02%]:
  if (_1975 == -1)
    goto <bb 827>; [0.52%]
  else
    goto <bb 828>; [99.48%]

  <bb 827> [0.00%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not create output file [%s]!\n", _908);
  goto <bb 882>; [100.00%]

  <bb 828> [0.05%]:
  # hOutfile_1979 = PHI <1(824), _1975(826)>

  <bb 829> [0.33%]:
  # CompressedSize_2008 = PHI <0(828), CompressedSize_2009(870)>
  # bytesLeft_1976 = PHI <_326(828), bytesLeft_1990(870)>
  # percentComplete_2016 = PHI <0(828), percentComplete_2014(870)>
  # currBlock_2010 = PHI <0(828), currBlock_2011(870)>
  if (bytesLeft_1976 <= 0)
    goto <bb 871>; [2.00%]
  else
    goto <bb 830>; [98.00%]

  <bb 830> [0.33%]:
  ret_2024 = pthread_mutex_lock (&TerminateFlagMutex);
  if (ret_2024 != 0)
    goto <bb 831>; [0.04%]
  else
    goto <bb 833>; [99.96%]

  <bb 831> [0.00%]:
  _2025 = stderr;
  __fprintf_chk (_2025, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_2024);

  <bb 832> [0.00%]:
  cleanupAndQuit (-5);

  <bb 833> [0.33%]:
  ret_1977 = terminateFlag;
  ret_2026 = pthread_mutex_unlock (&TerminateFlagMutex);
  if (ret_2026 != 0)
    goto <bb 834>; [0.04%]
  else
    goto <bb 836>; [99.96%]

  <bb 834> [0.00%]:
  _2027 = stderr;
  __fprintf_chk (_2027, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_2026);

  <bb 835> [0.00%]:
  cleanupAndQuit (-6);

  <bb 836> [0.33%]:
  if (ret_1977 != 0)
    goto <bb 837>; [2.00%]
  else
    goto <bb 841>; [98.00%]

  <bb 837> [0.01%]:
  close (hInfile_390);

  <bb 838> [0.01%]:
  _1978 = OutputStdOut;
  if (_1978 == 0)
    goto <bb 839>; [36.64%]
  else
    goto <bb 840>; [63.36%]

  <bb 839> [0.00%]:
  close (hOutfile_1979);

  <bb 840> [0.01%]:
  _1980 = stderr;
  __builtin_fwrite ("directcompress: terminating - terminateFlag set\n", 1, 48, _1980);
  goto <bb 882>; [100.00%]

  <bb 841> [0.32%]:
  _1981 = (long int) blockSize_399;
  inSize_1784 = MIN_EXPR <bytesLeft_1976, _1981>;
  _1983 = (sizetype) inSize_1784;
  FileData_1984 = operator new [] (_1983, &nothrow);
  if (FileData_1984 == 0B)
    goto <bb 842>; [2.00%]
  else
    goto <bb 846>; [98.00%]

  <bb 842> [0.01%]:
  close (hInfile_390);

  <bb 843> [0.01%]:
  _1985 = OutputStdOut;
  if (_1985 == 0)
    goto <bb 844>; [36.64%]
  else
    goto <bb 845>; [63.36%]

  <bb 844> [0.00%]:
  close (hOutfile_1979);

  <bb 845> [0.01%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (FileData)!  Aborting...\n");
  goto <bb 882>; [100.00%]

  <bb 846> [0.31%]:
  _1986 = do_read (hInfile_390, FileData_1984, _1983);

  <bb 847> [0.31%]:
  rret_1987 = (int) _1986;
  if (rret_1987 == 0)
    goto <bb 848>; [2.00%]
  else
    goto <bb 849>; [98.00%]

  <bb 848> [0.01%]:
  operator delete [] (FileData_1984);
  goto <bb 871>; [100.00%]

  <bb 849> [0.31%]:
  if (rret_1987 < 0)
    goto <bb 850>; [2.00%]
  else
    goto <bb 854>; [98.00%]

  <bb 850> [0.01%]:
  close (hInfile_390);

  <bb 851> [0.01%]:
  operator delete [] (FileData_1984);
  _1988 = OutputStdOut;
  if (_1988 == 0)
    goto <bb 852>; [36.64%]
  else
    goto <bb 853>; [63.36%]

  <bb 852> [0.00%]:
  close (hOutfile_1979);

  <bb 853> [0.01%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not read from file!  Aborting...\n");
  goto <bb 882>; [100.00%]

  <bb 854> [0.30%]:
  _1989 = (long int) rret_1987;
  bytesLeft_1990 = bytesLeft_1976 - _1989;
  _1991 = (double) inSize_1784;
  _1992 = _1991 * 1.0100000000000000088817841970012523233890533447265625e+0;
  _1993 = _1992 + 6.0e+2;
  _1994 = (int) _1993;
  _1995 = (unsigned int) _1994;
  outSize = _1995;
  _1996 = (sizetype) _1995;
  CompressedData_1997 = operator new [] (_1996, &nothrow);
  if (CompressedData_1997 == 0B)
    goto <bb 855>; [2.00%]
  else
    goto <bb 857>; [98.00%]

  <bb 855> [0.01%]:
  close (hInfile_390);

  <bb 856> [0.01%]:
  operator delete [] (FileData_1984);
  handle_error (1, -1, "pbzip2: *ERROR: Could not allocate memory (CompressedData)!  Aborting...\n");
  goto <bb 882>; [100.00%]

  <bb 857> [0.29%]:
  _1999 = BWTblockSize;
  _2000 = (unsigned int) inSize_1784;
  _2001 = BZ2_bzBuffToBuffCompress (CompressedData_1997, &outSize, FileData_1984, _2000, _1999, 0, 30);

  <bb 858> [0.29%]:
  if (_2001 != 0)
    goto <bb 859>; [2.00%]
  else
    goto <bb 861>; [98.00%]

  <bb 859> [0.01%]:
  close (hInfile_390);

  <bb 860> [0.01%]:
  operator delete [] (FileData_1984);
  handle_error (1, -1, "pbzip2: *ERROR during compression: %d!  Aborting...\n", _2001);
  goto <bb 882>; [100.00%]

  <bb 861> [0.29%]:
  _2002 = outSize;
  _2003 = (long unsigned int) _2002;
  _2004 = do_write (hOutfile_1979, CompressedData_1997, _2003);

  <bb 862> [0.29%]:
  ret_2005 = (int) _2004;
  if (ret_2005 <= 0)
    goto <bb 863>; [2.00%]
  else
    goto <bb 867>; [98.00%]

  <bb 863> [0.01%]:
  close (hInfile_390);

  <bb 864> [0.01%]:
  operator delete [] (FileData_1984);
  operator delete [] (CompressedData_1997);
  _2006 = OutputStdOut;
  if (_2006 == 0)
    goto <bb 865>; [36.64%]
  else
    goto <bb 866>; [63.36%]

  <bb 865> [0.00%]:
  close (hOutfile_1979);

  <bb 866> [0.01%]:
  handle_error (1, -1, "pbzip2: *ERROR: Could not write to file!  Aborting...\n");
  goto <bb 882>; [100.00%]

  <bb 867> [0.28%]:
  _2007 = (long int) ret_2005;
  CompressedSize_2009 = _2007 + CompressedSize_2008;
  currBlock_2011 = currBlock_2010 + 1;
  _2012 = currBlock_2011 * 100;
  _2013 = NumBlocksEstimated;
  percentComplete_2014 = _2012 / _2013;
  _2015 = QuietMode;
  _2063 = percentComplete_2014 != percentComplete_2016;
  _1096 = _2015 != 1;
  _1119 = _1096 & _2063;
  if (_1119 != 0)
    goto <bb 868>; [32.26%]
  else
    goto <bb 870>; [67.74%]

  <bb 868> [0.09%]:
  _2017 = stderr;
  __fprintf_chk (_2017, 1, "Completed: %d%%             \r", percentComplete_2014);

  <bb 869> [0.09%]:
  _2018 = stderr;
  fflush (_2018);

  <bb 870> [0.28%]:
  operator delete [] (FileData_1984);
  operator delete [] (CompressedData_1997);
  goto <bb 829>; [100.00%]

  <bb 871> [0.01%]:
  close (hInfile_390);

  <bb 872> [0.01%]:
  _2019 = OutputStdOut;
  if (_2019 == 0)
    goto <bb 873>; [33.00%]
  else
    goto <bb 874>; [67.00%]

  <bb 873> [0.00%]:
  close (hOutfile_1979);

  <bb 874> [0.01%]:
  _2020 = QuietMode;
  if (_2020 != 1)
    goto <bb 875>; [48.88%]
  else
    goto <bb 876>; [51.12%]

  <bb 875> [0.01%]:
  _2021 = (long unsigned int) CompressedSize_2008;
  _2022 = stderr;
  __fprintf_chk (_2022, 1, "    Output Size: %lu bytes\n", _2021);

  <bb 876> [0.01%]:
  _2028 = ProducerDoneMutex;
  ret_2030 = pthread_mutex_lock (_2028);
  if (ret_2030 != 0)
    goto <bb 877>; [0.04%]
  else
    goto <bb 879>; [99.96%]

  <bb 877> [0.00%]:
  _2031 = stderr;
  __fprintf_chk (_2031, 1, "pthread_mutex_lock error [%d]! Aborting immediately!\n", ret_2030);

  <bb 878> [0.00%]:
  cleanupAndQuit (-5);

  <bb 879> [0.01%]:
  producerDone = 1;
  _2029 = ProducerDoneMutex;
  ret_2032 = pthread_mutex_unlock (_2029);
  if (ret_2032 != 0)
    goto <bb 880>; [0.04%]
  else
    goto <bb 882>; [99.96%]

  <bb 880> [0.00%]:
  _2033 = stderr;
  __fprintf_chk (_2033, 1, "pthread_mutex_unlock error [%d]! Aborting immediately!\n", ret_2032);

  <bb 881> [0.00%]:
  cleanupAndQuit (-6);

  <bb 882> [0.05%]:
  # _2023 = PHI <-1(827), -1(840), -1(845), -1(853), -1(856), -1(860), -1(866), 0(879)>
  outSize ={v} {CLOBBER};
  if (_2023 != 0)
    goto <bb 883>; [50.00%]
  else
    goto <bb 884>; [50.00%]

  <bb 883> [0.02%]:

  <bb 884> [0.11%]:
  # errLevel_429 = PHI <errLevel_428(575), errLevel_428(716), 1(717), errLevel_426(820), 1(821), errLevel_426(882), 1(883)>
  # noThreads_442 = PHI <noThreads_441(575), noThreads_441(716), noThreads_441(717), noThreads_440(820), noThreads_440(821), noThreads_440(882), noThreads_440(883)>
  # switchedMtToSt_470 = PHI <switchedMtToSt_469(575), switchedMtToSt_469(716), switchedMtToSt_469(717), 0(820), 0(821), 0(882), 0(883)>
  # errLevelCurrentFile_472 = PHI <errLevelCurrentFile_471(575), errLevelCurrentFile_471(716), 1(717), 0(820), 1(821), 0(882), 1(883)>
  if (noThreads_442 == 0)
    goto <bb 885>; [33.00%]
  else
    goto <bb 888>; [67.00%]

  <bb 885> [0.04%]:
  _327 = output;
  _822 = pthread_join (_327, 0B);

  <bb 886> [0.04%]:
  if (_822 != 0)
    goto <bb 887>; [33.00%]
  else
    goto <bb 888>; [67.00%]

  <bb 887> [0.01%]:
  _328 = __errno_location ();
  _329 = *_328;
  _330 = stderr;
  printErrnoMsg (_330, _329);

  <bb 888> [0.11%]:
  # errLevel_430 = PHI <errLevel_429(884), errLevel_429(886), 1(887)>
  # errLevelCurrentFile_473 = PHI <errLevelCurrentFile_472(884), errLevelCurrentFile_472(886), 1(887)>
  _331 = noThreads_442 == 0;
  _332 = _331 | switchedMtToSt_470;
  if (_332 != 0)
    goto <bb 889>; [83.00%]
  else
    goto <bb 894>; [17.00%]

  <bb 889> [0.63%]:
  # errLevel_431 = PHI <errLevel_430(888), errLevel_432(893)>
  # i_462 = PHI <0(888), i_827(893)>
  # errLevelCurrentFile_474 = PHI <errLevelCurrentFile_473(888), errLevelCurrentFile_475(893)>
  _333 = (int) i_462;
  _334 = numCPU;
  if (_333 >= _334)
    goto <bb 894>; [15.00%]
  else
    goto <bb 890>; [85.00%]

  <bb 890> [0.53%]:
  _335 = _1563->consumers;
  _336 = i_462 * 8;
  _337 = _335 + _336;
  _338 = *_337;
  _825 = pthread_join (_338, 0B);

  <bb 891> [0.53%]:
  if (_825 != 0)
    goto <bb 892>; [33.00%]
  else
    goto <bb 893>; [67.00%]

  <bb 892> [0.18%]:
  _339 = __errno_location ();
  _340 = *_339;
  _341 = stderr;
  printErrnoMsg (_341, _340);

  <bb 893> [0.53%]:
  # errLevel_432 = PHI <errLevel_431(891), 1(892)>
  # errLevelCurrentFile_475 = PHI <errLevelCurrentFile_474(891), 1(892)>
  i_827 = i_462 + 1;
  goto <bb 889>; [100.00%]

  <bb 894> [0.11%]:
  # errLevel_433 = PHI <errLevel_430(888), errLevel_431(889)>
  # errLevelCurrentFile_476 = PHI <errLevelCurrentFile_473(888), errLevelCurrentFile_474(889)>
  safe_mutex_lock (&TerminateFlagMutex);

  <bb 895> [0.11%]:
  ret_909 = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);

  <bb 896> [0.11%]:
  if (ret_909 != 0)
    goto <bb 897>; [50.00%]
  else
    goto <bb 898>; [50.00%]

  <bb 897> [0.06%]:

  <bb 898> [0.11%]:
  # errLevel_434 = PHI <errLevel_433(896), 1(897)>
  # errLevelCurrentFile_477 = PHI <errLevelCurrentFile_476(896), 1(897)>
  _342 = OutputStdOut;
  if (_342 == 0)
    goto <bb 899>; [33.00%]
  else
    goto <bb 902>; [67.00%]

  <bb 899> [0.04%]:
  _910 = outFilename._M_dataplus._M_p;
  _830 = writeFileMetaData (_910);

  <bb 900> [0.04%]:
  if (_830 != 0)
    goto <bb 901>; [33.00%]
  else
    goto <bb 902>; [67.00%]

  <bb 901> [0.01%]:
  _911 = outFilename._M_dataplus._M_p;
  handle_error (0, -1, "pbzip2: *ERROR: Could not write file meta data to [%s]!\n", _911);

  <bb 902> [0.11%]:
  _343 = keep_452 != 1;
  _344 = errLevelCurrentFile_477 == 0;
  _345 = _343 & _344;
  if (_345 != 0)
    goto <bb 903>; [50.00%]
  else
    goto <bb 909>; [50.00%]

  <bb 903> [0.06%]:
  _346 = OutputStdOut;
  if (_346 != 0)
    goto <bb 905>; [50.00%]
  else
    goto <bb 904>; [50.00%]

  <bb 904> [0.03%]:
  _912 = outFilename._M_dataplus._M_p;
  _881 = __xstat (1, _912, &statbuf);
  if (_881 == 0)
    goto <bb 905>; [50.00%]
  else
    goto <bb 908>; [50.00%]

  <bb 905> [0.04%]:
  _833 = do_remove (InFilename_685);

  <bb 906> [0.04%]:
  if (_833 == -1)
    goto <bb 907>; [20.24%]
  else
    goto <bb 908>; [79.76%]

  <bb 907> [0.01%]:
  handle_error (0, 1, "Can\'t remove input file [%s]!", InFilename_685);

  <bb 908> [0.06%]:
  statbuf ={v} {CLOBBER};

  <bb 909> [0.11%]:
  _913 = OutputBuffer.D.4140._M_impl._M_start;
  OutputBuffer.D.4140._M_impl._M_finish = _913;
  MEM[(struct queue *)_1563].empty = 1;
  MEM[(struct queue *)_1563].full = 0;
  MEM[(struct queue *)_1563].head = 0;
  MEM[(struct queue *)_1563].tail = 0;
  MEM[(struct queue *)_1563].count = 0;
  MEM[(struct queue *)_1563].lastElement = 0B;
  MEM[(struct queue *)_1563].topLevelFull = 0;
  MEM[(struct queue *)_1563].topLevelEmpty = 1;
  if (errLevelCurrentFile_477 == 0)
    goto <bb 910>; [33.00%]
  else
    goto <bb 915>; [67.00%]

  <bb 910> [0.04%]:
  safe_mutex_lock (&TerminateFlagMutex);

  <bb 911> [0.04%]:
  ret_914 = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);

  <bb 912> [0.04%]:
  if (ret_914 == 0)
    goto <bb 913>; [33.00%]
  else
    goto <bb 915>; [67.00%]

  <bb 913> [0.01%]:
  safe_mutex_lock (&ErrorHandlerMutex);

  <bb 914> [0.01%]:
  sigInFilename = 0B;
  sigOutFilename = 0B;
  safe_mutex_unlock (&ErrorHandlerMutex);

  <bb 915> [0.11%]:
  if (errLevelCurrentFile_477 == 1)
    goto <bb 916>; [2.00%]
  else
    goto <bb 917>; [98.00%]

  <bb 916> [0.00%]:
  syncSetTerminateFlag (1);
  goto <bb 920>; [100.00%]

  <bb 917> [0.11%]:
  _347 = QuietMode;
  if (_347 != 1)
    goto <bb 918>; [48.88%]
  else
    goto <bb 919>; [51.12%]

  <bb 918> [0.05%]:
  _348 = stderr;
  __builtin_fwrite ("-------------------------------------------\n", 1, 44, _348);

  <bb 919> [1.33%]:
  # zeroByteFile_389 = PHI <zeroByteFile_385(300), zeroByteFile_385(352), zeroByteFile_385(362), zeroByteFile_387(388), zeroByteFile_388(399), zeroByteFile_388(450), zeroByteFile_388(917), zeroByteFile_388(918), zeroByteFile_388(444), zeroByteFile_385(299)>
  # blockSize_400 = PHI <blockSize_398(300), blockSize_398(352), blockSize_398(362), blockSize_398(388), blockSize_398(399), blockSize_399(450), blockSize_399(917), blockSize_399(918), blockSize_399(444), blockSize_398(299)>
  # errLevel_435 = PHI <errLevel_427(300), 1(352), 1(362), 1(388), 1(399), errLevel_426(450), errLevel_434(917), errLevel_434(918), errLevel_426(444), errLevel_427(299)>
  # noThreads_443 = PHI <noThreads_437(300), noThreads_437(352), noThreads_437(362), noThreads_437(388), noThreads_437(399), noThreads_439(450), noThreads_442(917), noThreads_442(918), noThreads_439(444), noThreads_437(299)>
  fileLoop_845 = fileLoop_458 + 1;
  goto <bb 219>; [100.00%]

  <bb 920> [0.03%]:
  # errLevel_436 = PHI <errLevel_426(219), 1(431), 1(440), errLevel_434(916)>
  _349 = OutputStdOut;
  if (_349 == 1)
    goto <bb 921>; [34.00%]
  else
    goto <bb 926>; [66.00%]

  <bb 921> [0.01%]:
  _847 = close (1);

  <bb 922> [0.01%]:
  if (_847 == -1)
    goto <bb 923>; [0.04%]
  else
    goto <bb 926>; [99.96%]

  <bb 923> [0.00%]:
  _849 = getInstance ();

  <bb 924> [0.00%]:
  saveError (_849);
  handle_error (1, 1, "pbzip2: *ERROR: Failed to close STDOUT! Aborting...\n");

  <bb 925> [0.00%]:
  exit (1);

  <bb 926> [0.03%]:
  _351 = SignalHandlerThread;
  ret_854 = pthread_kill (_351, 10);
  if (ret_854 != 0)
    goto <bb 927>; [0.04%]
  else
    goto <bb 929>; [99.96%]

  <bb 927> [0.00%]:
  _352 = stderr;
  __fprintf_chk (_352, 1, "Couldn\'t signal signal QUIT to SignalHandlerThread [%d]. Quitting prematurely!\n", ret_854);

  <bb 928> [0.00%]:
  exit (errLevel_436);

  <bb 929> [0.03%]:
  _353 = SignalHandlerThread;
  _856 = pthread_join (_353, 0B);

  <bb 930> [0.03%]:
  if (_856 != 0)
    goto <bb 931>; [50.00%]
  else
    goto <bb 932>; [50.00%]

  <bb 931> [0.02%]:
  _354 = stderr;
  __fprintf_chk (_354, 1, "Error on join of SignalHandlerThread [%d]\n", _856);

  <bb 932> [0.03%]:
  safe_mutex_lock (&TerminateFlagMutex);

  <bb 933> [0.03%]:
  ret_915 = terminateFlag;
  safe_mutex_unlock (&TerminateFlagMutex);

  <bb 934> [0.03%]:
  if (ret_915 == 0)
    goto <bb 935>; [50.00%]
  else
    goto <bb 937>; [50.00%]

  <bb 935> [0.02%]:
  safe_mutex_lock (&TerminateFlagMutex);

  <bb 936> [0.02%]:
  finishedFlag = 1;
  pthread_cond_signal (&TerminateCond);
  safe_mutex_unlock (&TerminateFlagMutex);

  <bb 937> [0.03%]:
  _355 = TerminatorThread;
  _859 = pthread_join (_355, 0B);

  <bb 938> [0.03%]:
  if (_859 != 0)
    goto <bb 939>; [50.00%]
  else
    goto <bb 940>; [50.00%]

  <bb 939> [0.02%]:
  _356 = stderr;
  __fprintf_chk (_356, 1, "Error on join of TerminatorThread [%d]\n", _859);

  <bb 940> [0.03%]:
  _2038 = _1563->mut;
  if (_2038 != 0B)
    goto <bb 941>; [53.47%]
  else
    goto <bb 942>; [46.53%]

  <bb 941> [0.01%]:
  pthread_mutex_destroy (_2038);
  _2039 = _1563->mut;
  operator delete (_2039, 40);
  _1563->mut = 0B;

  <bb 942> [0.02%]:
  _2040 = _1563->notFull;
  if (_2040 != 0B)
    goto <bb 943>; [53.47%]
  else
    goto <bb 944>; [46.53%]

  <bb 943> [0.01%]:
  pthread_cond_destroy (_2040);
  _2041 = _1563->notFull;
  operator delete (_2041, 48);
  _1563->notFull = 0B;

  <bb 944> [0.02%]:
  _2042 = _1563->notEmpty;
  if (_2042 != 0B)
    goto <bb 945>; [53.47%]
  else
    goto <bb 946>; [46.53%]

  <bb 945> [0.01%]:
  pthread_cond_destroy (_2042);
  _2043 = _1563->notEmpty;
  operator delete (_2043, 48);
  _1563->notEmpty = 0B;

  <bb 946> [0.02%]:
  _2044 = _1563->consumers;
  if (_2044 != 0B)
    goto <bb 947>; [53.47%]
  else
    goto <bb 948>; [46.53%]

  <bb 947> [0.01%]:
  operator delete [] (_2044);

  <bb 948> [0.02%]:
  _2045 = _1563->qData;
  if (_2045 != 0B)
    goto <bb 949>; [53.47%]
  else
    goto <bb 950>; [46.53%]

  <bb 949> [0.01%]:
  operator delete [] (_2045);

  <bb 950> [0.02%]:
  operator delete (_1563, 96);
  _2046 = notTooMuchNumBuffered;
  if (_2046 != 0B)
    goto <bb 951>; [57.44%]
  else
    goto <bb 952>; [42.56%]

  <bb 951> [0.01%]:
  pthread_cond_destroy (_2046);
  _2047 = notTooMuchNumBuffered;
  operator delete (_2047, 48);
  notTooMuchNumBuffered = 0B;

  <bb 952> [0.03%]:
  _2034 = OutMutex;
  if (_2034 != 0B)
    goto <bb 953>; [53.47%]
  else
    goto <bb 954>; [46.53%]

  <bb 953> [0.02%]:
  pthread_mutex_destroy (_2034);
  _2035 = OutMutex;
  operator delete (_2035, 40);
  OutMutex = 0B;

  <bb 954> [0.03%]:
  _2036 = ProducerDoneMutex;
  if (_2036 != 0B)
    goto <bb 955>; [57.44%]
  else
    goto <bb 956>; [42.56%]

  <bb 955> [0.02%]:
  pthread_mutex_destroy (_2036);
  _2037 = ProducerDoneMutex;
  operator delete (_2037, 40);
  ProducerDoneMutex = 0B;

  <bb 956> [0.03%]:
  operator delete [] (_8);
  gettimeofday (&tvStopTime, &tz);
  _357 = tvStartTime.tv_sec;
  _358 = (double) _357;
  _359 = tvStartTime.tv_usec;
  _360 = (double) _359;
  _361 = _360 / 1.0e+6;
  timeStart_863 = _358 + _361;
  _362 = tvStopTime.tv_sec;
  _363 = (double) _362;
  _364 = tvStopTime.tv_usec;
  _365 = (double) _364;
  _366 = _365 / 1.0e+6;
  timeStop_864 = _363 + _366;
  timeCalc_865 = timeStop_864 - timeStart_863;
  _367 = QuietMode;
  if (_367 != 1)
    goto <bb 957>; [66.00%]
  else
    goto <bb 958>; [34.00%]

  <bb 957> [0.02%]:
  _368 = stderr;
  __fprintf_chk (_368, 1, "\n     Wall Clock: %f seconds\n", timeCalc_865);

  <bb 958> [0.03%]:
  exit (errLevel_436);

  <bb 959> [17.97%]:
  # _479 = PHI <1(3), 1(26), 1(72), 1(74), 1(83), 1(92), 1(135), 1(140), 1(145), 1(153), 1(176), 1(178), _1552(204), 1(218), _1604(295), 1(505), 1(509), 1(572), 1(726), 1(730)>
  _916 = outFilename._M_dataplus._M_p;
  if (&outFilename.D.4158._M_local_buf != _916)
    goto <bb 960>; [57.44%]
  else
    goto <bb 961>; [42.56%]

  <bb 960> [10.32%]:
  operator delete (_916);

  <bb 961> [17.97%]:
  output ={v} {CLOBBER};
  bz2Header ={v} {CLOBBER};
  outFilename ={v} {CLOBBER};
  cmdLineTemp ={v} {CLOBBER};
  tmpBuff ={v} {CLOBBER};
  stdinFile ={v} {CLOBBER};
  tvStartTime ={v} {CLOBBER};
  tvStopTime ={v} {CLOBBER};
  tz ={v} {CLOBBER};
  loadAvgArray ={v} {CLOBBER};
  return _479;

<L27> [0.00%]:
  _917 = tbz2Tail._M_dataplus._M_p;
  if (&tbz2Tail.D.4158._M_local_buf != _917)
    goto <bb 963>; [53.47%]
  else
    goto <bb 964>; [46.53%]

  <bb 963> [0.00%]:
  operator delete (_917);

  <bb 964> [0.00%]:
  MEM[(struct  &)&tbz2Tail] ={v} {CLOBBER};
  resx 21

<L28> [0.00%]:
  D.5395 ={v} {CLOBBER};
  _918 = bz2Tail._M_dataplus._M_p;
  if (&bz2Tail.D.4158._M_local_buf != _918)
    goto <bb 966>; [53.47%]
  else
    goto <bb 967>; [46.53%]

  <bb 966> [0.00%]:
  operator delete (_918);

  <bb 967> [0.00%]:
  resx 15

<L29> [0.00%]:
  D.5398 ={v} {CLOBBER};
  _919 = bz2Tail._M_dataplus._M_p;
  if (&bz2Tail.D.4158._M_local_buf != _919)
    goto <bb 969>; [53.47%]
  else
    goto <bb 970>; [46.53%]

  <bb 969> [0.00%]:
  operator delete (_919);

  <bb 970> [0.00%]:
  resx 31

<L30> [0.00%]:
  statbuf ={v} {CLOBBER};
  statbuf ={v} {CLOBBER};
  statbuf ={v} {CLOBBER};
  D.5396 ={v} {CLOBBER};
  bz2Tail ={v} {CLOBBER};
  D.5394 ={v} {CLOBBER};
  bz2Tail ={v} {CLOBBER};
  tbz2Tail ={v} {CLOBBER};
  D.5393 ={v} {CLOBBER};
  _920 = outFilename._M_dataplus._M_p;
  if (&outFilename.D.4158._M_local_buf != _920)
    goto <bb 972>; [53.47%]
  else
    goto <bb 973>; [46.53%]

  <bb 972> [0.00%]:
  operator delete (_920);

  <bb 973> [0.00%]:
  resx 2

}

