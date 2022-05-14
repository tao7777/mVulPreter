 bool Cues::Find(long long time_ns, const Track* pTrack, const CuePoint*& pCP,
                 const CuePoint::TrackPosition*& pTP) const {
  if (time_ns < 0 || pTrack == NULL || m_cue_points == NULL || m_count == 0)
     return false;
 
   CuePoint** const ii = m_cue_points;
 CuePoint** i = ii;

 CuePoint** const jj = ii + m_count;

   CuePoint** j = jj;
 
   pCP = *i;
  if (pCP == NULL)
    return false;
 
   if (time_ns <= pCP->GetTime(m_pSegment)) {
     pTP = pCP->Find(pTrack);
 return (pTP != NULL);
 }

 while (i < j) {

 
     CuePoint** const k = i + (j - i) / 2;
    if (k >= jj)
      return false;
 
     CuePoint* const pCP = *k;
    if (pCP == NULL)
      return false;
 
     const long long t = pCP->GetTime(m_pSegment);
 
 if (t <= time_ns)
      i = k + 1;

     else
       j = k;
 
    if (i > j)
      return false;
   }
 
  if (i != j || i > jj || i <= ii)
    return false;
 
   pCP = *--i;

  if (pCP == NULL || pCP->GetTime(m_pSegment) > time_ns)
    return false;
 

  pTP = pCP->Find(pTrack);

   return (pTP != NULL);
 }
