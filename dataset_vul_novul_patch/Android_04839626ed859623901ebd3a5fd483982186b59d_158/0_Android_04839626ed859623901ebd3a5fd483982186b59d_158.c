long Segment::LoadCluster(
    if (result <= 1)
      return result;
  }
}
 
long Segment::DoLoadCluster(long long& pos, long& len) {
  if (m_pos < 0)
    return DoLoadClusterUnknownSize(pos, len);

  long long total, avail;

  long status = m_pReader->Length(&total, &avail);

  if (status < 0)  // error
    return status;

  assert((total < 0) || (avail <= total));

  const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;

  long long cluster_off = -1;  // offset relative to start of segment
  long long cluster_size = -1;  // size of cluster payload

  for (;;) {
    if ((total >= 0) && (m_pos >= total))
      return 1;  // no more clusters

    if ((segment_stop >= 0) && (m_pos >= segment_stop))
      return 1;  // no more clusters

    pos = m_pos;

    // Read ID

    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
     }
