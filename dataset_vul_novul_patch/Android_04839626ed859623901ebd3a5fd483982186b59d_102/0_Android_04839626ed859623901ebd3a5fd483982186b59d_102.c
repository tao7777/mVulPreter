const Cluster* Segment::GetLast() const
  const long idx = m_clusterCount - 1;
 
  Cluster* const pCluster = m_clusters[idx];
  assert(pCluster);
 
  return pCluster;
 }
