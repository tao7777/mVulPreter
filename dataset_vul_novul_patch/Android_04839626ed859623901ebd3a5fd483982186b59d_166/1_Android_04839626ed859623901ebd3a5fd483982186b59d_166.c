long SegmentInfo::Parse()
{
    assert(m_pMuxingAppAsUTF8 == NULL);
    assert(m_pWritingAppAsUTF8 == NULL);
    assert(m_pTitleAsUTF8 == NULL);
    IMkvReader* const pReader = m_pSegment->m_pReader;
    long long pos = m_start;
    const long long stop = m_start + m_size;
    m_timecodeScale = 1000000;
    m_duration = -1;
    while (pos < stop)
    {
        long long id, size;
        const long status = ParseElementHeader(
                                pReader,
                                pos,
                                stop,
                                id,
                                size);
        if (status < 0)  //error
            return status;
        if (id == 0x0AD7B1)  //Timecode Scale
        {
            m_timecodeScale = UnserializeUInt(pReader, pos, size);
            if (m_timecodeScale <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x0489)  //Segment duration
        {
            const long status = UnserializeFloat(
                                    pReader,
                                    pos,
                                    size,
                                    m_duration);
            if (status < 0)
                return status;
            if (m_duration < 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x0D80)  //MuxingApp
        {
            const long status = UnserializeString(
                                    pReader,
                                    pos,
                                    size,
                                    m_pMuxingAppAsUTF8);
            if (status)
                return status;
        }
        else if (id == 0x1741)  //WritingApp
        {
            const long status = UnserializeString(
                                    pReader,
                                    pos,
                                    size,
                                    m_pWritingAppAsUTF8);
            if (status)
                return status;
        }
        else if (id == 0x3BA9)  //Title
        {
            const long status = UnserializeString(
                                    pReader,
                                    pos,
                                    size,
                                    m_pTitleAsUTF8);
            if (status)
                return status;
        }
        pos += size;
        assert(pos <= stop);
    }
    assert(pos == stop);
    return 0;
}
