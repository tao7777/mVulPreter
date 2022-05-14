void Segment::PreloadCluster(Cluster* pCluster, ptrdiff_t idx)
  if (m_clusterPreloadCount > 0) {
     assert(m_clusters);
 
    Cluster** const p = m_clusters + m_clusterCount;
    assert(*p);
    assert((*p)->m_index < 0);
 
    Cluster** q = p + m_clusterPreloadCount;
     assert(q < (m_clusters + size));
 
    for (;;) {
      Cluster** const qq = q - 1;
      assert((*qq)->m_index < 0);
 
      *q = *qq;
      q = qq;

      if (q == p)
        break;
     }
  }
 
  m_clusters[idx] = pCluster;
  ++m_clusterCount;
 }
