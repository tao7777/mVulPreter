Cues::~Cues()
  return m_count;  // TODO: really ignore preload count?
}
 
bool Cues::DoneParsing() const {
  const long long stop = m_start + m_size;
  return (m_pos >= stop);
}
 
void Cues::Init() const {
  if (m_cue_points)
    return;
 
  assert(m_count == 0);
  assert(m_preload_count == 0);

  IMkvReader* const pReader = m_pSegment->m_pReader;

  const long long stop = m_start + m_size;
  long long pos = m_start;

  long cue_points_size = 0;

  while (pos < stop) {
    const long long idpos = pos;

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

    if (id == 0x3B)  // CuePoint ID
      PreloadCuePoint(cue_points_size, idpos);

    pos += size;  // consume payload
    assert(pos <= stop);
  }
}

void Cues::PreloadCuePoint(long& cue_points_size, long long pos) const {
  assert(m_count == 0);

  if (m_preload_count >= cue_points_size) {
    const long n = (cue_points_size <= 0) ? 2048 : 2 * cue_points_size;

    CuePoint** const qq = new CuePoint* [n];
    CuePoint** q = qq;  // beginning of target

    CuePoint** p = m_cue_points;  // beginning of source
    CuePoint** const pp = p + m_preload_count;  // end of source

    while (p != pp)
      *q++ = *p++;
 
     delete[] m_cue_points;

    m_cue_points = qq;
    cue_points_size = n;
  }

  CuePoint* const pCP = new CuePoint(m_preload_count, pos);
  m_cue_points[m_preload_count++] = pCP;
 }
