bool mkvparser::Match(
  long long total, available;
 
  const long status = pReader->Length(&total, &available);
  assert(status >= 0);
  assert((total < 0) || (available <= total));
  if (status < 0)
    return false;
 
  long len;
 
  const long long id = ReadUInt(pReader, pos, len);
  assert(id >= 0);
  assert(len > 0);
  assert(len <= 8);
  assert((pos + len) <= available);
 
  if ((unsigned long)id != id_)
    return false;
 
  pos += len;  // consume id
 
  const long long size = ReadUInt(pReader, pos, len);
  assert(size >= 0);
  assert(size <= 8);
  assert(len > 0);
  assert(len <= 8);
  assert((pos + len) <= available);
 
  pos += len;  // consume length of size of payload
 
  val = UnserializeUInt(pReader, pos, size);
  assert(val >= 0);
 
  pos += size;  // consume size of payload
 
  return true;
 }
