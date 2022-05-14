void DCTStream::reset() {
  int row_stride;

  str->reset();

  if (row_buffer)
  {
    jpeg_destroy_decompress(&cinfo);
    init();
  }

  bool startFound = false;
  int c = 0, c2 = 0;
  while (!startFound)
  {
    if (!c)
       if (c == -1)
       {
         error(-1, "Could not find start of jpeg data");
        src.abort = true;
         return;
       }
       if (c != 0xFF) c = 0;
        return;
      }
      if (c != 0xFF) c = 0;
    }
