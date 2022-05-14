long long Segment::CreateInstance(
    if (result < 0)  // error
      return result;
 
    if (result > 0)  // underflow (weird)
      return (pos + 1);
 
    if ((segment_stop >= 0) && ((pos + len) > segment_stop))
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > available)
      return pos + len;
 
    const long long idpos = pos;
    const long long id = ReadUInt(m_pReader, idpos, len);
 
    if (id < 0)  // error
      return id;

    if (id == 0x0F43B675)  // Cluster ID
      break;

    pos += len;  // consume ID

    if ((pos + 1) > available)
      return (pos + 1);

    // Read Size
    result = GetUIntLength(m_pReader, pos, len);

    if (result < 0)  // error
      return result;

    if (result > 0)  // underflow (weird)
      return (pos + 1);

    if ((segment_stop >= 0) && ((pos + len) > segment_stop))
      return E_FILE_FORMAT_INVALID;

    if ((pos + len) > available)
      return pos + len;

    const long long size = ReadUInt(m_pReader, pos, len);

    if (size < 0)  // error
      return size;

    pos += len;  // consume length of size of element

    const long long element_size = size + pos - element_start;

    // Pos now points to start of payload

    if ((segment_stop >= 0) && ((pos + size) > segment_stop))
      return E_FILE_FORMAT_INVALID;

    // We read EBML elements either in total or nothing at all.

    if ((pos + size) > available)
      return pos + size;

    if (id == 0x0549A966) {  // Segment Info ID
      if (m_pInfo)
        return E_FILE_FORMAT_INVALID;

      m_pInfo = new (std::nothrow)
          SegmentInfo(this, pos, size, element_start, element_size);

      if (m_pInfo == NULL)
         return -1;
 
      const long status = m_pInfo->Parse();

      if (status)
        return status;
    } else if (id == 0x0654AE6B) {  // Tracks ID
      if (m_pTracks)
        return E_FILE_FORMAT_INVALID;

      m_pTracks = new (std::nothrow)
          Tracks(this, pos, size, element_start, element_size);

      if (m_pTracks == NULL)
         return -1;
 
      const long status = m_pTracks->Parse();
 
