long Cluster::CreateSimpleBlock(
    long long st,
    long long sz)
{
    assert(m_entries);
    assert(m_entries_size > 0);
    assert(m_entries_count >= 0);
    assert(m_entries_count < m_entries_size);
    const long idx = m_entries_count;
    BlockEntry** const ppEntry = m_entries + idx;
    BlockEntry*& pEntry = *ppEntry;
    pEntry = new (std::nothrow) SimpleBlock(this, idx, st, sz);
    if (pEntry == NULL)
        return -1;  //generic error
    SimpleBlock* const p = static_cast<SimpleBlock*>(pEntry);
    const long status = p->Parse();
    if (status == 0)
    {
        ++m_entries_count;
        return 0;
    }
    delete pEntry;
    pEntry = 0;
    return status;
}
