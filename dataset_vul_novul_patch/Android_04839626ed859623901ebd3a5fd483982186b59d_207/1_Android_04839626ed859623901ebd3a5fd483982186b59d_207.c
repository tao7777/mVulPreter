Track::Track(
    Segment* pSegment,
    long long element_start,
    long long element_size) :
    m_pSegment(pSegment),
    m_element_start(element_start),
    m_element_size(element_size),
    content_encoding_entries_(NULL),
    content_encoding_entries_end_(NULL)
{
 }
