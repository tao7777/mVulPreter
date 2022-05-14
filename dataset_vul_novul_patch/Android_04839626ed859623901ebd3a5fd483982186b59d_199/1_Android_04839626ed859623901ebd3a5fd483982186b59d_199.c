SeekHead::SeekHead(
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
    m_entries(0),
    m_entry_count(0),
    m_void_elements(0),
    m_void_element_count(0)
{
 }
