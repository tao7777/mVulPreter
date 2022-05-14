const Chapters::Edition* Chapters::GetEdition(int idx) const
  const int size = (m_editions_size == 0) ? 1 : 2 * m_editions_size;
 
  Edition* const editions = new (std::nothrow) Edition[size];
 
  if (editions == NULL)
    return false;

  for (int idx = 0; idx < m_editions_count; ++idx) {
    m_editions[idx].ShallowCopy(editions[idx]);
  }

  delete[] m_editions;
  m_editions = editions;

  m_editions_size = size;
  return true;
 }
