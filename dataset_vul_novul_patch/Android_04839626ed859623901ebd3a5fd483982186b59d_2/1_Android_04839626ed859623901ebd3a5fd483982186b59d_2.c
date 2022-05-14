Block::Block(long long start, long long size_, long long discard_padding) :
    m_start(start),
    m_size(size_),
    m_track(0),
    m_timecode(-1),
    m_flags(0),
    m_frames(NULL),
    m_frame_count(-1),
    m_discard_padding(discard_padding)
{
}
