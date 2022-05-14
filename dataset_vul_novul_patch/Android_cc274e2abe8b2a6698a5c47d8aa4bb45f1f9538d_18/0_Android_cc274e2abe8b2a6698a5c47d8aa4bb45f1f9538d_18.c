 const CuePoint* Cues::GetLast() const {
  if (m_cue_points == NULL || m_count <= 0)
     return NULL;
 
   const long index = m_count - 1;
 
   CuePoint* const* const pp = m_cue_points;
  if (pp == NULL)
    return NULL;
 
   CuePoint* const pCP = pp[index];
  if (pCP == NULL || pCP->GetTimeCode() < 0)
    return NULL;
 
   return pCP;
 }
