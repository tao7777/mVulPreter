fiber_switch(mrb_state *mrb, mrb_value self, mrb_int len, const mrb_value *a, mrb_bool resume, mrb_bool vmexec)
 {
   struct mrb_context *c = fiber_check(mrb, self);
   struct mrb_context *old_c = mrb->c;
   mrb_value value;
 
   fiber_check_cfunc(mrb, c);
  if (resume && c->status == MRB_FIBER_TRANSFERRED) {
     mrb_raise(mrb, E_FIBER_ERROR, "resuming transferred fiber");
   }
  if (c->status == MRB_FIBER_RUNNING || c->status == MRB_FIBER_RESUMED) {
     mrb_raise(mrb, E_FIBER_ERROR, "double resume (fib)");
   }
  if (c->status == MRB_FIBER_TERMINATED) {
     mrb_raise(mrb, E_FIBER_ERROR, "resuming dead fiber");
   }
  mrb->c->status = resume ? MRB_FIBER_RESUMED : MRB_FIBER_TRANSFERRED;
   c->prev = resume ? mrb->c : (c->prev ? c->prev : mrb->root_c);
  if (c->status == MRB_FIBER_CREATED) {
     mrb_value *b, *e;
 
    if (len >= c->stend - c->stack) {
      mrb_raise(mrb, E_FIBER_ERROR, "too many arguments to fiber");
    }
     b = c->stack+1;
     e = b + len;
     while (b<e) {
      *b++ = *a++;
    }
    c->cibase->argc = (int)len;
    value = c->stack[0] = MRB_PROC_ENV(c->ci->proc)->stack[0];
  }
   else {
     value = fiber_result(mrb, a, len);
   }
  fiber_switch_context(mrb, c);
 
   if (vmexec) {
     c->vmexec = TRUE;
    value = mrb_vm_exec(mrb, c->ci[-1].proc, c->ci->pc);
    mrb->c = old_c;
  }
  else {
    MARK_CONTEXT_MODIFY(c);
  }
  return value;
}
