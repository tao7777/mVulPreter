long Cluster::ParseSimpleBlock(
    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
     }
 
     long long result = GetUIntLength(pReader, pos, len);
 
    if (result < 0)  // error
      return static_cast<long>(result);
 
    if (result > 0)  // weird
      return E_BUFFER_NOT_FULL;
 
    if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
      return E_FILE_FORMAT_INVALID;
 
     if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;

    const long long id = ReadUInt(pReader, pos, len);

    if (id < 0)  // error
      return static_cast<long>(id);

    if (id == 0)
      return E_FILE_FORMAT_INVALID;

    // This is the distinguished set of ID's we use to determine
    // that we have exhausted the sub-element's inside the cluster
    // whose ID we parsed earlier.

    if (id == 0x0F43B675)  // Cluster ID
      break;

    if (id == 0x0C53BB6B)  // Cues ID
      break;

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

    if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;

    const long long size = ReadUInt(pReader, pos, len);

    if (size < 0)  // error
      return static_cast<long>(size);

    const long long unknown_size = (1LL << (7 * len)) - 1;

    if (size == unknown_size)
      return E_FILE_FORMAT_INVALID;

    pos += len;  // consume size field

    if ((cluster_stop >= 0) && (pos > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    // pos now points to start of payload

    if (size == 0)  // weird
      continue;

    if ((cluster_stop >= 0) && ((pos + size) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if (id == 0x67) {  // TimeCode ID
      len = static_cast<long>(size);

      if ((pos + size) > avail)
         return E_BUFFER_NOT_FULL;
 
      timecode = UnserializeUInt(pReader, pos, size);
 
      if (timecode < 0)  // error (or underflow)
        return static_cast<long>(timecode);
 
      new_pos = pos + size;

      if (bBlock)
        break;
    } else if (id == 0x20) {  // BlockGroup ID
      bBlock = true;
      break;
    } else if (id == 0x23) {  // SimpleBlock ID
      bBlock = true;
      break;
    }

    pos += size;  // consume payload
    assert((cluster_stop < 0) || (pos <= cluster_stop));
  }

  assert((cluster_stop < 0) || (pos <= cluster_stop));

  if (timecode < 0)  // no timecode found
    return E_FILE_FORMAT_INVALID;

  if (!bBlock)
    return E_FILE_FORMAT_INVALID;

  m_pos = new_pos;  // designates position just beyond timecode payload
  m_timecode = timecode;  // m_timecode >= 0 means we're partially loaded

  if (cluster_size >= 0)
    m_element_size = cluster_stop - m_element_start;

  return 0;
}

long Cluster::Parse(long long& pos, long& len) const {
  long status = Load(pos, len);

  if (status < 0)
    return status;

  assert(m_pos >= m_element_start);
  assert(m_timecode >= 0);
  // assert(m_size > 0);
  // assert(m_element_size > m_size);

  const long long cluster_stop =
      (m_element_size < 0) ? -1 : m_element_start + m_element_size;

  if ((cluster_stop >= 0) && (m_pos >= cluster_stop))
    return 1;  // nothing else to do

  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long total, avail;

  status = pReader->Length(&total, &avail);

  if (status < 0)  // error
    return status;

  assert((total < 0) || (avail <= total));

  pos = m_pos;

  for (;;) {
    if ((cluster_stop >= 0) && (pos >= cluster_stop))
      break;

    if ((total >= 0) && (pos >= total)) {
      if (m_element_size < 0)
        m_element_size = pos - m_element_start;

      break;
    }

    // Parse ID

    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
    }

    long long result = GetUIntLength(pReader, pos, len);

    if (result < 0)  // error
      return static_cast<long>(result);

    if (result > 0)  // weird
      return E_BUFFER_NOT_FULL;

    if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;

    const long long id = ReadUInt(pReader, pos, len);

    if (id < 0)  // error
      return static_cast<long>(id);

    if (id == 0)  // weird
      return E_FILE_FORMAT_INVALID;

    // This is the distinguished set of ID's we use to determine
    // that we have exhausted the sub-element's inside the cluster
    // whose ID we parsed earlier.

    if ((id == 0x0F43B675) || (id == 0x0C53BB6B)) {  // Cluster or Cues ID
      if (m_element_size < 0)
        m_element_size = pos - m_element_start;

      break;
    }

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

    if ((cluster_stop >= 0) && ((pos + len) > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    if ((pos + len) > avail)
      return E_BUFFER_NOT_FULL;

    const long long size = ReadUInt(pReader, pos, len);

    if (size < 0)  // error
      return static_cast<long>(size);

    const long long unknown_size = (1LL << (7 * len)) - 1;

    if (size == unknown_size)
      return E_FILE_FORMAT_INVALID;

    pos += len;  // consume size field

    if ((cluster_stop >= 0) && (pos > cluster_stop))
      return E_FILE_FORMAT_INVALID;

    // pos now points to start of payload

    if (size == 0)  // weird
      continue;

    // const long long block_start = pos;
    const long long block_stop = pos + size;

    if (cluster_stop >= 0) {
      if (block_stop > cluster_stop) {
        if ((id == 0x20) || (id == 0x23))
          return E_FILE_FORMAT_INVALID;

        pos = cluster_stop;
        break;
      }
    } else if ((total >= 0) && (block_stop > total)) {
      m_element_size = total - m_element_start;
      pos = total;
      break;
    } else if (block_stop > avail) {
      len = static_cast<long>(size);
      return E_BUFFER_NOT_FULL;
    }

    Cluster* const this_ = const_cast<Cluster*>(this);

    if (id == 0x20)  // BlockGroup
      return this_->ParseBlockGroup(size, pos, len);

    if (id == 0x23)  // SimpleBlock
      return this_->ParseSimpleBlock(size, pos, len);

    pos += size;  // consume payload
    assert((cluster_stop < 0) || (pos <= cluster_stop));
  }

  assert(m_element_size > 0);

  m_pos = pos;
  assert((cluster_stop < 0) || (m_pos <= cluster_stop));

  if (m_entries_count > 0) {
    const long idx = m_entries_count - 1;

    const BlockEntry* const pLast = m_entries[idx];
    assert(pLast);

    const Block* const pBlock = pLast->GetBlock();
    assert(pBlock);

    const long long start = pBlock->m_start;

    if ((total >= 0) && (start > total))
      return -1;  // defend against trucated stream

    const long long size = pBlock->m_size;

    const long long stop = start + size;
    assert((cluster_stop < 0) || (stop <= cluster_stop));

    if ((total >= 0) && (stop > total))
      return -1;  // defend against trucated stream
  }

  return 1;  // no more entries
}

long Cluster::ParseSimpleBlock(long long block_size, long long& pos,
                               long& len) {
  const long long block_start = pos;
  const long long block_stop = pos + block_size;

  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long total, avail;

  long status = pReader->Length(&total, &avail);

  if (status < 0)  // error
    return status;

  assert((total < 0) || (avail <= total));

  // parse track number

  if ((pos + 1) > avail) {
    len = 1;
    return E_BUFFER_NOT_FULL;
  }

  long long result = GetUIntLength(pReader, pos, len);

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
 
  if ((pos + 1) > avail) {
    len = 1;
    return E_BUFFER_NOT_FULL;
  }
 
  unsigned char flags;
 
  status = pReader->Read(pos, 1, &flags);
 
  if (status < 0) {  // error or underflow
    len = 1;
    return status;
  }
 
  ++pos;  // consume flags byte
  assert(pos <= avail);
 
  if (pos >= block_stop)
    return E_FILE_FORMAT_INVALID;
 
  const int lacing = int(flags & 0x06) >> 1;
 
  if ((lacing != 0) && (block_stop > avail)) {
    len = static_cast<long>(block_stop - pos);
    return E_BUFFER_NOT_FULL;
  }
 
  status = CreateBlock(0x23,  // simple block id
                       block_start, block_size,
                       0);  // DiscardPadding
 
  if (status != 0)
    return status;
 
  m_pos = block_stop;
 
  return 0;  // success
 }
