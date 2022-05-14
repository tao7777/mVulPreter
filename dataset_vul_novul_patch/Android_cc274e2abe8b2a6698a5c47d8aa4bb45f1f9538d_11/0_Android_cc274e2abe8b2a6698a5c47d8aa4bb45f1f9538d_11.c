 const BlockEntry* Cues::GetBlock(const CuePoint* pCP,
                                  const CuePoint::TrackPosition* pTP) const {
  if (pCP == NULL || pTP == NULL)
     return NULL;
 
   return m_pSegment->GetBlock(*pCP, *pTP);
}
