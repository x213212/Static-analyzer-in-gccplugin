
;; Function xrealloc (xrealloc, funcdef_no=0, decl_uid=1963, cgraph_uid=0, symbol_order=0)

xrealloc (void * ptr, size_t size)
{
  void * ret;
  void * D.2094;

  ret = realloc (ptr, size);
  if (ret == 0B) goto <D.2082>; else goto <D.2083>;
  <D.2082>:
  if (size == 0) goto <D.2084>; else goto <D.2085>;
  <D.2084>:
  ret = realloc (ptr, 1);
  <D.2085>:
  <D.2083>:
  if (ret == 0B) goto <D.2086>; else goto <D.2087>;
  <D.2086>:
  ret = realloc (ptr, size);
  if (ret == 0B) goto <D.2088>; else goto <D.2089>;
  <D.2088>:
  if (size == 0) goto <D.2090>; else goto <D.2091>;
  <D.2090>:
  ret = realloc (ptr, 1);
  <D.2091>:
  <D.2089>:
  if (ret == 0B) goto <D.2092>; else goto <D.2093>;
  <D.2092>:
  printf ("Out of memory, realloc failed%s", "");
  exit (1);
  <D.2093>:
  <D.2087>:
  D.2094 = ret;
  goto <D.2095>;
  <D.2095>:
  return D.2094;
}



;; Function update_tree_entry (update_tree_entry, funcdef_no=2, decl_uid=1985, cgraph_uid=2, symbol_order=3)

update_tree_entry (struct tree_desc * desc)
{
  int tmp;
  struct strbuf err;

  err.alloc = 0;
  err.len = 0;
  err.buf = &strbuf_slopbuf;
  tmp = update_tree_entry_internal (desc, &err);
  if (tmp != 0) goto <D.2096>; else goto <D.2097>;
  <D.2096>:
  _1 = err.buf;
  printf ("%s", _1);
  exit (1);
  <D.2097>:
  strbuf_release (&err);
  goto <D.2099>;
  <D.2099>:
  err = {CLOBBER};
  goto <D.2098>;
  <D.2098>:
  return;
}



;; Function update_tree_entry_internal (update_tree_entry_internal, funcdef_no=1, decl_uid=1976, cgraph_uid=1, symbol_order=2)

update_tree_entry_internal (struct tree_desc * desc, struct strbuf * err)
{
  int tmp;
  long unsigned int len;
  long unsigned int size;
  const unsigned char * end;
  const void * buf;
  int D.2104;

  buf = desc->buffer;
  _1 = desc->entry.oid;
  _2 = &_1->hash;
  end = _2 + 20;
  _3 = desc->size;
  size = (long unsigned int) _3;
  end.0_4 = (long int) end;
  buf.1_5 = (long int) buf;
  _6 = end.0_4 - buf.1_5;
  len = (long unsigned int) _6;
  if (size < len) goto <D.2100>; else goto <D.2101>;
  <D.2100>:
  printf ("too-short tree file%s", buf);
  exit (1);
  <D.2101>:
  buf = end;
  size = size - len;
  desc->buffer = buf;
  _7 = (unsigned int) size;
  desc->size = _7;
  if (size != 0) goto <D.2102>; else goto <D.2103>;
  <D.2102>:
  tmp = decode_tree_entry (desc, buf, size, err);
  D.2104 = tmp;
  goto <D.2105>;
  <D.2103>:
  D.2104 = 0;
  goto <D.2105>;
  <D.2105>:
  return D.2104;
}



;; Function traverse_trees (traverse_trees, funcdef_no=4, decl_uid=2006, cgraph_uid=4, symbol_order=5)

traverse_trees (int n, struct tree_desc * t, struct traverse_info * info)
{
  struct name_entry * e;
  void * tmp___0;
  struct name_entry * entry;
  void * tmp;
  struct tree_desc_x * tx;
  int n___0;
  int i;
  int D.2106;

  _1 = (long unsigned int) n___0;
  tmp = calloc (_1, 48);
  tx = tmp;
  _2 = (long unsigned int) n___0;
  _3 = _2 * 24;
  tmp___0 = malloc (_3);
  entry = tmp___0;
  e = 0B;
  i = 0;
  goto <D.2016>;
  <D.2015>:
  _4 = (long unsigned int) i;
  _5 = _4 * 40;
  _6 = t + _5;
  _7 = (long unsigned int) i;
  _8 = _7 * 48;
  _9 = tx + _8;
  _9->d = *_6;
  i = i + 1;
  <D.2016>:
  if (i < n___0) goto <D.2015>; else goto <D.2017>;
  <D.2017>:
  i = 0;
  goto <D.2019>;
  <D.2018>:
  _10 = (long unsigned int) i;
  _11 = _10 * 24;
  e = entry + _11;
  _12 = (long unsigned int) i;
  _13 = _12 * 48;
  _14 = tx + _13;
  extended_entry_extract (_14, e, 0B, 0);
  i = i + 1;
  <D.2019>:
  if (i < n___0) goto <D.2018>; else goto <D.2020>;
  <D.2020>:
  free (tx);
  D.2106 = 1;
  goto <D.2107>;
  <D.2107>:
  return D.2106;
}



;; Function extended_entry_extract (extended_entry_extract, funcdef_no=3, decl_uid=1995, cgraph_uid=3, symbol_order=4)

extended_entry_extract (struct tree_desc_x * t, struct name_entry * a, const char * first, int first_len)
{
  struct tree_desc_skip * skip;

  <D.2002>:
  _1 = t->d.size;
  if (_1 == 0) goto <D.2108>; else goto <D.2109>;
  <D.2108>:
  entry_clear (a);
  goto <D.1998>;
  <D.2109>:
  _2 = &t->d;
  entry_extract (_2, a);
  skip = t->skip;
  goto <D.2001>;
  <D.2000>:
  _3 = a->path;
  _4 = (long unsigned int) _3;
  _5 = skip->ptr;
  _6 = (long unsigned int) _5;
  if (_4 == _6) goto <D.1999>; else goto <D.2110>;
  <D.2110>:
  skip = skip->prev;
  <D.2001>:
  if (skip != 0B) goto <D.2000>; else goto <D.1999>;
  <D.1999>:
  if (skip == 0B) goto <D.1998>; else goto <D.2111>;
  <D.2111>:
  _7 = &t->d;
  update_tree_entry (_7);
  goto <D.2002>;
  <D.1998>:
  goto <D.2112>;
  <D.2112>:
  return;
}



;; Function unpack_trees (unpack_trees, funcdef_no=5, decl_uid=2024, cgraph_uid=5, symbol_order=6)

unpack_trees (unsigned int len, struct tree_desc * t, struct unpack_trees_options * o)
{
  int D.2113;

  len.2_1 = (int) len;
  traverse_trees (len.2_1, t, 0B);
  D.2113 = 0;
  goto <D.2114>;
  <D.2114>:
  return D.2113;
}



;; Function init_tree_desc (init_tree_desc, funcdef_no=7, decl_uid=2036, cgraph_uid=7, symbol_order=8)

init_tree_desc (struct tree_desc * desc, const void * buffer, long unsigned int size)
{
  int tmp;
  struct strbuf err;

  err.alloc = 0;
  err.len = 0;
  err.buf = &strbuf_slopbuf;
  tmp = init_tree_desc_internal (desc, buffer, size, &err);
  if (tmp != 0) goto <D.2115>; else goto <D.2116>;
  <D.2115>:
  _1 = err.buf;
  printf ("%s", _1);
  exit (1);
  <D.2116>:
  strbuf_release (&err);
  goto <D.2118>;
  <D.2118>:
  err = {CLOBBER};
  goto <D.2117>;
  <D.2117>:
  return;
}



;; Function init_tree_desc_internal (init_tree_desc_internal, funcdef_no=6, decl_uid=2030, cgraph_uid=6, symbol_order=7)

init_tree_desc_internal (struct tree_desc * desc, const void * buffer, long unsigned int size, struct strbuf * err)
{
  int tmp;
  int D.2121;

  desc->buffer = buffer;
  _1 = (unsigned int) size;
  desc->size = _1;
  if (size != 0) goto <D.2119>; else goto <D.2120>;
  <D.2119>:
  tmp = decode_tree_entry (desc, buffer, size, err);
  D.2121 = tmp;
  goto <D.2122>;
  <D.2120>:
  D.2121 = 0;
  goto <D.2122>;
  <D.2122>:
  return D.2121;
}



;; Function fill_tree_descriptor (fill_tree_descriptor, funcdef_no=8, decl_uid=2043, cgraph_uid=8, symbol_order=9)

fill_tree_descriptor (struct tree_desc * desc, const struct object_id * oid)
{
  int tmp;
  void * buf;
  long unsigned int size;

  size = 0;
  buf = 0B;
  if (oid != 0B) goto <D.2123>; else goto <D.2124>;
  <D.2123>:
  buf = malloc (100);
  if (buf == 0B) goto <D.2125>; else goto <D.2126>;
  <D.2125>:
  tmp = oid_to_hex (oid);
  printf ("unable to read tree %s", tmp);
  exit (1);
  <D.2126>:
  <D.2124>:
  init_tree_desc (desc, buf, size);
  goto <D.2127>;
  <D.2127>:
  return;
}



;; Function main (main, funcdef_no=10, decl_uid=2076, cgraph_uid=10, symbol_order=11)

main ()
{
  int quiet;
  int type;
  struct object_id oid;
  int D.2128;

  reset_index (&oid, type, quiet);
  D.2128 = 0;
  goto <D.2130>;
  <D.2130>:
  oid = {CLOBBER};
  goto <D.2129>;
  D.2128 = 0;
  goto <D.2129>;
  <D.2129>:
  return D.2128;
}



;; Function reset_index (reset_index, funcdef_no=9, decl_uid=2058, cgraph_uid=9, symbol_order=10)

reset_index (const struct object_id * oid, int reset_type, int quiet)
{
  int tmp___3;
  int tmp___2;
  int tmp___1;
  int tmp___0;
  int tmp;
  struct object_id head_oid;
  struct unpack_trees_options opts;
  struct tree * tree;
  struct tree_desc desc[2];
  int nr;
  int D.2137;

  nr = 0;
  memset (&opts, 0, 640);
  opts.head_idx = 1;
  opts.src_index = &the_index;
  opts.dst_index = &the_index;
  opts.fn = oneway_merge;
  opts.merge = 1;
  if (quiet == 0) goto <D.2131>; else goto <D.2132>;
  <D.2131>:
  opts.verbose_update = 1;
  <D.2132>:
  switch (reset_type) <default: <D.2074>, case 2: <D.2073>, case 3: <D.2070>, case 4: <D.2071>>
  <D.2070>:
  <D.2071>:
  opts.update = 1;
  goto <D.2072>;
  <D.2073>:
  opts.update = 1;
  <D.2074>:
  opts.reset = 1;
  <D.2072>:
  read_cache_unmerged ();
  if (reset_type == 4) goto <D.2133>; else goto <D.2134>;
  <D.2133>:
  tmp___1 = get_oid ("HEAD", &head_oid);
  if (tmp___1 != 0) goto <D.2135>; else goto <D.2136>;
  <D.2135>:
  tmp = _ ("You do not have a valid HEAD.");
  tmp___0 = error (tmp);
  D.2137 = tmp___0;
  goto <D.2146>;
  <D.2136>:
  _1 = (long unsigned int) nr;
  _2 = _1 * 40;
  _3 = &desc + _2;
  fill_tree_descriptor (_3, &head_oid);
  nr = nr + 1;
  opts.fn = twoway_merge;
  <D.2134>:
  _4 = (long unsigned int) nr;
  _5 = _4 * 40;
  _6 = &desc + _5;
  fill_tree_descriptor (_6, oid);
  nr = nr + 1;
  _7 = desc[nr].buffer;
  printf ("%d\n", _7);
  nr.3_8 = (unsigned int) nr;
  tmp___2 = unpack_trees (nr.3_8, &desc, &opts);
  if (tmp___2 != 0) goto <D.2138>; else goto <D.2139>;
  <D.2138>:
  D.2137 = -1;
  goto <D.2146>;
  <D.2139>:
  if (reset_type == 0) goto <D.2140>; else goto <D.2141>;
  <D.2140>:
  tmp___3 = parse_tree_indirect (oid);
  _9 = (long int) tmp___3;
  tree = (struct tree *) _9;
  prime_cache_tree (&the_index, tree);
  goto <D.2142>;
  <D.2141>:
  if (reset_type == 2) goto <D.2143>; else goto <D.2144>;
  <D.2143>:
  tmp___3 = parse_tree_indirect (oid);
  _10 = (long int) tmp___3;
  tree = (struct tree *) _10;
  prime_cache_tree (&the_index, tree);
  <D.2144>:
  <D.2142>:
  D.2137 = 0;
  goto <D.2146>;
  <D.2146>:
  desc = {CLOBBER};
  opts = {CLOBBER};
  head_oid = {CLOBBER};
  goto <D.2145>;
  <D.2145>:
  return D.2137;
}


