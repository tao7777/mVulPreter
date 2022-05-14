CuePoint::CuePoint(long idx, long long pos) :
    m_element_start(0),
    m_element_size(0),
    m_index(idx),
    m_timecode(-1 * pos),
    m_track_positions(NULL),
    m_track_positions_count(0)
{
    assert(pos > 0);
 }
