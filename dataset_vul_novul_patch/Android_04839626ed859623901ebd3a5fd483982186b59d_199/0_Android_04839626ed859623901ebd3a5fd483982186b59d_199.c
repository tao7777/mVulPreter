SeekHead::SeekHead(
  // Outermost (level 0) segment object has been constructed,
  // and pos designates start of payload.  We need to find the
  // inner (level 1) elements.

  const long long header_status = ParseHeaders();

  if (header_status < 0)  // error
    return static_cast<long>(header_status);

  if (header_status > 0)  // underflow
    return E_BUFFER_NOT_FULL;

  assert(m_pInfo);
  assert(m_pTracks);

  for (;;) {
    const int status = LoadCluster();

    if (status < 0)  // error
      return status;

    if (status >= 1)  // no more clusters
      return 0;
  }
 }
