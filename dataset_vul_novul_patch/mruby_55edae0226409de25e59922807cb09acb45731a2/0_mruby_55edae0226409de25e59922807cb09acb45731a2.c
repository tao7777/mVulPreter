mrb_obj_clone(mrb_state *mrb, mrb_value self)
{
  struct RObject *p;
  mrb_value clone;

  if (mrb_immediate_p(self)) {
    mrb_raisef(mrb, E_TYPE_ERROR, "can't clone %S", self);
  }
  if (mrb_type(self) == MRB_TT_SCLASS) {
    mrb_raise(mrb, E_TYPE_ERROR, "can't clone singleton class");
  }
  p = (struct RObject*)mrb_obj_alloc(mrb, mrb_type(self), mrb_obj_class(mrb, self));
  p->c = mrb_singleton_class_clone(mrb, self);
   mrb_field_write_barrier(mrb, (struct RBasic*)p, (struct RBasic*)p->c);
   clone = mrb_obj_value(p);
   init_copy(mrb, clone, self);
  p->flags |= mrb_obj_ptr(self)->flags & MRB_FLAG_IS_FROZEN;
 
   return clone;
 }
