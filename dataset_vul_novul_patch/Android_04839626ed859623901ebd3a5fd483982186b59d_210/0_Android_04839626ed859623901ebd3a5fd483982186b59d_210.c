long mkvparser::UnserializeInt(
  if (size_ >= LONG_MAX)  // we need (size+1) chars
    return E_FILE_FORMAT_INVALID;
 
  const long size = static_cast<long>(size_);
 
  str = new (std::nothrow) char[size + 1];
 
  if (str == NULL)
    return -1;
 
  unsigned char* const buf = reinterpret_cast<unsigned char*>(str);
 
  const long status = pReader->Read(pos, size, buf);
 
