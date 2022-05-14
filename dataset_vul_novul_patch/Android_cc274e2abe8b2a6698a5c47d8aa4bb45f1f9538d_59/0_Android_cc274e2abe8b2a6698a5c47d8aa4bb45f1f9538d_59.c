void Cues::PreloadCuePoint(long& cue_points_size, long long pos) const {
bool Cues::PreloadCuePoint(long& cue_points_size, long long pos) const {
  if (m_count != 0)
    return false;
 
   if (m_preload_count >= cue_points_size) {
     const long n = (cue_points_size <= 0) ? 2048 : 2 * cue_points_size;
 
    CuePoint** const qq = new (std::nothrow) CuePoint*[n];
    if (qq == NULL)
      return false;

     CuePoint** q = qq;  // beginning of target
 
     CuePoint** p = m_cue_points;  // beginning of source
 CuePoint** const pp = p + m_preload_count; // end of source

 while (p != pp)
 *q++ = *p++;

 delete[] m_cue_points;

    m_cue_points = qq;

     cue_points_size = n;
   }
 
  CuePoint* const pCP = new (std::nothrow) CuePoint(m_preload_count, pos);
  if (pCP == NULL)
    return false;

   m_cue_points[m_preload_count++] = pCP;
  return true;
 }
