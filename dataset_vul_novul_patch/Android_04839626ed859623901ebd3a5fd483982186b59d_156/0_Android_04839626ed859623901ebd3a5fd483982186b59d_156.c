long Segment::Load()
  const long count = m_clusterCount + m_clusterPreloadCount;
 
  long& size = m_clusterSize;
  assert(size >= count);
 
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
 
  assert(m_clusters);

  Cluster** const p = m_clusters + idx;

  Cluster** q = m_clusters + count;
  assert(q >= p);
  assert(q < (m_clusters + size));

  while (q > p) {
    Cluster** const qq = q - 1;
    assert((*qq)->m_index < 0);

    *q = *qq;
    q = qq;
  }

  m_clusters[idx] = pCluster;
  ++m_clusterPreloadCount;
 }
