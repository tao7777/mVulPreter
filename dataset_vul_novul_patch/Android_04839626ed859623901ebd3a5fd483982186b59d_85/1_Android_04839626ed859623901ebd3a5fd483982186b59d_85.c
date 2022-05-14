long long Cluster::GetFirstTime() const
{
    const BlockEntry* pEntry;
    const long status = GetFirst(pEntry);
    if (status < 0)  //error
        return status;
    if (pEntry == NULL)  //empty cluster
        return GetTime();
    const Block* const pBlock = pEntry->GetBlock();
    assert(pBlock);
    return pBlock->GetTime(this);
}
