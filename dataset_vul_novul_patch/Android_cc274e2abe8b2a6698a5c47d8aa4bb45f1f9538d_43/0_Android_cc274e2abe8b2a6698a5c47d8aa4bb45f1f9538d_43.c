long Tracks::Parse() {
  assert(m_trackEntries == NULL);
  assert(m_trackEntriesEnd == NULL);

 const long long stop = m_start + m_size;
 IMkvReader* const pReader = m_pSegment->m_pReader;

 int count = 0;
 long long pos = m_start;

 while (pos < stop) {
 long long id, size;

 const long status = ParseElementHeader(pReader, pos, stop, id, size);

 if (status < 0) // error
 return status;

 if (size == 0) // weird
 continue;

 if (id == 0x2E) // TrackEntry ID

       ++count;
 
     pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
 
   if (count <= 0)
     return 0;  // success
 
  m_trackEntries = new (std::nothrow) Track*[count];
 
   if (m_trackEntries == NULL)
     return -1;

  m_trackEntriesEnd = m_trackEntries;

  pos = m_start;

 while (pos < stop) {
 const long long element_start = pos;

 long long id, payload_size;

 const long status =
 ParseElementHeader(pReader, pos, stop, id, payload_size);

 if (status < 0) // error
 return status;

 if (payload_size == 0) // weird
 continue;

 const long long payload_stop = pos + payload_size;
    assert(payload_stop <= stop); // checked in ParseElement

 const long long element_size = payload_stop - element_start;

 if (id == 0x2E) { // TrackEntry ID
 Track*& pTrack = *m_trackEntriesEnd;
      pTrack = NULL;

 const long status = ParseTrackEntry(pos, payload_size, element_start,
                                          element_size, pTrack);

 if (status)
 return status;

 if (pTrack)
 ++m_trackEntriesEnd;

     }
 
     pos = payload_stop;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
 
   return 0;  // success
 }
