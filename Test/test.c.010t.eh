
;; Function discard_cleanups (discard_cleanups, funcdef_no=39, decl_uid=3103, cgraph_uid=39, symbol_order=44)

discard_cleanups (struct cleanup * old_chain)
{
  discard_my_cleanups (&cleanup_chain, old_chain);
  return;
}



;; Function discard_final_cleanups (discard_final_cleanups, funcdef_no=40, decl_uid=3108, cgraph_uid=40, symbol_order=45)

discard_final_cleanups (struct cleanup * old_chain)
{
  discard_my_cleanups (&final_cleanup_chain, old_chain);
  return;
}



;; Function discard_my_cleanups (discard_my_cleanups, funcdef_no=41, decl_uid=3106, cgraph_uid=41, symbol_order=46)

discard_my_cleanups (struct cleanup * * pmy_chain, struct cleanup * old_chain)
{
  struct cleanup * ptr;

  goto <D.3139>;
  <D.3138>:
  _1 = ptr->next;
  *pmy_chain = _1;
  _2 = ptr->free_arg;
  if (_2 != 0B) goto <D.3467>; else goto <D.3468>;
  <D.3467>:
  _3 = ptr->free_arg;
  _4 = ptr->arg;
  _3 (_4);
  <D.3468>:
  free (ptr);
  <D.3139>:
  ptr = *pmy_chain;
  if (ptr != old_chain) goto <D.3138>; else goto <D.3140>;
  <D.3140>:
  _5 = ptr->arg;
  printf ("freearg: %x\n", _5);
  return;
}



;; Function printf (<unset-asm-name>, funcdef_no=15, decl_uid=798, cgraph_uid=15, symbol_order=15)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
printf (const char * restrict __fmt)
{
  int D.3469;

  D.3469 = __printf_chk (1, __fmt, __builtin_va_arg_pack ());
  goto <D.3470>;
  <D.3470>:
  return D.3469;
}



;; Function do_cleanups (do_cleanups, funcdef_no=43, decl_uid=3113, cgraph_uid=43, symbol_order=48)

do_cleanups (struct cleanup * old_chain)
{
  do_my_cleanups (&cleanup_chain, old_chain);
  return;
}



;; Function do_my_cleanups (do_my_cleanups, funcdef_no=42, decl_uid=3111, cgraph_uid=42, symbol_order=47)

do_my_cleanups (struct cleanup * * pmy_chain, struct cleanup * old_chain)
{
  struct cleanup * ptr;

  goto <D.3147>;
  <D.3146>:
  _1 = ptr->next;
  *pmy_chain = _1;
  _2 = ptr->function;
  _3 = ptr->arg;
  _2 (_3);
  printf ("cleanup!!!!! =========\n");
  _4 = ptr->arg;
  printf ("%x\n", _4);
  _5 = ptr->free_arg;
  if (_5 != 0B) goto <D.3471>; else goto <D.3472>;
  <D.3471>:
  _6 = ptr->free_arg;
  _7 = ptr->arg;
  _6 (_7);
  <D.3472>:
  free (ptr);
  <D.3147>:
  ptr = *pmy_chain;
  if (ptr != old_chain) goto <D.3146>; else goto <D.3148>;
  <D.3148>:
  return;
}



;; Function make_cleanup (make_cleanup, funcdef_no=46, decl_uid=3126, cgraph_uid=46, symbol_order=51)

make_cleanup (void (*<T9e0>) (void *) function, void * arg)
{
  struct cleanup * D.3473;

  D.3473 = make_my_cleanup (&cleanup_chain, function, arg);
  goto <D.3474>;
  <D.3474>:
  return D.3473;
}



;; Function make_my_cleanup (make_my_cleanup, funcdef_no=45, decl_uid=3123, cgraph_uid=45, symbol_order=50)

make_my_cleanup (struct cleanup * * pmy_chain, void (*<T9e0>) (void *) function, void * arg)
{
  struct cleanup * D.3475;

  D.3475 = make_my_cleanup2 (pmy_chain, function, arg, 0B);
  goto <D.3476>;
  <D.3476>:
  return D.3475;
}



;; Function make_my_cleanup2 (make_my_cleanup2, funcdef_no=44, decl_uid=3119, cgraph_uid=44, symbol_order=49)

make_my_cleanup2 (struct cleanup * * pmy_chain, void (*<T9e0>) (void *) function, void * arg, void (*<Ted>) (void *) free_arg)
{
  struct cleanup * old_chain;
  struct cleanup * new;
  struct cleanup * D.3479;

  new = malloc (32);
  old_chain = *pmy_chain;
  _1 = *pmy_chain;
  new->next = _1;
  new->function = function;
  new->free_arg = free_arg;
  new->arg = arg;
  *pmy_chain = new;
  if (old_chain == 0B) goto <D.3477>; else goto <D.3478>;
  <D.3477>:
  exit (1);
  <D.3478>:
  D.3479 = old_chain;
  goto <D.3480>;
  <D.3480>:
  return D.3479;
}



;; Function filename_cmp (filename_cmp, funcdef_no=47, decl_uid=3393, cgraph_uid=47, symbol_order=84)

filename_cmp (const char * s1, const char * s2)
{
  int D.3481;

  D.3481 = strcmp (s1, s2);
  goto <D.3482>;
  <D.3482>:
  return D.3481;
}



;; Function make_tempname (make_tempname, funcdef_no=49, decl_uid=3406, cgraph_uid=49, symbol_order=86)

make_tempname (char * filename)
{
  int fd;
  char * tmpname;
  char * D.3485;

  tmpname = template_in_dir (filename);
  fd = mkstemp (tmpname);
  if (fd == -1) goto <D.3483>; else goto <D.3484>;
  <D.3483>:
  free (tmpname);
  D.3485 = 0B;
  goto <D.3486>;
  <D.3484>:
  close (fd);
  D.3485 = tmpname;
  goto <D.3486>;
  <D.3486>:
  return D.3485;
}



;; Function template_in_dir (template_in_dir, funcdef_no=48, decl_uid=3397, cgraph_uid=48, symbol_order=85)

template_in_dir (const char * path)
{
  size_t len;
  char * tmpname;
  const char * slash;
  char * D.3490;

  slash = strrchr (path, 47);
  if (slash != 0B) goto <D.3487>; else goto <D.3488>;
  <D.3487>:
  slash.0_1 = (long int) slash;
  path.1_2 = (long int) path;
  _3 = slash.0_1 - path.1_2;
  len = (size_t) _3;
  _4 = len + 11;
  tmpname = malloc (_4);
  memcpy (tmpname, path, len);
  len.2_5 = len;
  len = len.2_5 + 1;
  _6 = tmpname + len.2_5;
  *_6 = 47;
  goto <D.3489>;
  <D.3488>:
  tmpname = malloc (9);
  len = 0;
  <D.3489>:
  _7 = tmpname + len;
  memcpy (_7, "stXXXXXX", 9);
  D.3490 = tmpname;
  goto <D.3491>;
  <D.3491>:
  return D.3490;
}



;; Function set_times (set_times, funcdef_no=50, decl_uid=3414, cgraph_uid=50, symbol_order=87)

set_times (const char * destination, const struct stat * statbuf)
{
  long int tb[2];
  int result;

  _1 = statbuf->st_atim.tv_sec;
  tb[0] = _1;
  _2 = statbuf->st_mtim.tv_sec;
  tb[1] = _2;
  result = utime (destination, &tb);
  tb = {CLOBBER};
  if (result != 0) goto <D.3492>; else goto <D.3493>;
  <D.3492>:
  printf ("%s: cannot set time:", destination);
  <D.3493>:
  return;
}



;; Function smart_rename (smart_rename, funcdef_no=51, decl_uid=3423, cgraph_uid=51, symbol_order=88)

smart_rename (const char * from, const char * to, int preserve_dates)
{
  int ret;
  struct stat s;
  bfd_boolean exists;
  int D.3510;

  ret = 0;
  _1 = lstat (to, &s);
  _2 = _1 == 0;
  exists = (bfd_boolean) _2;
  if (exists == 0) goto <D.3494>; else goto <D.3497>;
  <D.3497>:
  _3 = BIT_FIELD_REF <s, 32, 192>;
  _4 = _3 & 61568;
  if (_4 == 32896) goto <D.3498>; else goto <D.3495>;
  <D.3498>:
  _5 = s.st_nlink;
  if (_5 == 1) goto <D.3494>; else goto <D.3495>;
  <D.3494>:
  ret = rename (from, to);
  if (ret == 0) goto <D.3499>; else goto <D.3500>;
  <D.3499>:
  if (exists != 0) goto <D.3501>; else goto <D.3502>;
  <D.3501>:
  _6 = s.st_mode;
  _7 = _6 & 511;
  chmod (to, _7);
  _8 = s.st_gid;
  _9 = s.st_uid;
  _10 = chown (to, _9, _8);
  if (_10 >= 0) goto <D.3503>; else goto <D.3504>;
  <D.3503>:
  _11 = s.st_mode;
  _12 = _11 & 4095;
  chmod (to, _12);
  <D.3504>:
  <D.3502>:
  goto <D.3505>;
  <D.3500>:
  printf ("unable to rename \'%s\';", to);
  unlink (from);
  <D.3505>:
  goto <D.3496>;
  <D.3495>:
  if (ret != 0) goto <D.3506>; else goto <D.3507>;
  <D.3506>:
  printf ("unable to copy file \'%s\';", to);
  <D.3507>:
  if (preserve_dates != 0) goto <D.3508>; else goto <D.3509>;
  <D.3508>:
  set_times (to, &s);
  <D.3509>:
  unlink (from);
  <D.3496>:
  D.3510 = ret;
  goto <D.3512>;
  <D.3512>:
  s = {CLOBBER};
  goto <D.3511>;
  <D.3511>:
  return D.3510;
}



;; Function unlink_if_ordinary (unlink_if_ordinary, funcdef_no=52, decl_uid=3434, cgraph_uid=52, symbol_order=89)

unlink_if_ordinary (const char * name)
{
  _1 = MEM[(int *)name];
  printf ("%d\n", _1);
  return;
}



;; Function main (main, funcdef_no=55, decl_uid=3464, cgraph_uid=55, symbol_order=92)

main (int argc, char * * argv)
{
  int D.3513;

  _1 = *argv;
  program_name = _1;
  copy_main (argc, argv);
  D.3513 = status;
  goto <D.3514>;
  D.3513 = 0;
  goto <D.3514>;
  <D.3514>:
  return D.3513;
}



;; Function copy_main (copy_main, funcdef_no=54, decl_uid=3449, cgraph_uid=54, symbol_order=91)

copy_main (int argc, char * * argv)
{
  const struct bfd_arch_info_type * input_arch;
  int statbuf;
  int c;
  bfd_boolean formats_info;
  bfd_boolean change_warn;
  bfd_boolean show_version;
  char * output_target;
  char * input_target;
  char * tmpname;
  char * output_filename;
  char * input_filename;
  int D.3529;

  input_filename = 0B;
  output_filename = 0B;
  input_target = 0B;
  output_target = 0B;
  show_version = 0;
  change_warn = 1;
  formats_info = 0;
  input_arch = 0B;
  optind.3_1 = optind;
  _2 = (long unsigned int) optind.3_1;
  _3 = _2 * 8;
  _4 = argv + _3;
  input_filename = *_4;
  optind.4_5 = optind;
  _6 = optind.4_5 + 1;
  if (argc > _6) goto <D.3515>; else goto <D.3516>;
  <D.3515>:
  optind.5_7 = optind;
  _8 = (sizetype) optind.5_7;
  _9 = _8 + 1;
  _10 = _9 * 8;
  _11 = argv + _10;
  output_filename = *_11;
  <D.3516>:
  if (output_filename == 0B) goto <D.3517>; else goto <D.3518>;
  <D.3517>:
  tmpname = make_tempname (input_filename);
  goto <D.3519>;
  <D.3518>:
  tmpname = output_filename;
  <D.3519>:
  if (tmpname == 0B) goto <D.3520>; else goto <D.3521>;
  <D.3520>:
  printf ("warning: could not create temporary file whilst copying \'%s\'", input_filename);
  exit (1);
  <D.3521>:
  copy_file (input_filename, tmpname, input_target, output_target, input_arch);
  status.6_12 = status;
  if (status.6_12 == 0) goto <D.3522>; else goto <D.3523>;
  <D.3522>:
  preserve_dates.7_13 = preserve_dates;
  if (preserve_dates.7_13 != 0) goto <D.3524>; else goto <D.3525>;
  <D.3524>:
  set_times (tmpname, &statbuf);
  <D.3525>:
  if (tmpname != output_filename) goto <D.3526>; else goto <D.3527>;
  <D.3526>:
  preserve_dates.8_14 = preserve_dates;
  _15 = smart_rename (tmpname, input_filename, preserve_dates.8_14);
  _16 = _15 != 0;
  _17 = (int) _16;
  status = _17;
  <D.3527>:
  goto <D.3528>;
  <D.3523>:
  unlink_if_ordinary (tmpname);
  <D.3528>:
  D.3529 = 0;
  goto <D.3531>;
  <D.3531>:
  statbuf = {CLOBBER};
  goto <D.3530>;
  <D.3530>:
  return D.3529;
}



;; Function copy_file (copy_file, funcdef_no=53, decl_uid=3441, cgraph_uid=53, symbol_order=90)

copy_file (const char * input_filename, const char * output_filename, const char * input_target, const char * output_target, const struct bfd_arch_info_type * input_arch)
{
  off_t size;
  char * * core_matching;
  char * * obj_matching;
  struct bfd * ibfd;

  _1 = rand ();
  _2 = _1 % 2;
  _3 = _2 == 1;
  size = (off_t) _3;
  if (size <= 0) goto <D.3532>; else goto <D.3533>;
  <D.3532>:
  if (size == 0) goto <D.3534>; else goto <D.3535>;
  <D.3534>:
  printf ("error: the input file \'%s\' is empty", input_filename);
  <D.3535>:
  status = 1;
  goto <D.3536>;
  <D.3533>:
  <D.3536>:
  return;
}


