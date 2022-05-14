const Cluster* Segment::GetNext(const Cluster* pCurr)
  if (idx >= 0) {
    assert(m_clusterCount > 0);
    assert(idx < m_clusterCount);
    assert(pCurr == m_clusters[idx]);
 
    ++idx;
 
    if (idx >= m_clusterCount)
      return &m_eos;  // caller will LoadCluster as desired
 
    Cluster* const pNext = m_clusters[idx];
     assert(pNext);
    assert(pNext->m_index >= 0);
    assert(pNext->m_index == idx);
 
     return pNext;
