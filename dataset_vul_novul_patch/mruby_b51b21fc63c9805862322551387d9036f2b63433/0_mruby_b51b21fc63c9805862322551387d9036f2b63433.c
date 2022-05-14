mrb_io_initialize_copy(mrb_state *mrb, mrb_value copy)
{
  mrb_value orig;
  mrb_value buf;
  struct mrb_io *fptr_copy;
  struct mrb_io *fptr_orig;
   mrb_bool failed = TRUE;
 
   mrb_get_args(mrb, "o", &orig);
  fptr_orig = io_get_open_fptr(mrb, orig);
   fptr_copy = (struct mrb_io *)DATA_PTR(copy);
   if (fptr_copy != NULL) {
     fptr_finalize(mrb, fptr_copy, FALSE);
     mrb_free(mrb, fptr_copy);
   }
   fptr_copy = (struct mrb_io *)mrb_io_alloc(mrb);
 
   DATA_TYPE(copy) = &mrb_io_type;
   DATA_PTR(copy) = fptr_copy;

  buf = mrb_iv_get(mrb, orig, mrb_intern_cstr(mrb, "@buf"));
  mrb_iv_set(mrb, copy, mrb_intern_cstr(mrb, "@buf"), buf);

  fptr_copy->fd = mrb_dup(mrb, fptr_orig->fd, &failed);
  if (failed) {
    mrb_sys_fail(mrb, 0);
  }
  mrb_fd_cloexec(mrb, fptr_copy->fd);

  if (fptr_orig->fd2 != -1) {
    fptr_copy->fd2 = mrb_dup(mrb, fptr_orig->fd2, &failed);
    if (failed) {
      close(fptr_copy->fd);
      mrb_sys_fail(mrb, 0);
    }
    mrb_fd_cloexec(mrb, fptr_copy->fd2);
  }

  fptr_copy->pid = fptr_orig->pid;
  fptr_copy->readable = fptr_orig->readable;
  fptr_copy->writable = fptr_orig->writable;
  fptr_copy->sync = fptr_orig->sync;
  fptr_copy->is_socket = fptr_orig->is_socket;

  return copy;
}
