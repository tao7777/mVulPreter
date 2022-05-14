long Cluster::Load(long long& pos, long& len) const
{
    assert(m_pSegment);
    assert(m_pos >= m_element_start);
 
    if (m_timecode >= 0)  //at least partially loaded
        return 0;
 
    assert(m_pos == m_element_start);
    assert(m_element_size < 0);
 
    IMkvReader* const pReader = m_pSegment->m_pReader;
 
    long long total, avail;
 
    const int status = pReader->Length(&total, &avail);
 
    if (status < 0)  //error
        return status;
 
    assert((total < 0) || (avail <= total));
    assert((total < 0) || (m_pos <= total));  //TODO: verify this
 
    pos = m_pos;
 
    long long cluster_size = -1;
    {
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        long long result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error or underflow
            return static_cast<long>(result);
        if (result > 0)  //underflow (weird)
            return E_BUFFER_NOT_FULL;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long id_ = ReadUInt(pReader, pos, len);
        if (id_ < 0)  //error
            return static_cast<long>(id_);
        if (id_ != 0x0F43B675)  //Cluster ID
            return E_FILE_FORMAT_INVALID;
        pos += len;  //consume id
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error
            return static_cast<long>(result);
        if (result > 0)  //weird
            return E_BUFFER_NOT_FULL;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long size = ReadUInt(pReader, pos, len);
        if (size < 0)  //error
            return static_cast<long>(cluster_size);
        if (size == 0)
            return E_FILE_FORMAT_INVALID;  //TODO: verify this
        pos += len;  //consume length of size of element
        const long long unknown_size = (1LL << (7 * len)) - 1;
        if (size != unknown_size)
            cluster_size = size;
     }
 
//// pos points to start of payload
 
 #if 0
     len = static_cast<long>(size_);

 if (cluster_stop > avail)

         return E_BUFFER_NOT_FULL;
 #endif
 
    long long timecode = -1;
    long long new_pos = -1;
    bool bBlock = false;
 
    long long cluster_stop = (cluster_size < 0) ? -1 : pos + cluster_size;
 
    for (;;)
    {
        if ((cluster_stop >= 0) && (pos >= cluster_stop))
            break;
 
 
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        long long result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error
            return static_cast<long>(result);
        if (result > 0)  //weird
            return E_BUFFER_NOT_FULL;
        if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long id = ReadUInt(pReader, pos, len);
        if (id < 0) //error
            return static_cast<long>(id);
        if (id == 0)
            return E_FILE_FORMAT_INVALID;
        if (id == 0x0F43B675)  //Cluster ID
            break;
        if (id == 0x0C53BB6B)  //Cues ID
            break;
        pos += len;  //consume ID field
        if ((pos + 1) > avail)
        {
            len = 1;
            return E_BUFFER_NOT_FULL;
        }
        result = GetUIntLength(pReader, pos, len);
        if (result < 0)  //error
            return static_cast<long>(result);
        if (result > 0)  //weird
            return E_BUFFER_NOT_FULL;
        if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if ((pos + len) > avail)
            return E_BUFFER_NOT_FULL;
        const long long size = ReadUInt(pReader, pos, len);
        if (size < 0)  //error
            return static_cast<long>(size);
        const long long unknown_size = (1LL << (7 * len)) - 1;
        if (size == unknown_size)
            return E_FILE_FORMAT_INVALID;
        pos += len;  //consume size field
        if ((cluster_stop >= 0) && (pos > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if (size == 0)  //weird
            continue;
        if ((cluster_stop >= 0) && ((pos + size) > cluster_stop))
            return E_FILE_FORMAT_INVALID;
        if (id == 0x67)  //TimeCode ID
        {
            len = static_cast<long>(size);
            if ((pos + size) > avail)
                return E_BUFFER_NOT_FULL;
            timecode = UnserializeUInt(pReader, pos, size);
            if (timecode < 0)  //error (or underflow)
                return static_cast<long>(timecode);
            new_pos = pos + size;
            if (bBlock)
                break;
        }
        else if (id == 0x20)  //BlockGroup ID
        {
            bBlock = true;
            break;
        }
        else if (id == 0x23)  //SimpleBlock ID
        {
            bBlock = true;
            break;
        }
        pos += size;  //consume payload
        assert((cluster_stop < 0) || (pos <= cluster_stop));
    }
    assert((cluster_stop < 0) || (pos <= cluster_stop));
    if (timecode < 0)  //no timecode found
        return E_FILE_FORMAT_INVALID;
    if (!bBlock)
        return E_FILE_FORMAT_INVALID;
    m_pos = new_pos;  //designates position just beyond timecode payload
    m_timecode = timecode;  // m_timecode >= 0 means we're partially loaded
    if (cluster_size >= 0)
        m_element_size = cluster_stop - m_element_start;
    return 0;
}
