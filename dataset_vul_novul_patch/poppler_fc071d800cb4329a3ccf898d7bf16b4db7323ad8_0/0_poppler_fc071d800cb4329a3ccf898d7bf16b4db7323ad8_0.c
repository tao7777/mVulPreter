void DCTStream::init()
{
  jpeg_std_error(&jerr);
  jerr.error_exit = &exitErrorHandler;
  src.pub.init_source = str_init_source;
  src.pub.fill_input_buffer = str_fill_input_buffer;
  src.pub.skip_input_data = str_skip_input_data;
  src.pub.resync_to_restart = jpeg_resync_to_restart;
  src.pub.term_source = str_term_source;
   src.pub.next_input_byte = NULL;
   src.str = str;
   src.index = 0;
   current = NULL;
   limit = NULL;
   
  limit = NULL;
  
  cinfo.err = &jerr;
  jpeg_create_decompress(&cinfo);
  cinfo.src = (jpeg_source_mgr *)&src;
  row_buffer = NULL;
}
