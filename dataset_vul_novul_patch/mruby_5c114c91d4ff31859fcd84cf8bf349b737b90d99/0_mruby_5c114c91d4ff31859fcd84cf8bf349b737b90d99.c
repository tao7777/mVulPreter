mark_context_stack(mrb_state *mrb, struct mrb_context *c)
 {
   size_t i;
   size_t e;
  mrb_value nil;
 
   if (c->stack == NULL) return;
   e = c->stack - c->stbase;
  if (c->ci) e += c->ci->nregs;
  if (c->stbase + e > c->stend) e = c->stend - c->stbase;
  for (i=0; i<e; i++) {
     mrb_value v = c->stbase[i];
 
     if (!mrb_immediate_p(v)) {
      mrb_gc_mark(mrb, mrb_basic_ptr(v));
     }
   }
  e = c->stend - c->stbase;
  nil = mrb_nil_value();
  for (; i<e; i++) {
    c->stbase[i] = nil;
  }
 }
