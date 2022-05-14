const Cluster* Segment::FindOrPreloadCluster(long long requested_pos) {
 if (requested_pos < 0)
 return 0;

 Cluster** const ii = m_clusters;
 Cluster** i = ii;

 const long count = m_clusterCount + m_clusterPreloadCount;

 Cluster** const jj = ii + count;
 Cluster** j = jj;

 while (i < j) {

 Cluster** const k = i + (j - i) / 2;
    assert(k < jj);

 Cluster* const pCluster = *k;
    assert(pCluster);


 const long long pos = pCluster->GetPosition();
    assert(pos >= 0);

 if (pos < requested_pos)
      i = k + 1;
 else if (pos > requested_pos)
      j = k;
 else
 return pCluster;
 }

  assert(i == j);

 
   Cluster* const pCluster = Cluster::Create(this, -1, requested_pos);
  if (pCluster == NULL)
    return NULL;
 
   const ptrdiff_t idx = i - m_clusters;
 
  if (!PreloadCluster(pCluster, idx)) {
    delete pCluster;
    return NULL;
  }
   assert(m_clusters);
   assert(m_clusterPreloadCount > 0);
   assert(m_clusters[idx] == pCluster);

 return pCluster;
}
