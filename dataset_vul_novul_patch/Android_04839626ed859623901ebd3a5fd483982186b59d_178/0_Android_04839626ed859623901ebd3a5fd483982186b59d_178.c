long Cluster::ParseBlockGroup(
  IMkvReader* const pReader = m_pSegment->m_pReader;
 
  long long total, avail;
 
  long status = pReader->Length(&total, &avail);
 
  if (status < 0)  // error
    return status;
 
  assert((total < 0) || (avail <= total));
 
  if ((total >= 0) && (payload_stop > total))
    return E_FILE_FORMAT_INVALID;
 
  if (payload_stop > avail) {
    len = static_cast<long>(payload_size);
    return E_BUFFER_NOT_FULL;
  }
 
  long long discard_padding = 0;

  while (pos < payload_stop) {
    // parse sub-block element ID

    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
     }
 
    long long result = GetUIntLength(pReader, pos, len);
 
    if (result < 0)  // error
      return static_cast<long>(result);
 
    if (result > 0)  // weird
      return E_BUFFER_NOT_FULL;
 
    if ((pos + len) > payload_stop)
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;
 
    const long long id = ReadUInt(pReader, pos, len);
 
    if (id < 0)  // error
      return static_cast<long>(id);
 
    if (id == 0)  // not a value ID
      return E_FILE_FORMAT_INVALID;
 
    pos += len;  // consume ID field
 
    // Parse Size
 
    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
    }
 
    result = GetUIntLength(pReader, pos, len);
 
    if (result < 0)  // error
      return static_cast<long>(result);
 
    if (result > 0)  // weird
      return E_BUFFER_NOT_FULL;
 
    if ((pos + len) > payload_stop)
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;
 
    const long long size = ReadUInt(pReader, pos, len);
 
    if (size < 0)  // error
      return static_cast<long>(size);
 
    pos += len;  // consume size field
 
    // pos now points to start of sub-block group payload
 
    if (pos > payload_stop)
      return E_FILE_FORMAT_INVALID;
 
    if (size == 0)  // weird
      continue;
 
    const long long unknown_size = (1LL << (7 * len)) - 1;
 
    if (size == unknown_size)
      return E_FILE_FORMAT_INVALID;
 
    if (id == 0x35A2) {  // DiscardPadding
      status = UnserializeInt(pReader, pos, size, discard_padding);
 
      if (status < 0)  // error
        return status;
    }
 
    if (id != 0x21) {  // sub-part of BlockGroup is not a Block
      pos += size;  // consume sub-part of block group
 
      if (pos > payload_stop)
        return E_FILE_FORMAT_INVALID;
 
      continue;
    }
 
    const long long block_stop = pos + size;
 
    if (block_stop > payload_stop)
      return E_FILE_FORMAT_INVALID;
 
    // parse track number
 
    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
    }
 
    result = GetUIntLength(pReader, pos, len);
 
    if (result < 0)  // error
      return static_cast<long>(result);
 
    if (result > 0)  // weird
      return E_BUFFER_NOT_FULL;
 
    if ((pos + len) > block_stop)
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;
 
    const long long track = ReadUInt(pReader, pos, len);
 
    if (track < 0)  // error
      return static_cast<long>(track);
 
    if (track == 0)
      return E_FILE_FORMAT_INVALID;
 
 #if 0

 const Tracks* const pTracks = m_pSegment->GetTracks();
        assert(pTracks);

 const long tn = static_cast<long>(track);

 const Track* const pTrack = pTracks->GetTrackByNumber(tn);

 if (pTrack == NULL)

             return E_FILE_FORMAT_INVALID;
 #endif
 
    pos += len;  // consume track number
 
    if ((pos + 2) > block_stop)
      return E_FILE_FORMAT_INVALID;
 
    if ((pos + 2) > avail) {
      len = 2;
      return E_BUFFER_NOT_FULL;
     }
 
    pos += 2;  // consume timecode
 
    if ((pos + 1) > block_stop)
      return E_FILE_FORMAT_INVALID;
 
