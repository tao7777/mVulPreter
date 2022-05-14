bool mkvparser::Match(IMkvReader* pReader, long long& pos, unsigned long id_,
bool Match(IMkvReader* pReader, long long& pos, unsigned long expected_id,
           unsigned char*& buf, size_t& buflen) {
  if (!pReader || pos < 0)
     return false;
 
  long long total = 0;
  long long available = 0;
 
  long status = pReader->Length(&total, &available);
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
  if (size < 0 || len <= 0 || len > 8 || (available - pos) > len)
    return false;

  unsigned long long rollover_check =
      static_cast<unsigned long long>(pos) + len;
  if (rollover_check > LONG_LONG_MAX)
    return false;
 
   pos += len;  // consume length of size of payload
 
  rollover_check = static_cast<unsigned long long>(pos) + size;
  if (rollover_check > LONG_LONG_MAX)
    return false;
 
  if ((pos + size) > available)
    return false;

  if (size >= LONG_MAX)
    return false;

  const long buflen_ = static_cast<long>(size);

  buf = SafeArrayAlloc<unsigned char>(1, buflen_);
  if (!buf)
    return false;
 
   status = pReader->Read(pos, buflen_, buf);
  if (status != 0)
    return false;
 
   buflen = buflen_;
 
  pos += size;  // consume size of payload
   return true;
 }
