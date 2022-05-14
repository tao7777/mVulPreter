bool mkvparser::Match(
bool mkvparser::Match(IMkvReader* pReader, long long& pos, unsigned long id_,
                      unsigned char*& buf, size_t& buflen) {
  assert(pReader);
  assert(pos >= 0);
 
  long long total, available;
 
  long status = pReader->Length(&total, &available);
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
 
  const long long size_ = ReadUInt(pReader, pos, len);
  assert(size_ >= 0);
  assert(len > 0);
  assert(len <= 8);
  assert((pos + len) <= available);
 
  pos += len;  // consume length of size of payload
  assert((pos + size_) <= available);
 
  const long buflen_ = static_cast<long>(size_);
 
  buf = new (std::nothrow) unsigned char[buflen_];
  assert(buf);  // TODO
 
  status = pReader->Read(pos, buflen_, buf);
  assert(status == 0);  // TODO
 
  buflen = buflen_;
 
  pos += size_;  // consume size of payload
  return true;
 }
