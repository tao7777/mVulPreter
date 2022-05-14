long Segment::DoParseNext(const Cluster*& pResult, long long& pos, long& len) {
 long long total, avail;

 long status = m_pReader->Length(&total, &avail);

 if (status < 0) // error
 return status;

  assert((total < 0) || (avail <= total));

 const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;


 long long off_next = 0;
 long long cluster_size = -1;

 for (;;) {
 if ((total >= 0) && (pos >= total))
 return 1; // EOF

 if ((segment_stop >= 0) && (pos >= segment_stop))
 return 1; // EOF

 if ((pos + 1) > avail) {
      len = 1;
 return E_BUFFER_NOT_FULL;
 }

 long long result = GetUIntLength(m_pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((segment_stop >= 0) && ((pos + len) > segment_stop))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long idpos = pos; // absolute
 const long long idoff = pos - m_start; // relative

 const long long id = ReadUInt(m_pReader, idpos, len); // absolute

 if (id < 0) // error
 return static_cast<long>(id);

 if (id == 0) // weird
 return -1; // generic error

    pos += len; // consume ID


 if ((pos + 1) > avail) {
      len = 1;
 return E_BUFFER_NOT_FULL;
 }

    result = GetUIntLength(m_pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((segment_stop >= 0) && ((pos + len) > segment_stop))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long size = ReadUInt(m_pReader, pos, len);

 if (size < 0) // error
 return static_cast<long>(size);

    pos += len; // consume length of size of element


 if (size == 0) // weird
 continue;

 const long long unknown_size = (1LL << (7 * len)) - 1;

 if ((segment_stop >= 0) && (size != unknown_size) &&
 ((pos + size) > segment_stop)) {
 return E_FILE_FORMAT_INVALID;
 }

 if (id == 0x0C53BB6B) { // Cues ID
 if (size == unknown_size)
 return E_FILE_FORMAT_INVALID;

 const long long element_stop = pos + size;

 if ((segment_stop >= 0) && (element_stop > segment_stop))
 return E_FILE_FORMAT_INVALID;

 const long long element_start = idpos;

       const long long element_size = element_stop - element_start;
 
       if (m_pCues == NULL) {
        m_pCues = new Cues(this, pos, size, element_start, element_size);
        assert(m_pCues);  // TODO
       }
 
       pos += size;  // consume payload
      assert((segment_stop < 0) || (pos <= segment_stop));
 
       continue;
     }

 if (id != 0x0F43B675) { // not a Cluster ID
 if (size == unknown_size)

         return E_FILE_FORMAT_INVALID;
 
       pos += size;  // consume payload
      assert((segment_stop < 0) || (pos <= segment_stop));
 
       continue;
     }
 
#if 0  // this is commented-out to support incremental cluster parsing
        len = static_cast<long>(size);
        if (element_stop > avail)
            return E_BUFFER_NOT_FULL;
#endif
     off_next = idoff;
 
     if (size != unknown_size)
      cluster_size = size;

 break;
 }

  assert(off_next > 0); // have cluster


 Cluster** const ii = m_clusters + m_clusterCount;
 Cluster** i = ii;

 Cluster** const jj = ii + m_clusterPreloadCount;
 Cluster** j = jj;

 while (i < j) {

 Cluster** const k = i + (j - i) / 2;
    assert(k < jj);

 const Cluster* const pNext = *k;
    assert(pNext);
    assert(pNext->m_index < 0);

    pos = pNext->GetPosition();
    assert(pos >= 0);

 if (pos < off_next)
      i = k + 1;
 else if (pos > off_next)
      j = k;
 else {
      pResult = pNext;
 return 0; // success
 }
 }

  assert(i == j);

 long long pos_;
 long len_;

  status = Cluster::HasBlockEntries(this, off_next, pos_, len_);

 if (status < 0) { // error or underflow
    pos = pos_;
    len = len_;

 return status;
 }

 if (status > 0) { // means "found at least one block entry"

     Cluster* const pNext = Cluster::Create(this,
                                            -1,  // preloaded
                                            off_next);
    assert(pNext);
 
     const ptrdiff_t idx_next = i - m_clusters;  // insertion position
 
    PreloadCluster(pNext, idx_next);
     assert(m_clusters);
     assert(idx_next < m_clusterSize);
     assert(m_clusters[idx_next] == pNext);

    pResult = pNext;
 return 0; // success
 }


 if (cluster_size < 0) { // unknown size
 const long long payload_pos = pos; // absolute pos of cluster payload

 for (;;) { // determine cluster size
 if ((total >= 0) && (pos >= total))
 break;

 if ((segment_stop >= 0) && (pos >= segment_stop))
 break; // no more clusters


 if ((pos + 1) > avail) {
        len = 1;
 return E_BUFFER_NOT_FULL;
 }

 long long result = GetUIntLength(m_pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((segment_stop >= 0) && ((pos + len) > segment_stop))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long idpos = pos;
 const long long id = ReadUInt(m_pReader, idpos, len);

 if (id < 0) // error (or underflow)
 return static_cast<long>(id);


 if (id == 0x0F43B675) // Cluster ID
 break;

 if (id == 0x0C53BB6B) // Cues ID
 break;

      pos += len; // consume ID (of sub-element)


 if ((pos + 1) > avail) {
        len = 1;
 return E_BUFFER_NOT_FULL;
 }

      result = GetUIntLength(m_pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((segment_stop >= 0) && ((pos + len) > segment_stop))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long size = ReadUInt(m_pReader, pos, len);

 if (size < 0) // error
 return static_cast<long>(size);

      pos += len; // consume size field of element


 if (size == 0) // weird
 continue;

 const long long unknown_size = (1LL << (7 * len)) - 1;

 if (size == unknown_size)
 return E_FILE_FORMAT_INVALID; // not allowed for sub-elements

 if ((segment_stop >= 0) && ((pos + size) > segment_stop)) // weird

         return E_FILE_FORMAT_INVALID;
 
       pos += size;  // consume payload of sub-element
      assert((segment_stop < 0) || (pos <= segment_stop));
     }  // determine cluster size
 
     cluster_size = pos - payload_pos;
    assert(cluster_size >= 0); // TODO: handle cluster_size = 0

    pos = payload_pos; // reset and re-parse original cluster

   }
 
   pos += cluster_size;  // consume payload
  assert((segment_stop < 0) || (pos <= segment_stop));
 
   return 2;  // try to find a cluster that follows next
 }
