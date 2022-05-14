void Cues::PreloadCuePoint(long& cue_points_size, long long pos) const {
  assert(m_count == 0);
 
   if (m_preload_count >= cue_points_size) {
     const long n = (cue_points_size <= 0) ? 2048 : 2 * cue_points_size;
 
    CuePoint** const qq = new CuePoint* [n];
     CuePoint** q = qq;  // beginning of target
 
     CuePoint** p = m_cue_points;  // beginning of source
 CuePoint** const pp = p + m_preload_count; // end of source

 while (p != pp)
 *q++ = *p++;

 delete[] m_cue_points;

    m_cue_points = qq;

     cue_points_size = n;
   }
 
  CuePoint* const pCP = new CuePoint(m_preload_count, pos);
   m_cue_points[m_preload_count++] = pCP;
 }
