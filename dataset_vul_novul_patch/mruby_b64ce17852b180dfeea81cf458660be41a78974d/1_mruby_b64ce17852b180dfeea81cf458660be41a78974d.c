 init_copy(mrb_state *mrb, mrb_value dest, mrb_value obj)
 {
   switch (mrb_type(obj)) {
     case MRB_TT_CLASS:
     case MRB_TT_MODULE:
       copy_class(mrb, dest, obj);
      mrb_iv_copy(mrb, dest, obj);
      mrb_iv_remove(mrb, dest, mrb_intern_lit(mrb, "__classname__"));
      break;
    case MRB_TT_OBJECT:
    case MRB_TT_SCLASS:
    case MRB_TT_HASH:
    case MRB_TT_DATA:
    case MRB_TT_EXCEPTION:
      mrb_iv_copy(mrb, dest, obj);
      break;
    case MRB_TT_ISTRUCT:
      mrb_istruct_copy(dest, obj);
      break;

    default:
      break;
  }
  mrb_funcall(mrb, dest, "initialize_copy", 1, obj);
}
