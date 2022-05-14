long long Chapters::Atom::GetTime(
    const Chapters* pChapters,
    long long timecode)
{
    if (pChapters == NULL)
        return -1;
    Segment* const pSegment = pChapters->m_pSegment;
    if (pSegment == NULL)  // weird
        return -1;
    const SegmentInfo* const pInfo = pSegment->GetInfo();
    if (pInfo == NULL)
        return -1;
    const long long timecode_scale = pInfo->GetTimeCodeScale();
    if (timecode_scale < 1)  // weird
        return -1;
    if (timecode < 0)
        return -1;
    const long long result = timecode_scale * timecode;
    return result;
}
