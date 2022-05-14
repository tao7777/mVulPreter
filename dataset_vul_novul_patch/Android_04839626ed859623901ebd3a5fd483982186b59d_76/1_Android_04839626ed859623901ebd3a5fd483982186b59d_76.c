long Cluster::GetEntry(long index, const mkvparser::BlockEntry*& pEntry) const
{
    assert(m_pos >= m_element_start);
    pEntry = NULL;
    if (index < 0)
        return -1;  //generic error
    if (m_entries_count < 0)
        return E_BUFFER_NOT_FULL;
    assert(m_entries);
    assert(m_entries_size > 0);
    assert(m_entries_count <= m_entries_size);
    if (index < m_entries_count)
    {
        pEntry = m_entries[index];
        assert(pEntry);
        return 1;  //found entry
     }
 
    if (m_element_size < 0)        //we don't know cluster end yet
        return E_BUFFER_NOT_FULL;  //underflow
 
    const long long element_stop = m_element_start + m_element_size;
 
    if (m_pos >= element_stop)
        return 0;  //nothing left to parse
    return E_BUFFER_NOT_FULL;  //underflow, since more remains to be parsed
}
