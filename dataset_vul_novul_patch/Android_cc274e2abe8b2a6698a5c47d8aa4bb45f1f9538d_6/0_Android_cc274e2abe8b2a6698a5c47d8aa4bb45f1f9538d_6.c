long Segment::DoLoadCluster(long long& pos, long& len) {
 if (m_pos < 0)
 return DoLoadClusterUnknownSize(pos, len);

 long long total, avail;

 long status = m_pReader->Length(&total, &avail);


   if (status < 0)  // error
     return status;
 
  if (total >= 0 && avail > total)
    return E_FILE_FORMAT_INVALID;
 
   const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;
 
 long long cluster_off = -1; // offset relative to start of segment
 long long cluster_size = -1; // size of cluster payload

 for (;;) {
 if ((total >= 0) && (m_pos >= total))
 return 1; // no more clusters

 if ((segment_stop >= 0) && (m_pos >= segment_stop))
 return 1; // no more clusters

    pos = m_pos;


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
    const long long id = ReadID(m_pReader, idpos, len);
 
    if (id < 0)
      return E_FILE_FORMAT_INVALID;
 
     pos += len;  // consume ID
 

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


 if (size == 0) { // weird
      m_pos = pos;
 continue;
 }

 
     const long long unknown_size = (1LL << (7 * len)) - 1;
 
     if ((segment_stop >= 0) && (size != unknown_size) &&
         ((pos + size) > segment_stop)) {
       return E_FILE_FORMAT_INVALID;
     }
 
     if (id == 0x0C53BB6B) {  // Cues ID
       if (size == unknown_size)
         return E_FILE_FORMAT_INVALID;  // TODO: liberalize


       if (m_pCues == NULL) {
         const long long element_size = (pos - idpos) + size;
 
        m_pCues = new (std::nothrow) Cues(this, pos, size, idpos, element_size);
        if (m_pCues == NULL)
          return -1;
       }
 
       m_pos = pos + size;  // consume payload
 continue;
 }

 if (id != 0x0F43B675) { // Cluster ID
 if (size == unknown_size)
 return E_FILE_FORMAT_INVALID; // TODO: liberalize

      m_pos = pos + size; // consume payload
 continue;
 }


    cluster_off = idpos - m_start; // relative pos

 if (size != unknown_size)
      cluster_size = size;


     break;
   }
 
  if (cluster_off < 0) {
    // No cluster, die.
    return E_FILE_FORMAT_INVALID;
  }
 
   long long pos_;
   long len_;

  status = Cluster::HasBlockEntries(this, cluster_off, pos_, len_);

 if (status < 0) { // error, or underflow
    pos = pos_;
    len = len_;

 return status;
 }




   const long idx = m_clusterCount;
 
   if (m_clusterPreloadCount > 0) {
    if (idx >= m_clusterSize)
      return E_FILE_FORMAT_INVALID;
 
     Cluster* const pCluster = m_clusters[idx];
    if (pCluster == NULL || pCluster->m_index >= 0)
      return E_FILE_FORMAT_INVALID;
 
     const long long off = pCluster->GetPosition();
    if (off < 0)
      return E_FILE_FORMAT_INVALID;
 
     if (off == cluster_off) {  // preloaded already
       if (status == 0)  // no entries found
 return E_FILE_FORMAT_INVALID;

 if (cluster_size >= 0)
        pos += cluster_size;
 else {
 const long long element_size = pCluster->GetElementSize();

 if (element_size <= 0)
 return E_FILE_FORMAT_INVALID; // TODO: handle this case

        pos = pCluster->m_element_start + element_size;
 }

      pCluster->m_index = idx; // move from preloaded to loaded
 ++m_clusterCount;

       --m_clusterPreloadCount;
 
       m_pos = pos;  // consume payload
      if (segment_stop >= 0 && m_pos > segment_stop)
        return E_FILE_FORMAT_INVALID;
 
       return 0;  // success
     }
   }
 
   if (status == 0) {  // no entries found
    if (cluster_size >= 0)
      pos += cluster_size;
 
     if ((total >= 0) && (pos >= total)) {
       m_pos = total;
 return 1; // no more clusters
 }

 if ((segment_stop >= 0) && (pos >= segment_stop)) {
      m_pos = segment_stop;
 return 1; // no more clusters
 }

    m_pos = pos;
 return 2; // try again
 }


 
   Cluster* const pCluster = Cluster::Create(this, idx, cluster_off);
  if (pCluster == NULL)
    return -1;
 
  if (!AppendCluster(pCluster)) {
    delete pCluster;
    return -1;
  }
 
   if (cluster_size >= 0) {
     pos += cluster_size;
 
     m_pos = pos;

    if (segment_stop > 0 && m_pos > segment_stop)
      return E_FILE_FORMAT_INVALID;
 
     return 0;
   }

  m_pUnknownSize = pCluster;
  m_pos = -pos;

 
   return 0;  // partial success, since we have a new cluster
 
  // status == 0 means "no block entries found"
  // pos designates start of payload
  // m_pos has NOT been adjusted yet (in case we need to come back here)
 }
