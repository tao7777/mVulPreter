 long Segment::DoLoadClusterUnknownSize(long long& pos, long& len) {
  if (m_pos >= 0 || m_pUnknownSize == NULL)
    return E_PARSE_FAILED;
 
   const long status = m_pUnknownSize->Parse(pos, len);
 
   if (status < 0)  // error or underflow
 return status;


   if (status == 0)  // parsed a block
     return 2;  // continue parsing
 
   const long long start = m_pUnknownSize->m_element_start;
   const long long size = m_pUnknownSize->GetElementSize();

  if (size < 0)
    return E_FILE_FORMAT_INVALID;
 
   pos = start + size;
   m_pos = pos;


   m_pUnknownSize = 0;
 
   return 2;  // continue parsing
 }
