int Chapters::GetEditionCount() const
const Chapters::Edition* Chapters::GetEdition(int idx) const {
  if (idx < 0)
    return NULL;

  if (idx >= m_editions_count)
    return NULL;

  return m_editions + idx;
 }
