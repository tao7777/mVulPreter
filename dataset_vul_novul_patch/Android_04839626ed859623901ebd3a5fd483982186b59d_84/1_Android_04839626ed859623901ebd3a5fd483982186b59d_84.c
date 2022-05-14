const Cluster* Segment::GetFirst() const
{
    if ((m_clusters == NULL) || (m_clusterCount <= 0))
       return &m_eos;
 
    Cluster* const pCluster = m_clusters[0];
    assert(pCluster);
    return pCluster;
 }
