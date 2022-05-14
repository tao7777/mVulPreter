const Cluster* Segment::GetFirst() const
  Cluster* const pCluster = m_clusters[0];
  assert(pCluster);
 
  return pCluster;
 }
