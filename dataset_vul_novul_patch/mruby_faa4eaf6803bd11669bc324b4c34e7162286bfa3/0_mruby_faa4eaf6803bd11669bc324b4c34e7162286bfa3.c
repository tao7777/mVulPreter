 mrb_class_real(struct RClass* cl)
 {
  if (cl == 0) return NULL;
   while ((cl->tt == MRB_TT_SCLASS) || (cl->tt == MRB_TT_ICLASS)) {
     cl = cl->super;
    if (cl == 0) return NULL;
   }
   return cl;
 }
