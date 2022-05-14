 bool Cues::LoadCuePoint() const {
   const long long stop = m_start + m_size;
 
   if (m_pos >= stop)
     return false;  // nothing else to do
 
  if (!Init()) {
    m_pos = stop;
    return false;
  }
 
   IMkvReader* const pReader = m_pSegment->m_pReader;
 
 while (m_pos < stop) {
 const long long idpos = m_pos;

 
     long len;
 
    const long long id = ReadID(pReader, m_pos, len);
    if (id < 0 || (m_pos + len) > stop)
      return false;
 
     m_pos += len;  // consume ID
 
     const long long size = ReadUInt(pReader, m_pos, len);
    if (size < 0 || (m_pos + len) > stop)
      return false;
 
     m_pos += len;  // consume Size field
    if ((m_pos + size) > stop)
      return false;
 
     if (id != 0x3B) {  // CuePoint ID
       m_pos += size;  // consume payload
      if (m_pos > stop)
        return false;
 
       continue;
     }
 
    if (m_preload_count < 1)
       return false;
 
    CuePoint* const pCP = m_cue_points[m_count];
    if (!pCP || (pCP->GetTimeCode() < 0 && (-pCP->GetTimeCode() != idpos)))
      return false;

    if (!pCP->Load(pReader)) {
      m_pos = stop;
      return false;
    }
     ++m_count;
     --m_preload_count;
 
     m_pos += size;  // consume payload
    if (m_pos > stop)
      return false;
 
     return true;  // yes, we loaded a cue point
   }
 
 }
