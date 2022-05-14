long mkvparser::UnserializeInt(IMkvReader* pReader, long long pos, long size,
long UnserializeInt(IMkvReader* pReader, long long pos, long long size,
                    long long& result_ref) {
  if (!pReader || pos < 0 || size < 1 || size > 8)
    return E_FILE_FORMAT_INVALID;
 
  signed char first_byte = 0;
  const long status = pReader->Read(pos, 1, (unsigned char*)&first_byte);
 
  if (status < 0)
    return status;
 
  unsigned long long result = first_byte;
  ++pos;
 
   for (long i = 1; i < size; ++i) {
     unsigned char b;

 const long status = pReader->Read(pos, 1, &b);

 if (status < 0)
 return status;

    result <<= 8;
    result |= b;


     ++pos;
   }
 
  result_ref = static_cast<long long>(result);
  return 0;
 }
