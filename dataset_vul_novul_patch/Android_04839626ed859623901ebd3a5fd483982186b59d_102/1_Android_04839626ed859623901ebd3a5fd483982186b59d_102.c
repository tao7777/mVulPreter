const Cluster* Segment::GetLast() const
{
    if ((m_clusters == NULL) || (m_clusterCount <= 0))
        return &m_eos;
 
    const long idx = m_clusterCount - 1;
 
    Cluster* const pCluster = m_clusters[idx];
    assert(pCluster);
    return pCluster;
 }
