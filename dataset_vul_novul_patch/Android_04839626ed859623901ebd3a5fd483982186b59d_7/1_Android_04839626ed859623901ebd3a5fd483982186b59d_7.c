void Chapters::Atom::Clear()
{
    delete[] m_string_uid;
    m_string_uid = NULL;
    while (m_displays_count > 0)
    {
        Display& d = m_displays[--m_displays_count];
        d.Clear();
    }
    delete[] m_displays;
    m_displays = NULL;
    m_displays_size = 0;
}
