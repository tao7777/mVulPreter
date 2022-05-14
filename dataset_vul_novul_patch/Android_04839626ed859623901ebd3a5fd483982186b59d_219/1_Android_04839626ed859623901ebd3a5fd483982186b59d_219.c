Chapters::~Chapters()
{
    while (m_editions_count > 0)
    {
        Edition& e = m_editions[--m_editions_count];
        e.Clear();
    }
}
