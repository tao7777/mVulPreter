 allocate(struct file *file, int allocate_idat)
 {
   struct control *control = png_voidcast(struct control*, file->alloc_ptr);
 
    if (allocate_idat)
    {
      assert(file->idat == NULL);
      IDAT_init(&control->idat, file);
 }

 else /* chunk */
 {
      assert(file->chunk == NULL);
      chunk_init(&control->chunk, file);
 }
}
