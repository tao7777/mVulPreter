void Chapters::Atom::ShallowCopy(Atom& rhs) const
{
    rhs.m_string_uid = m_string_uid;
    rhs.m_uid = m_uid;
    rhs.m_start_timecode = m_start_timecode;
    rhs.m_stop_timecode = m_stop_timecode;
    rhs.m_displays = m_displays;
    rhs.m_displays_size = m_displays_size;
    rhs.m_displays_count = m_displays_count;
}
