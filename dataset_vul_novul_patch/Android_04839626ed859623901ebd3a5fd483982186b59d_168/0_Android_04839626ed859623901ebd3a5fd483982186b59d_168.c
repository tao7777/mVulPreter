long VideoTrack::Parse(
  IMkvReader* const pReader = pSegment->m_pReader;
 
  const Settings& s = info.settings;
  assert(s.start >= 0);
  assert(s.size >= 0);
 
  long long pos = s.start;
  assert(pos >= 0);
 
  const long long stop = pos + s.size;
 
  while (pos < stop) {
    long long id, size;
 
    const long status = ParseElementHeader(pReader, pos, stop, id, size);
 
    if (status < 0)  // error
      return status;
 
    if (id == 0x30) {  // pixel width
      width = UnserializeUInt(pReader, pos, size);
 
      if (width <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x3A) {  // pixel height
      height = UnserializeUInt(pReader, pos, size);
 
      if (height <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x0383E3) {  // frame rate
      const long status = UnserializeFloat(pReader, pos, size, rate);
 
