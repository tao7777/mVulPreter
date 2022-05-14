long Cluster::GetLast(const BlockEntry*& pLast) const
{
    for (;;)
    {
        long long pos;
        long len;
        const long status = Parse(pos, len);
        if (status < 0)  //error
        {
            pLast = NULL;
            return status;
        }
        if (status > 0)  //no new block
            break;
    }
    if (m_entries_count <= 0)
    {
        pLast = NULL;
        return 0;
    }
    assert(m_entries);
    const long idx = m_entries_count - 1;
    pLast = m_entries[idx];
    assert(pLast);
     return 0;
 }
