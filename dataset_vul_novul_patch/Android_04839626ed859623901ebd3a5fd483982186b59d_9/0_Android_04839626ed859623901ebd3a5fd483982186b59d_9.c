void Track::Info::Clear()
  if (dst)  // should be NULL already
    return -1;
 
  const char* const src = this->*str;
 
