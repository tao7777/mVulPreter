 const CuePoint* Cues::GetNext(const CuePoint* pCurr) const {
  if (pCurr == NULL || pCurr->GetTimeCode() < 0 ||
      m_cue_points == NULL || m_count < 1) {
    return NULL;
  }

  long index = pCurr->m_index;
  if (index >= m_count)
     return NULL;
 
   CuePoint* const* const pp = m_cue_points;
  if (pp == NULL || pp[index] != pCurr)
    return NULL;
 
   ++index;
 
 if (index >= m_count)

     return NULL;
 
   CuePoint* const pNext = pp[index];

  if (pNext == NULL || pNext->GetTimeCode() < 0)
    return NULL;
 
   return pNext;
 }
