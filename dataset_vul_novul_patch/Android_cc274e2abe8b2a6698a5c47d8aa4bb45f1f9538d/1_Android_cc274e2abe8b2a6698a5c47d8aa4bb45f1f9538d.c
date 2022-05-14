void Segment::AppendCluster(Cluster* pCluster) {
  assert(pCluster);
  assert(pCluster->m_index >= 0);
 
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
