const CuePoint* Cues::GetNext(const CuePoint* pCurr) const
{
    if (pCurr == NULL)
        return NULL;
    assert(pCurr->GetTimeCode() >= 0);
    assert(m_cue_points);
    assert(m_count >= 1);
 
 #if 0
     const size_t count = m_count + m_preload_count;

 size_t index = pCurr->m_index;
    assert(index < count);

 CuePoint* const* const pp = m_cue_points;
    assert(pp);
    assert(pp[index] == pCurr);

 ++index;

 if (index >= count)
 return NULL;

 CuePoint* const pNext = pp[index];
    assert(pNext);

 
     pNext->Load(m_pSegment->m_pReader);
 #else
    long index = pCurr->m_index;
    assert(index < m_count);
 
    CuePoint* const* const pp = m_cue_points;
    assert(pp);
    assert(pp[index] == pCurr);
 
    ++index;
 
    if (index >= m_count)
        return NULL;
 
    CuePoint* const pNext = pp[index];
    assert(pNext);
    assert(pNext->GetTimeCode() >= 0);
 #endif
 
    return pNext;
 }
