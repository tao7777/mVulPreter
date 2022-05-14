const BlockEntry* Segment::GetBlock(
    const CuePoint& cp,
    const CuePoint::TrackPosition& tp)
{
    Cluster** const ii = m_clusters;
    Cluster** i = ii;
 
    const long count = m_clusterCount + m_clusterPreloadCount;
 
    Cluster** const jj = ii + count;
    Cluster** j = jj;
 
    while (i < j)
    {
 
        Cluster** const k = i + (j - i) / 2;
        assert(k < jj);
        Cluster* const pCluster = *k;
        assert(pCluster);
        const long long pos = pCluster->GetPosition();
        assert(pos >= 0);
        if (pos < tp.m_pos)
            i = k + 1;
        else if (pos > tp.m_pos)
            j = k;
        else
            return pCluster->GetEntry(cp, tp);
    }
    assert(i == j);
    Cluster* const pCluster = Cluster::Create(this, -1, tp.m_pos); //, -1);
     assert(pCluster);
 
    const ptrdiff_t idx = i - m_clusters;
 
    PreloadCluster(pCluster, idx);
    assert(m_clusters);
    assert(m_clusterPreloadCount > 0);
    assert(m_clusters[idx] == pCluster);
 
    return pCluster->GetEntry(cp, tp);
 }
