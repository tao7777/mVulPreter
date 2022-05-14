void Chapters::Display::ShallowCopy(Display& rhs) const
{
    rhs.m_string = m_string;
    rhs.m_language = m_language;
    rhs.m_country = m_country;
}
