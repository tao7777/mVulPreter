const Track* Tracks::GetTrackByNumber(long tn) const
{
    if (tn < 0)
        return NULL;
    Track** i = m_trackEntries;
    Track** const j = m_trackEntriesEnd;
    while (i != j)
    {
        Track* const pTrack = *i++;
        if (pTrack == NULL)
            continue;
        if (tn == pTrack->GetNumber())
            return pTrack;
    }
    return NULL;  //not found
}
