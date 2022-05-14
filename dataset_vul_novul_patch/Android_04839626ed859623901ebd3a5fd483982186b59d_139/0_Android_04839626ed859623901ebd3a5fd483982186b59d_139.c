long long mkvparser::GetUIntLength(
long long mkvparser::GetUIntLength(IMkvReader* pReader, long long pos,
                                   long& len) {
  assert(pReader);
  assert(pos >= 0);
 
  long long total, available;
 
  int status = pReader->Length(&total, &available);
  assert(status >= 0);
  assert((total < 0) || (available <= total));
 
  len = 1;
 
  if (pos >= available)
    return pos;  // too few bytes available
 
  unsigned char b;

  status = pReader->Read(pos, 1, &b);

  if (status < 0)
    return status;

  assert(status == 0);

  if (b == 0)  // we can't handle u-int values larger than 8 bytes
    return E_FILE_FORMAT_INVALID;

  unsigned char m = 0x80;

  while (!(b & m)) {
    m >>= 1;
    ++len;
  }

  return 0;  // success
}

//// TODO(vigneshv): This function assumes that unsigned values never have their
//// high bit set.
long long mkvparser::UnserializeUInt(IMkvReader* pReader, long long pos,
                                     long long size) {
  assert(pReader);
  assert(pos >= 0);

  if ((size <= 0) || (size > 8))
    return E_FILE_FORMAT_INVALID;

  long long result = 0;

  for (long long i = 0; i < size; ++i) {
     unsigned char b;
 
    const long status = pReader->Read(pos, 1, &b);
 
     if (status < 0)
      return status;
 
    result <<= 8;
    result |= b;
 
    ++pos;
  }
 
  return result;
 }
