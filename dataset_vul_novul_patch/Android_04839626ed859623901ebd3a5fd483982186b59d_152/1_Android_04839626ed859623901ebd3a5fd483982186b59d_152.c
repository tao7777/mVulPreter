void Cues::Init() const
{
    if (m_cue_points)
        return;
 
    assert(m_count == 0);
    assert(m_preload_count == 0);
 
    IMkvReader* const pReader = m_pSegment->m_pReader;
 
    const long long stop = m_start + m_size;
    long long pos = m_start;
 
    long cue_points_size = 0;
 
    while (pos < stop)
    {
        const long long idpos = pos;
 
        long len;
        const long long id = ReadUInt(pReader, pos, len);
        assert(id >= 0);  //TODO
        assert((pos + len) <= stop);
        pos += len;  //consume ID
        const long long size = ReadUInt(pReader, pos, len);
        assert(size >= 0);
        assert((pos + len) <= stop);
        pos += len;  //consume Size field
        assert((pos + size) <= stop);
        if (id == 0x3B)  //CuePoint ID
            PreloadCuePoint(cue_points_size, idpos);
        pos += size;  //consume payload
        assert(pos <= stop);
    }
}
