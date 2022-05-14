long mkvparser::UnserializeFloat(
  {
    signed char b;
 
    const long status = pReader->Read(pos, 1, (unsigned char*)&b);
 
    if (status < 0)
      return status;
 
    result = b;
 
    ++pos;
  }
 
  for (long i = 1; i < size; ++i) {
    unsigned char b;
 
    const long status = pReader->Read(pos, 1, &b);
 
    if (status < 0)
      return status;
 
    result <<= 8;
    result |= b;
 
    ++pos;
  }
 
  return 0;  // success
 }
