long Cluster::GetNext(
  size_t idx = pCurr->GetIndex();
  assert(idx < size_t(m_entries_count));
  assert(m_entries[idx] == pCurr);

  ++idx;

  if (idx >= size_t(m_entries_count)) {
    long long pos;
    long len;

    const long status = Parse(pos, len);

    if (status < 0) {  // error
      pNext = NULL;
      return status;
    }

    if (status > 0) {
      pNext = NULL;
      return 0;
    }

     assert(m_entries);
     assert(m_entries_count > 0);
     assert(idx < size_t(m_entries_count));
  }
 
  pNext = m_entries[idx];
  assert(pNext);
 
  return 0;
 }
