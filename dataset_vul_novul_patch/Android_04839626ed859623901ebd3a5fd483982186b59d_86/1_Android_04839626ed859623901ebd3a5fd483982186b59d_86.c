const Block::Frame& Block::GetFrame(int idx) const
{
    assert(idx >= 0);
    assert(idx < m_frame_count);
    const Frame& f = m_frames[idx];
    assert(f.pos > 0);
    assert(f.len > 0);
    return f;
}
