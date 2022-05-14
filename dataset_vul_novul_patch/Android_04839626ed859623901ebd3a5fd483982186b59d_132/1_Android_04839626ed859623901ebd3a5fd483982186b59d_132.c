const Track* Tracks::GetTrackByIndex(unsigned long idx) const
{
    const ptrdiff_t count = m_trackEntriesEnd - m_trackEntries;
    if (idx >= static_cast<unsigned long>(count))
         return NULL;
    return m_trackEntries[idx];
 }
