void CuePoint::Load(IMkvReader* pReader) {
 
   if (m_timecode >= 0)  // already loaded
    return;
 
   assert(m_track_positions == NULL);
   assert(m_track_positions_count == 0);

 long long pos_ = -m_timecode;
 const long long element_start = pos_;

 long long stop;


   {
     long len;
 
    const long long id = ReadUInt(pReader, pos_, len);
    assert(id == 0x3B);  // CuePoint ID
     if (id != 0x3B)
      return;
 
     pos_ += len;  // consume ID
 
 const long long size = ReadUInt(pReader, pos_, len);
    assert(size >= 0);

    pos_ += len; // consume Size field

    stop = pos_ + size;
 }

 const long long element_size = stop - element_start;

 long long pos = pos_;



   while (pos < stop) {
     long len;
 
    const long long id = ReadUInt(pReader, pos, len);
    assert(id >= 0);  // TODO
    assert((pos + len) <= stop);
 
     pos += len;  // consume ID
 
     const long long size = ReadUInt(pReader, pos, len);
    assert(size >= 0);
    assert((pos + len) <= stop);
 
     pos += len;  // consume Size field
    assert((pos + size) <= stop);
 
     if (id == 0x33)  // CueTime ID
       m_timecode = UnserializeUInt(pReader, pos, size);

 else if (id == 0x37) // CueTrackPosition(s) ID

       ++m_track_positions_count;
 
     pos += size;  // consume payload
    assert(pos <= stop);
   }
 
  assert(m_timecode >= 0);
  assert(m_track_positions_count > 0);
 
 
  m_track_positions = new TrackPosition[m_track_positions_count];
 
 
 TrackPosition* p = m_track_positions;
  pos = pos_;


   while (pos < stop) {
     long len;
 
    const long long id = ReadUInt(pReader, pos, len);
    assert(id >= 0);  // TODO
    assert((pos + len) <= stop);
 
     pos += len;  // consume ID
 
 const long long size = ReadUInt(pReader, pos, len);
    assert(size >= 0);
    assert((pos + len) <= stop);

    pos += len; // consume Size field
    assert((pos + size) <= stop);

 
     if (id == 0x37) {  // CueTrackPosition(s) ID
       TrackPosition& tp = *p++;
      tp.Parse(pReader, pos, size);
     }
 
     pos += size;  // consume payload
    assert(pos <= stop);
   }
 
   assert(size_t(p - m_track_positions) == m_track_positions_count);
 
   m_element_start = element_start;
   m_element_size = element_size;
 }
