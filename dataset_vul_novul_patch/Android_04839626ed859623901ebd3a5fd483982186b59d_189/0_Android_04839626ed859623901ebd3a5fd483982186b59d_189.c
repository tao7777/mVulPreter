long long Segment::ParseHeaders()
      if (status)
         return status;
    } else if (id == 0x0C53BB6B) {  // Cues ID
      if (m_pCues == NULL) {
        m_pCues = new (std::nothrow)
            Cues(this, pos, size, element_start, element_size);
 
        if (m_pCues == NULL)
          return -1;
      }
    } else if (id == 0x014D9B74) {  // SeekHead ID
      if (m_pSeekHead == NULL) {
        m_pSeekHead = new (std::nothrow)
            SeekHead(this, pos, size, element_start, element_size);
 
        if (m_pSeekHead == NULL)
          return -1;
 
        const long status = m_pSeekHead->Parse();
 
        if (status)
          return status;
      }
    } else if (id == 0x0043A770) {  // Chapters ID
      if (m_pChapters == NULL) {
        m_pChapters = new (std::nothrow)
            Chapters(this, pos, size, element_start, element_size);
 
        if (m_pChapters == NULL)
          return -1;
 
        const long status = m_pChapters->Parse();
 
        if (status)
          return status;
      }
     }
 
    m_pos = pos + size;  // consume payload
  }
 
  assert((segment_stop < 0) || (m_pos <= segment_stop));
 
  if (m_pInfo == NULL)  // TODO: liberalize this behavior
    return E_FILE_FORMAT_INVALID;
 
  if (m_pTracks == NULL)
    return E_FILE_FORMAT_INVALID;

  return 0;  // success
 }
