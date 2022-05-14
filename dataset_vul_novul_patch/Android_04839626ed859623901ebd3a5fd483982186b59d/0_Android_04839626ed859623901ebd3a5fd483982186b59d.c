void Segment::AppendCluster(Cluster* pCluster)
  const long count = m_clusterCount + m_clusterPreloadCount;
 
  long& size = m_clusterSize;
  assert(size >= count);
 
  const long idx = pCluster->m_index;
  assert(idx == m_clusterCount);
 
  if (count >= size) {
    const long n = (size <= 0) ? 2048 : 2 * size;
 
    Cluster** const qq = new Cluster* [n];
    Cluster** q = qq;
 
    Cluster** p = m_clusters;
    Cluster** const pp = p + count;
 
    while (p != pp)
      *q++ = *p++;
 
    delete[] m_clusters;
 
    m_clusters = qq;
    size = n;
  }
 
