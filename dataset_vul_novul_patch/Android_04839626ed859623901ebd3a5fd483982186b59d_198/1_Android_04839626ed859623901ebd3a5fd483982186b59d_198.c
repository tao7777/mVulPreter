long VideoTrack::Seek(
    long long time_ns,
    const BlockEntry*& pResult) const
{
    const long status = GetFirst(pResult);
    if (status < 0)  //buffer underflow, etc
         return status;
 
    assert(pResult);
 
    if (pResult->EOS())
        return 0;
 
    const Cluster* pCluster = pResult->GetCluster();
     assert(pCluster);
     assert(pCluster->GetIndex() >= 0);
 
    if (time_ns <= pResult->GetBlock()->GetTime(pCluster))
        return 0;
 
    Cluster** const clusters = m_pSegment->m_clusters;
    assert(clusters);
 
    const long count = m_pSegment->GetCount();  //loaded only, not pre-loaded
    assert(count > 0);
 
    Cluster** const i = clusters + pCluster->GetIndex();
    assert(i);
    assert(*i == pCluster);
    assert(pCluster->GetTime() <= time_ns);
 
    Cluster** const j = clusters + count;
 
    Cluster** lo = i;
    Cluster** hi = j;
 
    while (lo < hi)
    {
 
        Cluster** const mid = lo + (hi - lo) / 2;
        assert(mid < hi);
        pCluster = *mid;
        assert(pCluster);
        assert(pCluster->GetIndex() >= 0);
        assert(pCluster->GetIndex() == long(mid - m_pSegment->m_clusters));
        const long long t = pCluster->GetTime();
        if (t <= time_ns)
            lo = mid + 1;
        else
            hi = mid;
        assert(lo <= hi);
    }
    assert(lo == hi);
    assert(lo > i);
    assert(lo <= j);
     pCluster = *--lo;
     assert(pCluster);
     assert(pCluster->GetTime() <= time_ns);
 
    pResult = pCluster->GetEntry(this, time_ns);
    if ((pResult != 0) && !pResult->EOS())  //found a keyframe
        return 0;
    while (lo != i)
    {
        pCluster = *--lo;
        assert(pCluster);
        assert(pCluster->GetTime() <= time_ns);
 #if 0

         pResult = pCluster->GetMaxKey(this);
 #else
        pResult = pCluster->GetEntry(this, time_ns);
 #endif
 
        if ((pResult != 0) && !pResult->EOS())
            return 0;
     }
 
 
    pResult = GetEOS();
    return 0;
 }
