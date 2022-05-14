long long Cluster::GetTime() const
{
    const long long tc = GetTimeCode();
    if (tc < 0)
        return tc;
    const SegmentInfo* const pInfo = m_pSegment->GetInfo();
    assert(pInfo);
    const long long scale = pInfo->GetTimeCodeScale();
    assert(scale >= 1);
    const long long t = m_timecode * scale;
    return t;
}
