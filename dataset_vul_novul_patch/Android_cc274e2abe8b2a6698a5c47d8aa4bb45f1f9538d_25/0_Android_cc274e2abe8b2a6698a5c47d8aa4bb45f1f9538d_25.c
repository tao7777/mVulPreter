void Cues::Init() const {
bool Cues::Init() const {
   if (m_cue_points)
    return true;
 
  if (m_count != 0 || m_preload_count != 0)
    return false;
 
   IMkvReader* const pReader = m_pSegment->m_pReader;
 
 const long long stop = m_start + m_size;
 long long pos = m_start;

 long cue_points_size = 0;

 while (pos < stop) {
 const long long idpos = pos;

 
     long len;
 
    const long long id = ReadID(pReader, pos, len);
    if (id < 0 || (pos + len) > stop) {
      return false;
    }
 
     pos += len;  // consume ID
 
     const long long size = ReadUInt(pReader, pos, len);
    if (size < 0 || (pos + len > stop)) {
      return false;
    }
 
     pos += len;  // consume Size field
    if (pos + size > stop) {
      return false;
    }
 
    if (id == 0x3B) {  // CuePoint ID
      if (!PreloadCuePoint(cue_points_size, idpos))
        return false;
    }
 
    pos += size;  // skip payload
   }
  return true;
 }
