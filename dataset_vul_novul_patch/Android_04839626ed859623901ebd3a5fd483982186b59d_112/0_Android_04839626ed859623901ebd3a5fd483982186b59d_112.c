long long Cluster::GetPosition() const
long Cluster::GetIndex() const { return m_index; }
 
long long Cluster::GetPosition() const {
  const long long pos = m_element_start - m_pSegment->m_start;
  assert(pos >= 0);

  return pos;
 }
