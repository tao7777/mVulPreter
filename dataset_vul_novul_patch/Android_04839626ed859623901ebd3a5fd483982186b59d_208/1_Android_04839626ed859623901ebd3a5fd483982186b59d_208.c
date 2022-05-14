Tracks::Tracks(
    Segment* pSegment,
    long long start,
    long long size_,
    long long element_start,
    long long element_size) :
    m_pSegment(pSegment),
    m_start(start),
    m_size(size_),
    m_element_start(element_start),
    m_element_size(element_size),
    m_trackEntries(NULL),
    m_trackEntriesEnd(NULL)
{
}
