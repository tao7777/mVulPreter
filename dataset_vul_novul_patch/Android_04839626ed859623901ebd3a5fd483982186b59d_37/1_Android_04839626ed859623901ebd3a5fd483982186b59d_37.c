bool Chapters::Atom::ExpandDisplaysArray()
{
    if (m_displays_size > m_displays_count)
        return true;  // nothing else to do
    const int size = (m_displays_size == 0) ? 1 : 2 * m_displays_size;
    Display* const displays = new (std::nothrow) Display[size];
    if (displays == NULL)
        return false;
    for (int idx = 0; idx < m_displays_count; ++idx)
    {
        m_displays[idx].ShallowCopy(displays[idx]);
    }
    delete[] m_displays;
    m_displays = displays;
    m_displays_size = size;
    return true;
}
