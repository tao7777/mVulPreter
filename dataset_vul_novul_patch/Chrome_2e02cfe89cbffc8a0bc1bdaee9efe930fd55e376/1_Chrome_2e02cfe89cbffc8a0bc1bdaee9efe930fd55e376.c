bool ParamTraits<LOGFONT>::Read(const Message* m, PickleIterator* iter,
                                 param_type* r) {
   const char *data;
   int data_size = 0;
  bool result = m->ReadData(iter, &data, &data_size);
  if (result && data_size == sizeof(LOGFONT)) {
    memcpy(r, data, sizeof(LOGFONT));
  } else {
    result = false;
    NOTREACHED();
   }
 
  return result;
 }
