bool Chapters::ExpandEditionsArray()
{
    if (m_editions_size > m_editions_count)
        return true;  // nothing else to do
 
    const int size = (m_editions_size == 0) ? 1 : 2 * m_editions_size;
 
    Edition* const editions = new (std::nothrow) Edition[size];
 
    if (editions == NULL)
        return false;
 
    for (int idx = 0; idx < m_editions_count; ++idx)
    {
        m_editions[idx].ShallowCopy(editions[idx]);
     }
 
    delete[] m_editions;
    m_editions = editions;
 
    m_editions_size = size;
    return true;
 }
