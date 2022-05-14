long Track::ParseContentEncodingsEntry(long long start, long long size) {
 IMkvReader* const pReader = m_pSegment->m_pReader;
  assert(pReader);

 long long pos = start;
 const long long stop = start + size;

 int count = 0;
 while (pos < stop) {
 long long id, size;
 const long status = ParseElementHeader(pReader, pos, stop, id, size);
 if (status < 0) // error
 return status;

 if (id == 0x2240) // ContentEncoding ID

       ++count;
 
     pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
   if (count <= 0)
     return -1;
 
  content_encoding_entries_ = new (std::nothrow) ContentEncoding*[count];
   if (!content_encoding_entries_)
     return -1;
 
  content_encoding_entries_end_ = content_encoding_entries_;

  pos = start;
 while (pos < stop) {
 long long id, size;
 long status = ParseElementHeader(pReader, pos, stop, id, size);
 if (status < 0) // error
 return status;

 if (id == 0x2240) { // ContentEncoding ID
 ContentEncoding* const content_encoding =
 new (std::nothrow) ContentEncoding();
 if (!content_encoding)
 return -1;

      status = content_encoding->ParseContentEncodingEntry(pos, size, pReader);
 if (status) {
 delete content_encoding;
 return status;
 }

 *content_encoding_entries_end_++ = content_encoding;

     }
 
     pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
 
   return 0;
 }
