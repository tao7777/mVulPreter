Tracks::~Tracks()
{
    Track** i = m_trackEntries;
    Track** const j = m_trackEntriesEnd;
    while (i != j)
    {
        Track* const pTrack = *i++;
        delete pTrack;
    }
    delete[] m_trackEntries;
}
