bool Track::VetEntry(const BlockEntry* pBlockEntry) const
bool Track::VetEntry(const BlockEntry* pBlockEntry) const {
  assert(pBlockEntry);
  const Block* const pBlock = pBlockEntry->GetBlock();
  assert(pBlock);
  assert(pBlock->GetTrackNumber() == m_info.number);
  if (!pBlock || pBlock->GetTrackNumber() != m_info.number)
    return false;
 
  // This function is used during a seek to determine whether the
  // frame is a valid seek target.  This default function simply
  // returns true, which means all frames are valid seek targets.
  // It gets overridden by the VideoTrack class, because only video
  // keyframes can be used as seek target.
 
  return true;
 }
