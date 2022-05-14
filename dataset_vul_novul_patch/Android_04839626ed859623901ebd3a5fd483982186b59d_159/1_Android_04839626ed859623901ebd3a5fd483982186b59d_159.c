bool Cues::LoadCuePoint() const
{
    const long long stop = m_start + m_size;
    if (m_pos >= stop)
        return false;  //nothing else to do
    Init();
    IMkvReader* const pReader = m_pSegment->m_pReader;
    while (m_pos < stop)
    {
        const long long idpos = m_pos;
        long len;
        const long long id = ReadUInt(pReader, m_pos, len);
        assert(id >= 0);  //TODO
        assert((m_pos + len) <= stop);
        m_pos += len;  //consume ID
        const long long size = ReadUInt(pReader, m_pos, len);
        assert(size >= 0);
        assert((m_pos + len) <= stop);
        m_pos += len;  //consume Size field
        assert((m_pos + size) <= stop);
        if (id != 0x3B)  //CuePoint ID
        {
            m_pos += size;  //consume payload
            assert(m_pos <= stop);
            continue;
        }
        assert(m_preload_count > 0);
        CuePoint* const pCP = m_cue_points[m_count];
        assert(pCP);
        assert((pCP->GetTimeCode() >= 0) || (-pCP->GetTimeCode() == idpos));
        if (pCP->GetTimeCode() < 0 && (-pCP->GetTimeCode() != idpos))
            return false;
        pCP->Load(pReader);
        ++m_count;
        --m_preload_count;
        m_pos += size;  //consume payload
        assert(m_pos <= stop);
        return true;  //yes, we loaded a cue point
    }
    return false;  //no, we did not load a cue point
}
