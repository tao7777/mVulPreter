long VideoTrack::Parse(Segment* pSegment, const Info& info,
 long long element_start, long long element_size,
 VideoTrack*& pResult) {
 if (pResult)
 return -1;

 if (info.type != Track::kVideo)
 return -1;

 
   long long width = 0;
   long long height = 0;
  long long display_width = 0;
  long long display_height = 0;
  long long display_unit = 0;
  long long stereo_mode = 0;

   double rate = 0.0;
 
   IMkvReader* const pReader = pSegment->m_pReader;

 const Settings& s = info.settings;
  assert(s.start >= 0);
  assert(s.size >= 0);

 long long pos = s.start;
  assert(pos >= 0);

 const long long stop = pos + s.size;

 while (pos < stop) {
 long long id, size;

 const long status = ParseElementHeader(pReader, pos, stop, id, size);

 if (status < 0) // error
 return status;

 if (id == 0x30) { // pixel width
      width = UnserializeUInt(pReader, pos, size);

 if (width <= 0)
 return E_FILE_FORMAT_INVALID;
 } else if (id == 0x3A) { // pixel height
      height = UnserializeUInt(pReader, pos, size);

 
       if (height <= 0)
         return E_FILE_FORMAT_INVALID;
    } else if (id == 0x14B0) {  // display width
      display_width = UnserializeUInt(pReader, pos, size);

      if (display_width <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x14BA) {  // display height
      display_height = UnserializeUInt(pReader, pos, size);

      if (display_height <= 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x14B2) {  // display unit
      display_unit = UnserializeUInt(pReader, pos, size);

      if (display_unit < 0)
        return E_FILE_FORMAT_INVALID;
    } else if (id == 0x13B8) {  // stereo mode
      stereo_mode = UnserializeUInt(pReader, pos, size);

      if (stereo_mode < 0)
        return E_FILE_FORMAT_INVALID;
     } else if (id == 0x0383E3) {  // frame rate
       const long status = UnserializeFloat(pReader, pos, size, rate);
 
 if (status < 0)
 return status;

 if (rate <= 0)
 return E_FILE_FORMAT_INVALID;

     }
 
     pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
 
   VideoTrack* const pTrack =
       new (std::nothrow) VideoTrack(pSegment, element_start, element_size);

 if (pTrack == NULL)
 return -1; // generic error

 const int status = info.Copy(pTrack->m_info);

 if (status) { // error
 delete pTrack;
 return status;
 }

 
   pTrack->m_width = width;
   pTrack->m_height = height;
  pTrack->m_display_width = display_width;
  pTrack->m_display_height = display_height;
  pTrack->m_display_unit = display_unit;
  pTrack->m_stereo_mode = stereo_mode;
   pTrack->m_rate = rate;
 
   pResult = pTrack;
 return 0; // success
}
