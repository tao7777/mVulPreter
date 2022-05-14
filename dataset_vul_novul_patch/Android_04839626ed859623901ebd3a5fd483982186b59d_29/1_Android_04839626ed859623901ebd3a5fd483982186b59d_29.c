bool Cues::DoneParsing() const
{
    const long long stop = m_start + m_size;
    return (m_pos >= stop);
}
