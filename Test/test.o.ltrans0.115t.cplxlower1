
;; Function main (main, funcdef_no=0, decl_uid=3985, cgraph_uid=0, symbol_order=64) (executed once)

main ()
{
  struct cleanup * ptr;
  struct cleanup * new;
  struct cleanup * old_chain;
  struct cleanup * D.4116;
  struct cleanup * D.4101;
  struct cleanup * D.4092;
  int i;
  ULONGEST D.4085;
  void * newmem;
  void * D.4074;
  size_t size;
  int i;
  int ret;
  int D.4065;
  int ldi_size;
  void * ldi;
  int rc;
  gdb_byte * D.4048;
  gdb_byte * ldi_buf;
  ULONGEST result;
  struct cleanup * cleanup;
  target_xfer_status D.4039;
  int len;
  char buf[10];
  time_t t;
  int _1;
  unsigned int _2;
  int _3;
  int _4;
  gdb_byte * _5;
  target_xfer_status _17;
  long long int _20;
  long unsigned int _24;
  int _28;
  int _29;
  int _30;
  unsigned char _38;
  long unsigned int _40;
  struct cleanup * _45;
  struct cleanup * _47;
  void (*<Ta03>) (void *) _48;
  void * _49;
  int _50;
  void * _51;
  int _52;
  void (*<Ta03>) (void *) _53;
  void (*<Ta03>) (void *) _54;
  void * _55;

  <bb 2> [16.49%]:
  _1 = time (&t);
  _2 = (unsigned int) _1;
  srand (_2);
  _3 = rand ();
  _4 = _3 % 2;
  if (_4 == 1)
    goto <bb 4>; [30.50%]
  else
    goto <bb 3>; [69.50%]

  <bb 3> [11.46%]:

  <bb 4> [16.49%]:
  # _5 = PHI <&buf(2), 0B(3)>
  if (_5 != 0B)
    goto <bb 26>; [8.82%]
  else
    goto <bb 5>; [91.18%]

  <bb 5> [15.03%]:
  ldi_18 = malloc (1024);

  <bb 6> [100.00%]:
  # ldi_size_21 = PHI <1024(5), ldi_size_23(14)>
  # ldi_19 = PHI <ldi_18(5), newmem_36(14)>
  _20 = (long long int) ldi_19;
  i_27 = MEM[(int *)ldi_19];
  _28 = rand ();
  _29 = _28 % 2;
  if (_29 == 1)
    goto <bb 8>; [96.19%]
  else
    goto <bb 7>; [3.81%]

  <bb 7> [3.81%]:

  <bb 8> [100.00%]:
  # _30 = PHI <1(6), -1(7)>
  if (_30 != -1)
    goto <bb 16>; [15.00%]
  else
    goto <bb 9>; [85.00%]

  <bb 9> [85.00%]:
  ldi_size_23 = ldi_size_21 * 2;
  _24 = (long unsigned int) ldi_size_23;
  if (_24 == 0)
    goto <bb 10>; [50.00%]
  else
    goto <bb 11>; [50.00%]

  <bb 10> [42.50%]:

  <bb 11> [85.00%]:
  # size_33 = PHI <_24(9), 1(10)>
  if (ldi_19 == 0B)
    goto <bb 12>; [30.00%]
  else
    goto <bb 13>; [70.00%]

  <bb 12> [25.50%]:
  newmem_34 = malloc (size_33);
  goto <bb 14>; [100.00%]

  <bb 13> [59.50%]:
  newmem_35 = realloc (ldi_19, size_33);

  <bb 14> [85.00%]:
  # newmem_36 = PHI <newmem_34(12), newmem_35(13)>
  if (newmem_36 == 0B)
    goto <bb 15>; [0.04%]
  else
    goto <bb 6>; [99.96%]

  <bb 15> [0.03%]:
  exit (1);

  <bb 16> [15.00%]:
  if (ldi_19 == 0B)
    goto <bb 17>; [0.04%]
  else
    goto <bb 18>; [99.96%]

  <bb 17> [0.01%]:
  exit (1);

  <bb 18> [14.99%]:
  new_43 = malloc (32);
  old_chain_44 = cleanup_chain;
  _45 = cleanup_chain;
  new_43->next = _45;
  new_43->function = free;
  new_43->free_arg = 0B;
  new_43->arg = ldi_19;
  cleanup_chain = new_43;
  if (old_chain_44 == 0B)
    goto <bb 19>; [0.04%]
  else
    goto <bb 20>; [99.96%]

  <bb 19> [0.01%]:
  exit (1);

  <bb 20> [14.99%]:
  _38 = MEM[(const gdb_byte *)ldi_19];
  i_39 = (int) _38;
  _40 = (long unsigned int) _38;
  free (ldi_19);
  ptr_31 = cleanup_chain;
  if (old_chain_44 != ptr_31)
    goto <bb 21>; [85.00%]
  else
    goto <bb 24>; [15.00%]

  <bb 21> [84.93%]:
  # ptr_32 = PHI <ptr_46(23), ptr_31(20)>
  _47 = ptr_32->next;
  cleanup_chain = _47;
  _48 = ptr_32->function;
  _49 = ptr_32->arg;
  _48 (_49);
  _50 = __printf_chk (1, "cleanup!!!!! =========\n");
  _51 = ptr_32->arg;
  _52 = __printf_chk (1, "%x\n", _51);
  _53 = ptr_32->free_arg;
  if (_53 != 0B)
    goto <bb 22>; [93.48%]
  else
    goto <bb 23>; [6.52%]

  <bb 22> [79.39%]:
  _54 = ptr_32->free_arg;
  _55 = ptr_32->arg;
  _54 (_55);

  <bb 23> [84.93%]:
  free (ptr_32);
  ptr_46 = cleanup_chain;
  if (old_chain_44 != ptr_46)
    goto <bb 21>; [85.00%]
  else
    goto <bb 24>; [15.00%]

  <bb 24> [14.99%]:
  if (_40 == 0)
    goto <bb 26>; [46.00%]
  else
    goto <bb 25>; [54.00%]

  <bb 25> [8.09%]:
  MEM[(ULONGEST *)&len] = _40;

  <bb 26> [16.44%]:
  # _17 = PHI <-1(4), 0(24), 1(25)>
  t ={v} {CLOBBER};
  buf ={v} {CLOBBER};
  len ={v} {CLOBBER};
  return 0;

}


