const SeekHead::Entry* SeekHead::GetEntry(int idx) const
{
    if (idx < 0)
        return 0;
    if (idx >= m_entry_count)
        return 0;
    return m_entries + idx;
}
