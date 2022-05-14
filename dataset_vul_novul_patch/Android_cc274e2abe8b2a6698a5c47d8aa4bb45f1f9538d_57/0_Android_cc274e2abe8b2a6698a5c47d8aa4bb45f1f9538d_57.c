long Tracks::ParseTrackEntry(long long track_start, long long track_size,
 long long element_start, long long element_size,
 Track*& pResult) const {
 if (pResult)
 return -1;

 IMkvReader* const pReader = m_pSegment->m_pReader;

 long long pos = track_start;
 const long long track_stop = track_start + track_size;

 Track::Info info;

  info.type = 0;
  info.number = 0;
  info.uid = 0;
  info.defaultDuration = 0;

 Track::Settings v;
  v.start = -1;
  v.size = -1;

 Track::Settings a;
  a.start = -1;
  a.size = -1;

 Track::Settings e; // content_encodings_settings;
  e.start = -1;
  e.size = -1;

 long long lacing = 1; // default is true

 while (pos < track_stop) {
 long long id, size;

 const long status = ParseElementHeader(pReader, pos, track_stop, id, size);

 if (status < 0) // error
 return status;

 if (size < 0)
 return E_FILE_FORMAT_INVALID;

 const long long start = pos;

 if (id == 0x60) { // VideoSettings ID
      v.start = start;
      v.size = size;
 } else if (id == 0x61) { // AudioSettings ID
      a.start = start;
      a.size = size;
 } else if (id == 0x2D80) { // ContentEncodings ID
      e.start = start;
      e.size = size;
 } else if (id == 0x33C5) { // Track UID
 if (size > 8)
 return E_FILE_FORMAT_INVALID;

      info.uid = 0;

 long long pos_ = start;
 const long long pos_end = start + size;

 while (pos_ != pos_end) {
 unsigned char b;

 const int status = pReader->Read(pos_, 1, &b);

 if (status)
 return status;

        info.uid <<= 8;
        info.uid |= b;

 ++pos_;
 }
 } else if (id == 0x57) { // Track Number
 const long long num = UnserializeUInt(pReader, pos, size);

 if ((num <= 0) || (num > 127))
 return E_FILE_FORMAT_INVALID;

      info.number = static_cast<long>(num);
 } else if (id == 0x03) { // Track Type
 const long long type = UnserializeUInt(pReader, pos, size);

 if ((type <= 0) || (type > 254))
 return E_FILE_FORMAT_INVALID;

      info.type = static_cast<long>(type);
 } else if (id == 0x136E) { // Track Name
 const long status =
 UnserializeString(pReader, pos, size, info.nameAsUTF8);

 if (status)
 return status;
 } else if (id == 0x02B59C) { // Track Language
 const long status = UnserializeString(pReader, pos, size, info.language);

 if (status)
 return status;
 } else if (id == 0x03E383) { // Default Duration
 const long long duration = UnserializeUInt(pReader, pos, size);

 if (duration < 0)
 return E_FILE_FORMAT_INVALID;

      info.defaultDuration = static_cast<unsigned long long>(duration);
 } else if (id == 0x06) { // CodecID
 const long status = UnserializeString(pReader, pos, size, info.codecId);

 if (status)
 return status;
 } else if (id == 0x1C) { // lacing
      lacing = UnserializeUInt(pReader, pos, size);

 if ((lacing < 0) || (lacing > 1))
 return E_FILE_FORMAT_INVALID;
 } else if (id == 0x23A2) { // Codec Private
 delete[] info.codecPrivate;
      info.codecPrivate = NULL;
      info.codecPrivateSize = 0;


       const size_t buflen = static_cast<size_t>(size);
 
       if (buflen) {
        unsigned char* buf = SafeArrayAlloc<unsigned char>(1, buflen);
 
         if (buf == NULL)
           return -1;

 const int status = pReader->Read(pos, static_cast<long>(buflen), buf);

 if (status) {
 delete[] buf;
 return status;
 }

        info.codecPrivate = buf;
        info.codecPrivateSize = buflen;
 }
 } else if (id == 0x058688) { // Codec Name
 const long status =
 UnserializeString(pReader, pos, size, info.codecNameAsUTF8);

 if (status)
 return status;
 } else if (id == 0x16AA) { // Codec Delay
      info.codecDelay = UnserializeUInt(pReader, pos, size);
 } else if (id == 0x16BB) { // Seek Pre Roll
      info.seekPreRoll = UnserializeUInt(pReader, pos, size);

     }
 
     pos += size;  // consume payload
    if (pos > track_stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != track_stop)
    return E_FILE_FORMAT_INVALID;
 
   if (info.number <= 0)  // not specified
     return E_FILE_FORMAT_INVALID;

 if (GetTrackByNumber(info.number))
 return E_FILE_FORMAT_INVALID;

 if (info.type <= 0) // not specified
 return E_FILE_FORMAT_INVALID;

  info.lacing = (lacing > 0) ? true : false;

 if (info.type == Track::kVideo) {
 if (v.start < 0)
 return E_FILE_FORMAT_INVALID;

 if (a.start >= 0)
 return E_FILE_FORMAT_INVALID;

    info.settings = v;

 VideoTrack* pTrack = NULL;

 const long status = VideoTrack::Parse(m_pSegment, info, element_start,
                                          element_size, pTrack);

 if (status)
 return status;

    pResult = pTrack;
    assert(pResult);

 if (e.start >= 0)
      pResult->ParseContentEncodingsEntry(e.start, e.size);
 } else if (info.type == Track::kAudio) {
 if (a.start < 0)
 return E_FILE_FORMAT_INVALID;

 if (v.start >= 0)
 return E_FILE_FORMAT_INVALID;

    info.settings = a;

 AudioTrack* pTrack = NULL;

 const long status = AudioTrack::Parse(m_pSegment, info, element_start,
                                          element_size, pTrack);

 if (status)
 return status;

    pResult = pTrack;
    assert(pResult);

 if (e.start >= 0)
      pResult->ParseContentEncodingsEntry(e.start, e.size);
 } else {

 if (a.start >= 0)
 return E_FILE_FORMAT_INVALID;

 if (v.start >= 0)
 return E_FILE_FORMAT_INVALID;

 if (info.type == Track::kMetadata && e.start >= 0)
 return E_FILE_FORMAT_INVALID;

    info.settings.start = -1;
    info.settings.size = 0;

 Track* pTrack = NULL;

 const long status =
 Track::Create(m_pSegment, info, element_start, element_size, pTrack);

 if (status)
 return status;

    pResult = pTrack;
    assert(pResult);
 }

 return 0; // success
}
