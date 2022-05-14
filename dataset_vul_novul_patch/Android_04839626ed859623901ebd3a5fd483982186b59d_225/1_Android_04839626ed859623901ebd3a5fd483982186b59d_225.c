Cues::~Cues()
{
    const long n = m_count + m_preload_count;
 
    CuePoint** p = m_cue_points;
    CuePoint** const q = p + n;
 
    while (p != q)
    {
        CuePoint* const pCP = *p++;
        assert(pCP);
 
        delete pCP;
    }
 
     delete[] m_cue_points;
 }
