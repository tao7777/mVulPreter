void Segment::AppendCluster(Cluster* pCluster) {
bool Segment::AppendCluster(Cluster* pCluster) {
  if (pCluster == NULL || pCluster->m_index < 0)
    return false;
 
   const long count = m_clusterCount + m_clusterPreloadCount;
 
   long& size = m_clusterSize;
   const long idx = pCluster->m_index;

  if (size < count || idx != m_clusterCount)
    return false;
 
   if (count >= size) {
     const long n = (size <= 0) ? 2048 : 2 * size;
 
    Cluster** const qq = new (std::nothrow) Cluster*[n];
    if (qq == NULL)
      return false;
 
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
     Cluster** const p = m_clusters + m_clusterCount;
    if (*p == NULL || (*p)->m_index >= 0)
      return false;
 
     Cluster** q = p + m_clusterPreloadCount;
    if (q >= (m_clusters + size))
      return false;
 
     for (;;) {
       Cluster** const qq = q - 1;
      if ((*qq)->m_index >= 0)
        return false;
 
       *q = *qq;
       q = qq;

 if (q == p)
 break;
 }
 }

 
   m_clusters[idx] = pCluster;
   ++m_clusterCount;
  return true;
 }
