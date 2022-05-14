unsigned long Tracks::GetTracksCount() const
{
    const ptrdiff_t result = m_trackEntriesEnd - m_trackEntries;
    assert(result >= 0);
    return static_cast<unsigned long>(result);
}
