long SeekHead::Parse() {
 IMkvReader* const pReader = m_pSegment->m_pReader;

 long long pos = m_start;
 const long long stop = m_start + m_size;


 int entry_count = 0;
 int void_element_count = 0;

 while (pos < stop) {
 long long id, size;

 const long status = ParseElementHeader(pReader, pos, stop, id, size);

 if (status < 0) // error
 return status;

 if (id == 0x0DBB) // SeekEntry ID
 ++entry_count;
 else if (id == 0x6C) // Void ID

       ++void_element_count;
 
     pos += size;  // consume payload

    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
 
   m_entries = new (std::nothrow) Entry[entry_count];
 
 if (m_entries == NULL)
 return -1;

  m_void_elements = new (std::nothrow) VoidElement[void_element_count];

 if (m_void_elements == NULL)
 return -1;


 Entry* pEntry = m_entries;
 VoidElement* pVoidElement = m_void_elements;

  pos = m_start;

 while (pos < stop) {
 const long long idpos = pos;

 long long id, size;

 const long status = ParseElementHeader(pReader, pos, stop, id, size);

 if (status < 0) // error
 return status;

 if (id == 0x0DBB) { // SeekEntry ID
 if (ParseEntry(pReader, pos, size, pEntry)) {
 Entry& e = *pEntry++;

        e.element_start = idpos;
        e.element_size = (pos + size) - idpos;
 }
 } else if (id == 0x6C) { // Void ID
 VoidElement& e = *pVoidElement++;

      e.element_start = idpos;
      e.element_size = (pos + size) - idpos;

     }
 
     pos += size;  // consume payload
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
 
   ptrdiff_t count_ = ptrdiff_t(pEntry - m_entries);
   assert(count_ >= 0);
  assert(count_ <= entry_count);

  m_entry_count = static_cast<int>(count_);

  count_ = ptrdiff_t(pVoidElement - m_void_elements);
  assert(count_ >= 0);
  assert(count_ <= void_element_count);

  m_void_element_count = static_cast<int>(count_);

 return 0;
}
