long Block::Parse(const Cluster* pCluster)
{
    if (pCluster == NULL)
        return -1;
 
    if (pCluster->m_pSegment == NULL)
        return -1;
 
    assert(m_start >= 0);
    assert(m_size >= 0);
    assert(m_track <= 0);
    assert(m_frames == NULL);
    assert(m_frame_count <= 0);
 
    long long pos = m_start;
    const long long stop = m_start + m_size;
 
    long len;
 
    IMkvReader* const pReader = pCluster->m_pSegment->m_pReader;
 
    m_track = ReadUInt(pReader, pos, len);
 
    if (m_track <= 0)
        return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > stop)
        return E_FILE_FORMAT_INVALID;
 
    pos += len;  //consume track number
 
    if ((stop - pos) < 2)
        return E_FILE_FORMAT_INVALID;
 
    long status;
    long long value;
 
    status = UnserializeInt(pReader, pos, 2, value);
 
    if (status)
        return E_FILE_FORMAT_INVALID;
 
    if (value < SHRT_MIN)
        return E_FILE_FORMAT_INVALID;
 
    if (value > SHRT_MAX)
        return E_FILE_FORMAT_INVALID;
 
    m_timecode = static_cast<short>(value);
 
    pos += 2;
 
    if ((stop - pos) <= 0)
        return E_FILE_FORMAT_INVALID;
    status = pReader->Read(pos, 1, &m_flags);
    if (status)
        return E_FILE_FORMAT_INVALID;
    const int lacing = int(m_flags & 0x06) >> 1;
    ++pos;  //consume flags byte
    if (lacing == 0)  //no lacing
    {
        if (pos > stop)
            return E_FILE_FORMAT_INVALID;
        m_frame_count = 1;
        m_frames = new Frame[m_frame_count];
        Frame& f = m_frames[0];
        f.pos = pos;
        const long long frame_size = stop - pos;
        if (frame_size > LONG_MAX)
            return E_FILE_FORMAT_INVALID;
        f.len = static_cast<long>(frame_size);
        return 0;  //success
    }
    if (pos >= stop)
        return E_FILE_FORMAT_INVALID;
    unsigned char biased_count;
    status = pReader->Read(pos, 1, &biased_count);
    if (status)
        return E_FILE_FORMAT_INVALID;
    ++pos;  //consume frame count
    assert(pos <= stop);
    m_frame_count = int(biased_count) + 1;
     m_frames = new Frame[m_frame_count];
    assert(m_frames);
 
    if (lacing == 1)  //Xiph
    {
        Frame* pf = m_frames;
        Frame* const pf_end = pf + m_frame_count;
 
        long size = 0;
        int frame_count = m_frame_count;
 
        while (frame_count > 1)
        {
            long frame_size = 0;
 
            for (;;)
            {
                unsigned char val;
 
                if (pos >= stop)
                    return E_FILE_FORMAT_INVALID;
 
                status = pReader->Read(pos, 1, &val);
 
                if (status)
                    return E_FILE_FORMAT_INVALID;
 
                ++pos;  //consume xiph size byte
 
                frame_size += val;
 
                if (val < 255)
                    break;
            }
 
            Frame& f = *pf++;
            assert(pf < pf_end);
 
            f.pos = 0;  //patch later
 
            f.len = frame_size;
            size += frame_size;  //contribution of this frame
 
            --frame_count;
        }
 
        assert(pf < pf_end);
        assert(pos <= stop);
 
        {
            Frame& f = *pf++;
            if (pf != pf_end)
                return E_FILE_FORMAT_INVALID;
            f.pos = 0;  //patch later
            const long long total_size = stop - pos;
            if (total_size < size)
                return E_FILE_FORMAT_INVALID;
            const long long frame_size = total_size - size;
            if (frame_size > LONG_MAX)
                return E_FILE_FORMAT_INVALID;
            f.len = static_cast<long>(frame_size);
        }
        pf = m_frames;
        while (pf != pf_end)
        {
            Frame& f = *pf++;
            assert((pos + f.len) <= stop);
            f.pos = pos;
            pos += f.len;
        }
        assert(pos == stop);
    }
    else if (lacing == 2)  //fixed-size lacing
    {
        const long long total_size = stop - pos;
        if ((total_size % m_frame_count) != 0)
            return E_FILE_FORMAT_INVALID;
        const long long frame_size = total_size / m_frame_count;
        if (frame_size > LONG_MAX)
            return E_FILE_FORMAT_INVALID;
        Frame* pf = m_frames;
        Frame* const pf_end = pf + m_frame_count;
        while (pf != pf_end)
        {
            assert((pos + frame_size) <= stop);
            Frame& f = *pf++;
            f.pos = pos;
            f.len = static_cast<long>(frame_size);
            pos += frame_size;
        }
        assert(pos == stop);
    }
    else
    {
        assert(lacing == 3);  //EBML lacing
 
         if (pos >= stop)
            return E_FILE_FORMAT_INVALID;
 
        long size = 0;
        int frame_count = m_frame_count;
 
        long long frame_size = ReadUInt(pReader, pos, len);
 
        if (frame_size < 0)
            return E_FILE_FORMAT_INVALID;
 
        if (frame_size > LONG_MAX)
            return E_FILE_FORMAT_INVALID;
 
        if ((pos + len) > stop)
            return E_FILE_FORMAT_INVALID;
 
        pos += len; //consume length of size of first frame
 
        if ((pos + frame_size) > stop)
            return E_FILE_FORMAT_INVALID;
 
        Frame* pf = m_frames;
        Frame* const pf_end = pf + m_frame_count;
 
        {
            Frame& curr = *pf;
            curr.pos = 0;  //patch later
            curr.len = static_cast<long>(frame_size);
            size += curr.len;  //contribution of this frame
        }
        --frame_count;
        while (frame_count > 1)
        {
            if (pos >= stop)
                return E_FILE_FORMAT_INVALID;
            assert(pf < pf_end);
            const Frame& prev = *pf++;
            assert(prev.len == frame_size);
            if (prev.len != frame_size)
                return E_FILE_FORMAT_INVALID;
            assert(pf < pf_end);
            Frame& curr = *pf;
            curr.pos = 0;  //patch later
            const long long delta_size_ = ReadUInt(pReader, pos, len);
            if (delta_size_ < 0)
                return E_FILE_FORMAT_INVALID;
            if ((pos + len) > stop)
                return E_FILE_FORMAT_INVALID;
            pos += len;  //consume length of (delta) size
            assert(pos <= stop);
            const int exp = 7*len - 1;
            const long long bias = (1LL << exp) - 1LL;
            const long long delta_size = delta_size_ - bias;
            frame_size += delta_size;
            if (frame_size < 0)
                return E_FILE_FORMAT_INVALID;
            if (frame_size > LONG_MAX)
                return E_FILE_FORMAT_INVALID;
            curr.len = static_cast<long>(frame_size);
            size += curr.len;  //contribution of this frame
            --frame_count;
        }
        {
            assert(pos <= stop);
            assert(pf < pf_end);
            const Frame& prev = *pf++;
            assert(prev.len == frame_size);
            if (prev.len != frame_size)
                return E_FILE_FORMAT_INVALID;
            assert(pf < pf_end);
            Frame& curr = *pf++;
            assert(pf == pf_end);
            curr.pos = 0;  //patch later
            const long long total_size = stop - pos;
            if (total_size < size)
                return E_FILE_FORMAT_INVALID;
            frame_size = total_size - size;
            if (frame_size > LONG_MAX)
                return E_FILE_FORMAT_INVALID;
            curr.len = static_cast<long>(frame_size);
        }
        pf = m_frames;
        while (pf != pf_end)
        {
            Frame& f = *pf++;
            assert((pos + f.len) <= stop);
            f.pos = pos;
            pos += f.len;
        }
        assert(pos == stop);
     }
 
    return 0;  //success
 }
