long VideoTrack::Parse(
    Segment* pSegment,
    const Info& info,
    long long element_start,
    long long element_size,
    VideoTrack*& pResult)
{
    if (pResult)
        return -1;
 
    if (info.type != Track::kVideo)
        return -1;
 
    long long width = 0;
    long long height = 0;
    double rate = 0.0;
 
    IMkvReader* const pReader = pSegment->m_pReader;
 
    const Settings& s = info.settings;
    assert(s.start >= 0);
    assert(s.size >= 0);
 
    long long pos = s.start;
    assert(pos >= 0);
 
    const long long stop = pos + s.size;
 
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
 
        if (id == 0x30)  //pixel width
        {
            width = UnserializeUInt(pReader, pos, size);
            if (width <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x3A)  //pixel height
        {
            height = UnserializeUInt(pReader, pos, size);
            if (height <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x0383E3)  //frame rate
        {
            const long status = UnserializeFloat(
                                    pReader,
                                    pos,
                                    size,
                                    rate);
            if (status < 0)
                return status;
            if (rate <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        pos += size;  //consume payload
        assert(pos <= stop);
    }
    assert(pos == stop);
    VideoTrack* const pTrack = new (std::nothrow) VideoTrack(pSegment,
                                                             element_start,
                                                             element_size);
    if (pTrack == NULL)
        return -1;  //generic error
    const int status = info.Copy(pTrack->m_info);
    if (status)  // error
    {
        delete pTrack;
        return status;
    }
    pTrack->m_width = width;
    pTrack->m_height = height;
    pTrack->m_rate = rate;
    pResult = pTrack;
    return 0;  //success
}
