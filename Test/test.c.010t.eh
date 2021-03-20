
;; Function strlcat (strlcat, funcdef_no=39, decl_uid=3566, cgraph_uid=39, symbol_order=40)

strlcat (char * restrict dst, const char * restrict src, size_t maxlen)
{
  const size_t dstlen;
  const size_t srclen;
  size_t D.3638;

  srclen = strlen (src);
  _1 = strnlen (dst, maxlen);
  dstlen = (const size_t) _1;
  if (dstlen == maxlen) goto <D.3636>; else goto <D.3637>;
  <D.3636>:
  D.3638 = maxlen + srclen;
  goto <D.3642>;
  <D.3637>:
  _2 = maxlen - dstlen;
  if (srclen < _2) goto <D.3639>; else goto <D.3640>;
  <D.3639>:
  _3 = srclen + 1;
  _4 = dst + dstlen;
  memcpy (_4, src, _3);
  goto <D.3641>;
  <D.3640>:
  _5 = maxlen + 18446744073709551615;
  _6 = dst + dstlen;
  memcpy (_6, src, _5);
  _7 = dstlen + maxlen;
  _8 = _7 + 18446744073709551615;
  _9 = dst + _8;
  *_9 = 0;
  <D.3641>:
  D.3638 = dstlen + srclen;
  goto <D.3642>;
  <D.3642>:
  return D.3638;
}



;; Function tilde_expand_filename (tilde_expand_filename, funcdef_no=40, decl_uid=3577, cgraph_uid=40, symbol_order=41)

tilde_expand_filename (const char * filename, uid_t uid)
{
  u_int slash;
  u_int len;
  struct passwd * pw;
  char ret[64];
  char user[128];
  const char * path;
  char * D.3645;

  _1 = *filename;
  if (_1 != 126) goto <D.3643>; else goto <D.3644>;
  <D.3643>:
  D.3645 = strdup (filename);
  goto <D.3655>;
  <D.3644>:
  filename = filename + 1;
  path = strchr (filename, 47);
  _2 = path != 0B;
  _3 = path > filename;
  _4 = _2 & _3;
  if (_4 != 0) goto <D.3646>; else goto <D.3647>;
  <D.3646>:
  path.0_5 = (long int) path;
  filename.1_6 = (long int) filename;
  _7 = path.0_5 - filename.1_6;
  slash = (u_int) _7;
  if (slash > 127) goto <D.3648>; else goto <D.3649>;
  <D.3648>:
  printf ("tilde_expand_filename: ~username too long%s\n", "");
  exit (1);
  <D.3649>:
  _8 = (long unsigned int) slash;
  memcpy (&user, filename, _8);
  user[slash] = 0;
  <D.3647>:
  if (path != 0B) goto <D.3650>; else goto <D.3651>;
  <D.3650>:
  filename = path + 1;
  <D.3651>:
  _9 = strlcat (&ret, filename, 64);
  if (_9 > 63) goto <D.3652>; else goto <D.3653>;
  <D.3652>:
  printf ("tilde_expand_filename: Path too long%s\n", "");
  exit (1);
  <D.3653>:
  D.3645 = strdup (&ret);
  goto <D.3655>;
  <D.3655>:
  user = {CLOBBER};
  ret = {CLOBBER};
  goto <D.3654>;
  <D.3654>:
  return D.3645;
}



;; Function printf (<unset-asm-name>, funcdef_no=15, decl_uid=798, cgraph_uid=15, symbol_order=15)

__attribute__((__artificial__, __gnu_inline__, __always_inline__))
printf (const char * restrict __fmt)
{
  int D.3656;

  D.3656 = __printf_chk (1, __fmt, __builtin_va_arg_pack ());
  goto <D.3657>;
  <D.3657>:
  return D.3656;
}



;; Function read_keyfile_line (read_keyfile_line, funcdef_no=41, decl_uid=3594, cgraph_uid=41, symbol_order=42)

read_keyfile_line (struct FILE * f, const char * filename, char * buf, size_t bufsz, u_long * lineno)
{
  static const char __func__[18] = "read_keyfile_line";
  int D.3660;

  goto <D.3598>;
  <D.3597>:
  _1 = *lineno;
  _2 = _1 + 1;
  *lineno = _2;
  _3 = *lineno;
  printf ("%s: %s line %lu exceeds size limit\n", &__func__, filename, _3);
  _4 = rand ();
  _5 = _4 % 2;
  if (_5 == 1) goto <D.3658>; else goto <D.3659>;
  <D.3658>:
  D.3660 = 0;
  goto <D.3661>;
  <D.3659>:
  <D.3598>:
  _6 = rand ();
  _7 = _6 % 2;
  if (_7 == 1) goto <D.3597>; else goto <D.3599>;
  <D.3599>:
  D.3660 = -1;
  goto <D.3661>;
  <D.3661>:
  return D.3660;
}



;; Function main (main, funcdef_no=43, decl_uid=3625, cgraph_uid=43, symbol_order=44)

main (int argc, char * * argv)
{
  int i;
  int updating;
  struct passwd pw;
  int D.3662;

  _1 = time (0B);
  _2 = (unsigned int) _1;
  srand (_2);
  i = 0;
  goto <D.3632>;
  <D.3631>:
  _3 = (long unsigned int) i;
  _4 = _3 * 8;
  _5 = argv + _4;
  _6 = *_5;
  update_krl_from_file (&pw, _6);
  i = i + 1;
  <D.3632>:
  if (i < argc) goto <D.3631>; else goto <D.3633>;
  <D.3633>:
  D.3662 = 0;
  goto <D.3664>;
  <D.3664>:
  pw = {CLOBBER};
  goto <D.3663>;
  D.3662 = 0;
  goto <D.3663>;
  <D.3663>:
  return D.3662;
}



;; Function update_krl_from_file (update_krl_from_file, funcdef_no=42, decl_uid=3602, cgraph_uid=42, symbol_order=43)

update_krl_from_file (struct passwd * pw, const char * file)
{
  struct FILE * krl_spec;
  int r;
  int was_sha1;
  int was_explicit_key;
  int i;
  long long unsigned int serial2;
  long long unsigned int serial;
  char line[16384];
  char * ep;
  char * cp;
  char * path;
  u_long lnum;
  struct Key * key;

  key = 0B;
  lnum = 0;
  _1 = pw->pw_uid;
  _2 = (unsigned int) _1;
  path = tilde_expand_filename (file, _2);
  _3 = strcmp (path, "-");
  if (_3 == 0) goto <D.3665>; else goto <D.3666>;
  <D.3665>:
  krl_spec = stdin;
  free (path);
  path = strdup ("(standard input)");
  goto <D.3667>;
  <D.3666>:
  krl_spec = fopen (path, "r");
  if (krl_spec == 0B) goto <D.3668>; else goto <D.3669>;
  <D.3668>:
  printf ("fopen %s: \n", path);
  exit (1);
  <D.3669>:
  <D.3667>:
  quiet.2_4 = quiet;
  if (quiet.2_4 == 0) goto <D.3670>; else goto <D.3671>;
  <D.3670>:
  printf ("Revoking from %s\n", path);
  <D.3671>:
  goto <D.3620>;
  <D.3619>:
  _5 = rand ();
  _6 = _5 % 2;
  if (_6 == 1) goto <D.3672>; else goto <D.3673>;
  <D.3672>:
  printf ("%s: invalid\n", path);
  exit (1);
  <D.3673>:
  <D.3620>:
  _7 = read_keyfile_line (krl_spec, path, &line, 16384, &lnum);
  if (_7 == 0) goto <D.3619>; else goto <D.3621>;
  <D.3621>:
  _8 = strcmp (path, "-");
  if (_8 != 0) goto <D.3674>; else goto <D.3675>;
  <D.3674>:
  fclose (krl_spec);
  <D.3675>:
  lnum = {CLOBBER};
  line = {CLOBBER};
  return;
}


