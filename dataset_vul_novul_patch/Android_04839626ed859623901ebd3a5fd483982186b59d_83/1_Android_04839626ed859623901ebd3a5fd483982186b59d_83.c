const CuePoint* Cues::GetFirst() const
{
    if (m_cue_points == NULL)
        return NULL;
    if (m_count == 0)
        return NULL;
 
 #if 0
     LoadCuePoint();  //init cues

 const size_t count = m_count + m_preload_count;

 if (count == 0) //weird

         return NULL;
 #endif
 
    CuePoint* const* const pp = m_cue_points;
    assert(pp);
 
    CuePoint* const pCP = pp[0];
    assert(pCP);
    assert(pCP->GetTimeCode() >= 0);
 
    return pCP;
 }
