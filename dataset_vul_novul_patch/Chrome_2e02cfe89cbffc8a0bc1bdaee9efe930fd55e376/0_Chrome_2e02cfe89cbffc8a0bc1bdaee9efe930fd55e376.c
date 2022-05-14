bool ParamTraits<LOGFONT>::Read(const Message* m, PickleIterator* iter,
                                 param_type* r) {
   const char *data;
   int data_size = 0;
  if (m->ReadData(iter, &data, &data_size) && data_size == sizeof(LOGFONT)) {
    const LOGFONT *font = reinterpret_cast<LOGFONT*>(const_cast<char*>(data));
    if (_tcsnlen(font->lfFaceName, LF_FACESIZE) < LF_FACESIZE) {
      memcpy(r, data, sizeof(LOGFONT));
      return true;
    }
   }
 
  NOTREACHED();
  return false;
 }
