 const CuePoint* Cues::GetFirst() const {
  if (m_cue_points == NULL || m_count == 0)
     return NULL;
 
   CuePoint* const* const pp = m_cue_points;
  if (pp == NULL)
    return NULL;
 
   CuePoint* const pCP = pp[0];
  if (pCP == NULL || pCP->GetTimeCode() < 0)
    return NULL;
 
   return pCP;
 }
