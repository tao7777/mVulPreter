 static void tokenadd(struct jv_parser* p, char c) {
   assert(p->tokenpos <= p->tokenlen);
  if (p->tokenpos >= (p->tokenlen - 1)) {
     p->tokenlen = p->tokenlen*2 + 256;
     p->tokenbuf = jv_mem_realloc(p->tokenbuf, p->tokenlen);
   }
  assert(p->tokenpos < p->tokenlen);
  p->tokenbuf[p->tokenpos++] = c;
}
