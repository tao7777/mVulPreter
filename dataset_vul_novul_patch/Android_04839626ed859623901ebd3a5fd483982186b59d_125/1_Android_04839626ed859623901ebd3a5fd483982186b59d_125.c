long long Block::GetTime(const Cluster* pCluster) const
{
    assert(pCluster);
 
    const long long tc = GetTimeCode(pCluster);
 
    const Segment* const pSegment = pCluster->m_pSegment;
    const SegmentInfo* const pInfo = pSegment->GetInfo();
    assert(pInfo);
 
    const long long scale = pInfo->GetTimeCodeScale();
    assert(scale >= 1);
 
    const long long ns = tc * scale;
    return ns;
 }
