long Segment::ParseNext(const Cluster* pCurr, const Cluster*& pResult,
 long long& pos, long& len) {
  assert(pCurr);
  assert(!pCurr->EOS());
  assert(m_clusters);

  pResult = 0;

 if (pCurr->m_index >= 0) { // loaded (not merely preloaded)
    assert(m_clusters[pCurr->m_index] == pCurr);

 const long next_idx = pCurr->m_index + 1;

 if (next_idx < m_clusterCount) {
      pResult = m_clusters[next_idx];
 return 0; // success
 }


 const long result = LoadCluster(pos, len);

 if (result < 0) // error or underflow
 return result;

 if (result > 0) // no more clusters
 {
 return 1;
 }

    pResult = GetLast();
 return 0; // success
 }

  assert(m_pos > 0);

 long long total, avail;

 long status = m_pReader->Length(&total, &avail);

 if (status < 0) // error
 return status;

  assert((total < 0) || (avail <= total));

 const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;


  pos = pCurr->m_element_start;

 if (pCurr->m_element_size >= 0)
    pos += pCurr->m_element_size;
 else {
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

 const long long id = ReadUInt(m_pReader, pos, len);

 if (id != 0x0F43B675) // weird: not Cluster ID
 return -1;

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

    pos += len; // consume size field

 const long long unknown_size = (1LL << (7 * len)) - 1;

 if (size == unknown_size) // TODO: should never happen
 return E_FILE_FORMAT_INVALID; // TODO: resolve this


 if ((segment_stop >= 0) && ((pos + size) > segment_stop))
 return E_FILE_FORMAT_INVALID;


 
     pos += size;  // consume payload (that is, the current cluster)
    assert((segment_stop < 0) || (pos <= segment_stop));
 
 }


 for (;;) {
 const long status = DoParseNext(pResult, pos, len);

 if (status <= 1)
 return status;
 }
}
