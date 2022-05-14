int SeekHead::GetCount() const
const SeekHead::VoidElement* SeekHead::GetVoidElement(int idx) const {
  if (idx < 0)
    return 0;

  if (idx >= m_void_element_count)
    return 0;

  return m_void_elements + idx;
 }
