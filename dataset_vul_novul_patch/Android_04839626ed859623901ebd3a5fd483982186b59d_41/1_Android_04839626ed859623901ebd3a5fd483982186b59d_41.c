const Cluster* Segment::FindCluster(long long time_ns) const
{
    if ((m_clusters == NULL) || (m_clusterCount <= 0))
        return &m_eos;
 
    {
        Cluster* const pCluster = m_clusters[0];
        assert(pCluster);
        assert(pCluster->m_index == 0);
 
        if (time_ns <= pCluster->GetTime())
            return pCluster;
    }
 
 
    long i = 0;
    long j = m_clusterCount;
 
    while (i < j)
    {
        const long k = i + (j - i) / 2;
        assert(k < m_clusterCount);
        Cluster* const pCluster = m_clusters[k];
        assert(pCluster);
        assert(pCluster->m_index == k);
        const long long t = pCluster->GetTime();
        if (t <= time_ns)
            i = k + 1;
        else
            j = k;
        assert(i <= j);
    }
    assert(i == j);
    assert(i > 0);
    assert(i <= m_clusterCount);
    const long k = i - 1;
 
     Cluster* const pCluster = m_clusters[k];
     assert(pCluster);
     assert(pCluster->m_index == k);
    assert(pCluster->GetTime() <= time_ns);
 
    return pCluster;
 }
