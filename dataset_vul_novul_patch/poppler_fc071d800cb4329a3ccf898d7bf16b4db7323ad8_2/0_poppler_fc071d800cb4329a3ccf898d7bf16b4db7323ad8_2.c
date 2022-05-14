static boolean str_fill_input_buffer(j_decompress_ptr cinfo)
 {
   int c;
   struct str_src_mgr * src = (struct str_src_mgr *)cinfo->src;
   if (src->index == 0) {
     c = 0xFF;
     src->index++;
    src->index++;
  }
  else if (src->index == 1) {
    c = 0xD8;
    src->index++;
  }
  else c = src->str->getChar();
  if (c != EOF)
  {
    src->buffer = c;
    src->pub.next_input_byte = &src->buffer;
    src->pub.bytes_in_buffer = 1;
    return TRUE;
  }
  else return FALSE;
}
