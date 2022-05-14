 store_message(png_store *ps, png_const_structp pp, char *buffer, size_t bufsize,
   size_t pos, const char *msg)
 {
    if (pp != NULL && pp == ps->pread)
    {
 /* Reading a file */
      pos = safecat(buffer, bufsize, pos, "read: ");

 if (ps->current != NULL)
 {
         pos = safecat(buffer, bufsize, pos, ps->current->name);
         pos = safecat(buffer, bufsize, pos, sep);
 }
 }

 else if (pp != NULL && pp == ps->pwrite)
 {
 /* Writing a file */
      pos = safecat(buffer, bufsize, pos, "write: ");
      pos = safecat(buffer, bufsize, pos, ps->wname);
      pos = safecat(buffer, bufsize, pos, sep);
 }

 else
 {
 /* Neither reading nor writing (or a memory error in struct delete) */
      pos = safecat(buffer, bufsize, pos, "pngvalid: ");
 }

 if (ps->test[0] != 0)
 {
      pos = safecat(buffer, bufsize, pos, ps->test);
      pos = safecat(buffer, bufsize, pos, sep);
 }
   pos = safecat(buffer, bufsize, pos, msg);
 return pos;
}
