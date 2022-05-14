Cues::Cues(
    Segment* pSegment,
    long long start_,
    long long size_,
    long long element_start,
    long long element_size) :
    m_pSegment(pSegment),
    m_start(start_),
    m_size(size_),
    m_element_start(element_start),
    m_element_size(element_size),
    m_cue_points(NULL),
    m_count(0),
    m_preload_count(0),
    m_pos(start_)
{
 }
