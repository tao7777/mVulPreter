bool Track::VetEntry(const BlockEntry* pBlockEntry) const
{
    assert(pBlockEntry);
    const Block* const pBlock = pBlockEntry->GetBlock();
    assert(pBlock);
    assert(pBlock->GetTrackNumber() == m_info.number);
    if (!pBlock || pBlock->GetTrackNumber() != m_info.number)
        return false;
 
 
    return true;
 }
