Track::~Track()
long Track::Create(Segment* pSegment, const Info& info, long long element_start,
                   long long element_size, Track*& pResult) {
  if (pResult)
    return -1;
 
  Track* const pTrack =
      new (std::nothrow) Track(pSegment, element_start, element_size);
 
  if (pTrack == NULL)
    return -1;  // generic error
 
  const int status = info.Copy(pTrack->m_info);

  if (status) {  // error
    delete pTrack;
    return status;
  }

  pResult = pTrack;
  return 0;  // success
 }
