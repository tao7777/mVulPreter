long mkvparser::ParseElementHeader(
  long len;
 
  id = ReadUInt(pReader, pos, len);
 
  if (id < 0)
    return E_FILE_FORMAT_INVALID;
 
  pos += len;  // consume id
 
  if ((stop >= 0) && (pos >= stop))
    return E_FILE_FORMAT_INVALID;
 
  size = ReadUInt(pReader, pos, len);
 
  if (size < 0)
    return E_FILE_FORMAT_INVALID;
 
  pos += len;  // consume length of size
 
  // pos now designates payload
 
  if ((stop >= 0) && ((pos + size) > stop))
    return E_FILE_FORMAT_INVALID;
 
  return 0;  // success
 }
