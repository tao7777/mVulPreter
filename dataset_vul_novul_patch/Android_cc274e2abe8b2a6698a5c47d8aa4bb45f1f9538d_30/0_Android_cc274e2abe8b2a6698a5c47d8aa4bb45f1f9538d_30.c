bool mkvparser::Match(IMkvReader* pReader, long long& pos, unsigned long id_,
bool Match(IMkvReader* pReader, long long& pos, unsigned long expected_id,
           long long& val) {
  if (!pReader || pos < 0)
     return false;
 
  long long total = 0;
  long long available = 0;
 
  const long status = pReader->Length(&total, &available);
  if (status < 0 || (total >= 0 && available > total))
    return false;
 
  long len = 0;

  const long long id = ReadID(pReader, pos, len);
  if (id < 0 || (available - pos) > len)
    return false;

  if (static_cast<unsigned long>(id) != expected_id)
     return false;
 
   pos += len;  // consume id
 
   const long long size = ReadUInt(pReader, pos, len);
  if (size < 0 || size > 8 || len < 1 || len > 8 || (available - pos) > len)
    return false;
 
   pos += len;  // consume length of size of payload
 
   val = UnserializeUInt(pReader, pos, size);
  if (val < 0)
    return false;
 
   pos += size;  // consume size of payload
 
   return true;
 }
