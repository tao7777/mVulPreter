void Cues::Init() const
    long len;
 
    const long long id = ReadUInt(pReader, m_pos, len);
    assert(id >= 0);  // TODO
    assert((m_pos + len) <= stop);
 
    m_pos += len;  // consume ID
 
    const long long size = ReadUInt(pReader, m_pos, len);
    assert(size >= 0);
    assert((m_pos + len) <= stop);
 
    m_pos += len;  // consume Size field
    assert((m_pos + size) <= stop);
 
    if (id != 0x3B) {  // CuePoint ID
      m_pos += size;  // consume payload
      assert(m_pos <= stop);
 
