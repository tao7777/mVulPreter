Segment::~Segment()
  assert((total < 0) || (available <= total));
 
  const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;
  assert((segment_stop < 0) || (total < 0) || (segment_stop <= total));
  assert((segment_stop < 0) || (m_pos <= segment_stop));
 
  for (;;) {
    if ((total >= 0) && (m_pos >= total))
      break;
 
    if ((segment_stop >= 0) && (m_pos >= segment_stop))
      break;
 
    long long pos = m_pos;
    const long long element_start = pos;
 
