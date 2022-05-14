long VideoTrack::Seek(
      if (status < 0)
         return status;
 
      if (rate <= 0)
        return E_FILE_FORMAT_INVALID;
    }
 
    pos += size;  // consume payload
    assert(pos <= stop);
  }
 
  assert(pos == stop);

  VideoTrack* const pTrack =
      new (std::nothrow) VideoTrack(pSegment, element_start, element_size);

  if (pTrack == NULL)
    return -1;  // generic error

  const int status = info.Copy(pTrack->m_info);

  if (status) {  // error
    delete pTrack;
    return status;
  }

  pTrack->m_width = width;
  pTrack->m_height = height;
  pTrack->m_rate = rate;

  pResult = pTrack;
  return 0;  // success
}

bool VideoTrack::VetEntry(const BlockEntry* pBlockEntry) const {
  return Track::VetEntry(pBlockEntry) && pBlockEntry->GetBlock()->IsKey();
}

long VideoTrack::Seek(long long time_ns, const BlockEntry*& pResult) const {
  const long status = GetFirst(pResult);

  if (status < 0)  // buffer underflow, etc
    return status;

  assert(pResult);

  if (pResult->EOS())
    return 0;

  const Cluster* pCluster = pResult->GetCluster();
  assert(pCluster);
  assert(pCluster->GetIndex() >= 0);

  if (time_ns <= pResult->GetBlock()->GetTime(pCluster))
    return 0;

  Cluster** const clusters = m_pSegment->m_clusters;
  assert(clusters);

  const long count = m_pSegment->GetCount();  // loaded only, not pre-loaded
  assert(count > 0);

  Cluster** const i = clusters + pCluster->GetIndex();
  assert(i);
  assert(*i == pCluster);
  assert(pCluster->GetTime() <= time_ns);

  Cluster** const j = clusters + count;

  Cluster** lo = i;
  Cluster** hi = j;

  while (lo < hi) {
    // INVARIANT:
    //[i, lo) <= time_ns
    //[lo, hi) ?
    //[hi, j)  > time_ns

    Cluster** const mid = lo + (hi - lo) / 2;
    assert(mid < hi);

    pCluster = *mid;
     assert(pCluster);
     assert(pCluster->GetIndex() >= 0);
    assert(pCluster->GetIndex() == long(mid - m_pSegment->m_clusters));
 
    const long long t = pCluster->GetTime();
 
    if (t <= time_ns)
      lo = mid + 1;
    else
      hi = mid;
 
    assert(lo <= hi);
  }
 
  assert(lo == hi);
  assert(lo > i);
  assert(lo <= j);
 
  pCluster = *--lo;
  assert(pCluster);
  assert(pCluster->GetTime() <= time_ns);
 
  pResult = pCluster->GetEntry(this, time_ns);
 
  if ((pResult != 0) && !pResult->EOS())  // found a keyframe
    return 0;
 
  while (lo != i) {
     pCluster = *--lo;
     assert(pCluster);
     assert(pCluster->GetTime() <= time_ns);
 
 #if 0

         pResult = pCluster->GetMaxKey(this);
 #else
    pResult = pCluster->GetEntry(this, time_ns);
 #endif
 
    if ((pResult != 0) && !pResult->EOS())
      return 0;
  }

  // weird: we're on the first cluster, but no keyframe found
  // should never happen but we must return something anyway

  pResult = GetEOS();
  return 0;
}

long long VideoTrack::GetWidth() const { return m_width; }

long long VideoTrack::GetHeight() const { return m_height; }

double VideoTrack::GetFrameRate() const { return m_rate; }

AudioTrack::AudioTrack(Segment* pSegment, long long element_start,
                       long long element_size)
    : Track(pSegment, element_start, element_size) {}

long AudioTrack::Parse(Segment* pSegment, const Info& info,
                       long long element_start, long long element_size,
                       AudioTrack*& pResult) {
  if (pResult)
    return -1;

  if (info.type != Track::kAudio)
    return -1;

  IMkvReader* const pReader = pSegment->m_pReader;

  const Settings& s = info.settings;
  assert(s.start >= 0);
  assert(s.size >= 0);

  long long pos = s.start;
  assert(pos >= 0);

  const long long stop = pos + s.size;

  double rate = 8000.0;  // MKV default
  long long channels = 1;
  long long bit_depth = 0;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (id == 0x35) {  // Sample Rate
      status = UnserializeFloat(pReader, pos, size, rate);

      if (status < 0)
        return status;

      if (rate <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x1F) {  // Channel Count
      channels = UnserializeUInt(pReader, pos, size);

      if (channels <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x2264) {  // Bit Depth
      bit_depth = UnserializeUInt(pReader, pos, size);

      if (bit_depth <= 0)
        return E_FILE_FORMAT_INVALID;
     }
 
    pos += size;  // consume payload
    assert(pos <= stop);
  }
 
  assert(pos == stop);

  AudioTrack* const pTrack =
      new (std::nothrow) AudioTrack(pSegment, element_start, element_size);

  if (pTrack == NULL)
    return -1;  // generic error

  const int status = info.Copy(pTrack->m_info);

  if (status) {
    delete pTrack;
    return status;
  }

  pTrack->m_rate = rate;
  pTrack->m_channels = channels;
  pTrack->m_bitDepth = bit_depth;

  pResult = pTrack;
  return 0;  // success
 }
