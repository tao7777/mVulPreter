void Chapters::Display::Clear()
{
    delete[] m_string;
    m_string = NULL;
    delete[] m_language;
    m_language = NULL;
    delete[] m_country;
    m_country = NULL;
}
