
;; Function newentry (newentry, funcdef_no=0, decl_uid=4000, cgraph_uid=5, symbol_order=60)

newentry (char * word)
{
  struct dict_type * new_d;
  struct dict_type * _1;
  void * _2;

  <bb 2> [100.00%]:
  new_d_5 = malloc (40);
  new_d_5->word = word_6(D);
  _1 = root;
  new_d_5->next = _1;
  root = new_d_5;
  _2 = malloc (8);
  new_d_5->code = _2;
  new_d_5->code_length = 1;
  new_d_5->code_end = 0;
  return new_d_5;

}



;; Function nextword (nextword, funcdef_no=1, decl_uid=3990, cgraph_uid=4, symbol_order=63)

nextword (char * string, char * * word)
{
  int length;
  char * src;
  char * dst;
  int idx;
  char * word_start;
  char _1;
  _Bool _2;
  _Bool _3;
  _Bool _4;
  char _5;
  unsigned char _6;
  int _7;
  int _8;
  char _9;
  char _10;
  char _11;
  char _12;
  char _13;
  char _14;
  unsigned char _15;
  int _16;
  int _17;
  int _18;
  long unsigned int _19;
  void * _20;
  sizetype _21;
  char * _22;
  char _23;
  sizetype _24;
  sizetype _25;
  char * _26;
  char _27;
  int _28;
  char _29;
  sizetype _30;
  sizetype _31;
  char * _32;
  _Bool _33;
  char _34;
  _Bool _35;
  sizetype _36;
  char * _37;
  char _39;
  char _41;
  char * _56;
  char * _77;
  char _90;
  _Bool _103;

  <bb 2> [6.05%]:
  if (string_61(D) == 0B)
    goto <bb 27>; [4.07%]
  else
    goto <bb 8>; [95.93%]

  <bb 3> [71.53%]:
  _1 = *string_43;
  if (_1 == 45)
    goto <bb 4>; [20.97%]
  else
    goto <bb 6>; [79.03%]

  <bb 4> [15.00%]:
  # string_38 = PHI <string_43(3)>
  _29 = *string_38;
  _33 = _29 != 0;
  _35 = _29 != 10;
  _103 = _33 & _35;
  if (_103 != 0)
    goto <bb 5>; [85.00%]
  else
    goto <bb 7>; [15.00%]

  <bb 5> [85.00%]:
  # string_104 = PHI <string_91(5), string_38(4)>
  string_91 = string_104 + 1;
  _90 = *string_91;
  _2 = _90 != 0;
  _3 = _90 != 10;
  _4 = _2 & _3;
  if (_4 != 0)
    goto <bb 5>; [85.00%]
  else
    goto <bb 7>; [15.00%]

  <bb 6> [56.53%]:
  string_89 = string_43 + 1;

  <bb 7> [71.53%]:
  # string_60 = PHI <string_91(5), string_89(6), string_38(4)>

  <bb 8> [77.33%]:
  # string_43 = PHI <string_60(7), string_61(D)(2)>
  _5 = *string_43;
  _6 = (unsigned char) _5;
  _7 = (int) _6;
  _8 = isspace (_7);
  if (_8 != 0)
    goto <bb 3>; [50.00%]
  else
    goto <bb 9>; [50.00%]

  <bb 9> [38.67%]:
  _9 = *string_43;
  if (_9 == 45)
    goto <bb 3>; [85.00%]
  else
    goto <bb 10>; [15.00%]

  <bb 10> [5.80%]:
  _10 = *string_43;
  if (_10 == 0)
    goto <bb 27>; [9.00%]
  else
    goto <bb 11>; [91.00%]

  <bb 11> [5.28%]:
  _11 = *string_43;
  if (_11 == 34)
    goto <bb 12>; [34.00%]
  else
    goto <bb 16>; [66.00%]

  <bb 12> [11.96%]:
  # string_44 = PHI <string_43(11), string_45(14)>
  # length_52 = PHI <0(11), length_53(14)>
  string_66 = string_44 + 1;
  length_67 = length_52 + 1;
  _12 = MEM[(char *)string_44 + 1B];
  if (_12 == 92)
    goto <bb 13>; [34.00%]
  else
    goto <bb 14>; [66.00%]

  <bb 13> [4.07%]:
  string_68 = &MEM[(void *)string_44 + 3B];
  length_69 = length_52 + 3;

  <bb 14> [11.96%]:
  # string_45 = PHI <string_66(12), string_68(13)>
  # length_53 = PHI <length_67(12), length_69(13)>
  _13 = *string_45;
  if (_13 != 34)
    goto <bb 12>; [85.00%]
  else
    goto <bb 17>; [15.00%]

  <bb 15> [19.74%]:
  string_64 = string_46 + 1;
  length_65 = length_54 + 1;

  <bb 16> [23.22%]:
  # string_46 = PHI <string_43(11), string_64(15)>
  # length_54 = PHI <0(11), length_65(15)>
  _14 = *string_46;
  _15 = (unsigned char) _14;
  _16 = (int) _15;
  _17 = isspace (_16);
  if (_17 == 0)
    goto <bb 15>; [85.00%]
  else
    goto <bb 17>; [15.00%]

  <bb 17> [5.28%]:
  # string_47 = PHI <string_45(14), string_46(16)>
  # length_55 = PHI <length_53(14), length_54(16)>
  _18 = length_55 + 1;
  _19 = (long unsigned int) _18;
  _20 = malloc (_19);
  *word_71(D) = _20;
  dst_73 = *word_71(D);
  if (length_55 > 0)
    goto <bb 18>; [85.00%]
  else
    goto <bb 25>; [15.00%]

  <bb 18> [29.90%]:
  # idx_105 = PHI <idx_88(24), 0(17)>
  # dst_106 = PHI <dst_50(24), dst_73(17)>
  _21 = (sizetype) idx_105;
  _22 = string_43 + _21;
  _23 = *_22;
  if (_23 == 92)
    goto <bb 19>; [34.00%]
  else
    goto <bb 23>; [66.00%]

  <bb 19> [10.17%]:
  _24 = (sizetype) idx_105;
  _25 = _24 + 1;
  _26 = string_43 + _25;
  _27 = *_26;
  _28 = (int) _27;
  switch (_27) <default: <L2> [33.33%], case 34: <L1> [33.33%], case 92: <L1> [33.33%], case 110: <L0> [33.33%]>

<L0> [3.39%]:
  dst_80 = dst_106 + 1;
  *dst_106 = 10;
  idx_82 = idx_105 + 1;
  goto <bb 24>; [100.00%]

<L1> [3.39%]:
  _30 = (sizetype) idx_105;
  _31 = _30 + 1;
  _32 = string_43 + _31;
  dst_83 = dst_106 + 1;
  _34 = *_32;
  *dst_106 = _34;
  idx_85 = idx_105 + 1;
  goto <bb 24>; [100.00%]

<L2> [3.39%]:
  dst_86 = dst_106 + 1;
  *dst_106 = 92;
  goto <bb 24>; [100.00%]

  <bb 23> [19.74%]:
  _36 = (sizetype) idx_105;
  _37 = string_43 + _36;
  dst_78 = dst_106 + 1;
  _39 = *_37;
  *dst_106 = _39;

  <bb 24> [29.90%]:
  # idx_48 = PHI <idx_82(20), idx_85(21), idx_105(22), idx_105(23)>
  # dst_50 = PHI <dst_80(20), dst_83(21), dst_86(22), dst_78(23)>
  idx_88 = idx_48 + 1;
  if (length_55 > idx_88)
    goto <bb 18>; [85.00%]
  else
    goto <bb 25>; [15.00%]

  <bb 25> [5.28%]:
  # dst_107 = PHI <dst_50(24), dst_73(17)>
  dst_75 = dst_107 + 1;
  *dst_107 = 0;
  _41 = *string_47;
  if (_41 != 0)
    goto <bb 26>; [91.00%]
  else
    goto <bb 27>; [9.00%]

  <bb 26> [4.80%]:
  _77 = string_47 + 1;

  <bb 27> [6.04%]:
  # _56 = PHI <0B(2), 0B(10), 0B(25), _77(26)>
  return _56;

}



;; Function main (main, funcdef_no=2, decl_uid=3985, cgraph_uid=0, symbol_order=65) (executed once)

main ()
{
  unsigned int D.4100;
  struct dict_type * new_d;
  char * word;
  struct dict_type * ptr;
  char * string;
  struct dict_type * dict;
  char * _1;
  char * _2;
  int _10;
  int _18;
  char * _19;
  int _20;
  char * _22;
  char * _24;
  char _25;
  char * _26;
  char * _28;
  char * _31;
  char _32;
  int _33;
  char * _34;
  char * _35;
  void (*<Ta03>) () _36;
  int _37;
  int _38;
  char * _39;
  int _40;
  int _41;
  char * _42;
  char * _45;
  char _46;
  int _47;
  int _48;
  char * _51;
  struct FILE * _52;
  int _53;
  char _54;
  char * _55;
  char _56;
  int _59;
  int _60;
  int _61;
  int _62;
  int _63;
  long unsigned int _64;
  long unsigned int _65;
  void (*stinst_type) () * _66;
  void * _67;
  void (*stinst_type) () * _68;
  int _69;
  long unsigned int _70;
  long unsigned int _71;
  void (*stinst_type) () * _72;
  int _73;
  int _74;
  unsigned int _75;
  char _108;
  char * _109;
  char _110;

  <bb 2> [14.30%]:
  string_16 = nextword ("var second : stored_in_dict 5 ; : next", &word);
  if (string_16 != 0B)
    goto <bb 3>; [95.00%]
  else
    goto <bb 22>; [5.00%]

  <bb 3> [13.59%]:
  _108 = *string_16;
  if (_108 != 0)
    goto <bb 4>; [95.00%]
  else
    goto <bb 22>; [5.00%]

  <bb 4> [12.91%]:
  _109 = word;
  _110 = *_109;
  if (_110 != 0)
    goto <bb 5>; [95.00%]
  else
    goto <bb 22>; [5.00%]

  <bb 5> [85.74%]:
  # string_114 = PHI <string_57(21), string_16(4)>
  _18 = __printf_chk (1, "string: %s\n", string_114);
  _19 = word;
  _20 = strcmp (_19, "var");
  if (_20 == 0)
    goto <bb 6>; [33.00%]
  else
    goto <bb 7>; [67.00%]

  <bb 6> [28.29%]:
  string_21 = nextword (string_114, &word);
  _22 = word;
  new_d_58 = newentry (_22);
  string_23 = nextword (string_21, &word);
  goto <bb 19>; [100.00%]

  <bb 7> [57.44%]:
  _24 = word;
  _25 = *_24;
  if (_25 == 58)
    goto <bb 8>; [20.97%]
  else
    goto <bb 18>; [79.03%]

  <bb 8> [12.05%]:
  _26 = word;
  free (_26);
  string_27 = nextword (string_114, &word);
  _28 = word;
  ptr_29 = newentry (_28);
  string_30 = nextword (string_27, &word);
  goto <bb 16>; [100.00%]

  <bb 9> [77.13%]:
  _31 = word;
  _32 = *_31;
  _33 = (int) _32;
  switch (_32) <default: <L2> [33.33%], case 34: <L0> [33.33%], case 48 ... 57: <L1> [33.33%]>

<L0> [25.71%]:
  _34 = word;
  _35 = _34 + 1;
  _36 = (void (*<Ta03>) ()) _35;
  _59 = ptr_29->code_end;
  _60 = ptr_29->code_length;
  if (_59 == _60)
    goto <bb 11>; [22.95%]
  else
    goto <bb 12>; [77.05%]

  <bb 11> [5.90%]:
  _61 = ptr_29->code_length;
  _62 = _61 + 2;
  ptr_29->code_length = _62;
  _63 = ptr_29->code_length;
  _64 = (long unsigned int) _63;
  _65 = _64 * 8;
  _66 = ptr_29->code;
  _67 = realloc (_66, _65);
  ptr_29->code = _67;

  <bb 12> [25.71%]:
  _68 = ptr_29->code;
  _69 = ptr_29->code_end;
  _70 = (long unsigned int) _69;
  _71 = _70 * 8;
  _72 = _68 + _71;
  *_72 = _36;
  _73 = ptr_29->code_end;
  _74 = _73 + 1;
  ptr_29->code_end = _74;
  _75 = (unsigned int) _73;
  goto <bb 15>; [100.00%]

<L1> [25.71%]:
  _37 = MEM[(int *)ptr_29];
  _38 = __printf_chk (1, "%d\n", _37);
  _39 = word;
  free (_39);
  goto <bb 15>; [100.00%]

<L2> [25.71%]:
  _40 = MEM[(int *)ptr_29];
  _41 = __printf_chk (1, "%d\n", _40);
  _42 = word;
  free (_42);

  <bb 15> [77.12%]:
  string_44 = nextword (string_43, &word);
  if (string_44 == 0B)
    goto <bb 17>; [7.50%]
  else
    goto <bb 16>; [92.50%]

  <bb 16> [83.39%]:
  # string_43 = PHI <string_30(8), string_44(15)>
  _45 = word;
  _46 = *_45;
  if (_46 != 59)
    goto <bb 9>; [92.50%]
  else
    goto <bb 17>; [7.50%]

  <bb 17> [12.04%]:
  # string_49 = PHI <string_44(15), string_43(16)>
  _47 = MEM[(int *)ptr_29];
  _48 = __printf_chk (1, "%d\n", _47);
  string_50 = nextword (string_49, &word);
  goto <bb 19>; [100.00%]

  <bb 18> [45.40%]:
  _51 = string_114 + 18446744073709551615;
  _52 = stderr;
  _53 = __fprintf_chk (_52, 1, "syntax error at %s\n", _51);

  <bb 19> [85.73%]:
  # string_57 = PHI <string_50(17), string_23(6), string_114(18)>
  if (string_57 != 0B)
    goto <bb 20>; [95.00%]
  else
    goto <bb 22>; [5.00%]

  <bb 20> [81.42%]:
  _54 = *string_57;
  if (_54 != 0)
    goto <bb 21>; [95.00%]
  else
    goto <bb 22>; [5.00%]

  <bb 21> [77.34%]:
  _55 = word;
  _56 = *_55;
  if (_56 != 0)
    goto <bb 5>; [95.00%]
  else
    goto <bb 22>; [5.00%]

  <bb 22> [14.26%]:
  word ={v} {CLOBBER};
  dict_7 = root;
  if (dict_7 != 0B)
    goto <bb 23>; [85.00%]
  else
    goto <bb 24>; [15.00%]

  <bb 23> [80.82%]:
  # dict_111 = PHI <dict_9(23), dict_7(22)>
  _1 = dict_111->word;
  _10 = __printf_chk (1, "%s\n", _1);
  _2 = dict_111->word;
  free (_2);
  dict_9 = dict_111->next;
  if (dict_9 != 0B)
    goto <bb 23>; [85.00%]
  else
    goto <bb 24>; [15.00%]

  <bb 24> [14.26%]:
  return 0;

}


