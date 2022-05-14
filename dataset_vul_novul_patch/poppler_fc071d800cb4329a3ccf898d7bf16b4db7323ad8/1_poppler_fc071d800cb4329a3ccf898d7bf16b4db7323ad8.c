 static void exitErrorHandler(jpeg_common_struct *error) {
   j_decompress_ptr cinfo = (j_decompress_ptr)error;
   str_src_mgr * src = (struct str_src_mgr *)cinfo->src;
  src->abort = true;
 }
