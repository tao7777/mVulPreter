const BlockEntry* Cues::GetBlock(
  if (pTP == NULL)
    return NULL;
 
  return m_pSegment->GetBlock(*pCP, *pTP);
 }
