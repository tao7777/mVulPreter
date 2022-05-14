long long CuePoint::GetTime(const Segment* pSegment) const
{
    assert(pSegment);
    assert(m_timecode >= 0);
    const SegmentInfo* const pInfo = pSegment->GetInfo();
    assert(pInfo);
    const long long scale = pInfo->GetTimeCodeScale();
    assert(scale >= 1);
    const long long time = scale * m_timecode;
    return time;
}
