bool VideoTrack::VetEntry(const BlockEntry* pBlockEntry) const
{
    return Track::VetEntry(pBlockEntry) && pBlockEntry->GetBlock()->IsKey();
}
