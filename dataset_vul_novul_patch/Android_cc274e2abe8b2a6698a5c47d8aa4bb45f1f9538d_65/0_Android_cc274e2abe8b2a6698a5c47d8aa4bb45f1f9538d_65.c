long mkvparser::UnserializeString(IMkvReader* pReader, long long pos,
long UnserializeString(IMkvReader* pReader, long long pos, long long size,
                       char*& str) {
   delete[] str;
   str = NULL;
 
  if (size >= LONG_MAX || size < 0)
     return E_FILE_FORMAT_INVALID;
 
  // +1 for '\0' terminator
  const long required_size = static_cast<long>(size) + 1;
 
  str = SafeArrayAlloc<char>(1, required_size);
   if (str == NULL)
    return E_FILE_FORMAT_INVALID;
 
   unsigned char* const buf = reinterpret_cast<unsigned char*>(str);
 
 const long status = pReader->Read(pos, size, buf);

 if (status) {
 delete[] str;
    str = NULL;


     return status;
   }
 
  str[required_size - 1] = '\0';
  return 0;
 }
