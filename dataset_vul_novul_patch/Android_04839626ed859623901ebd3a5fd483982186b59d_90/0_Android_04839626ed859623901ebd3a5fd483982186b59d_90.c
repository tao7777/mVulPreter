long Cluster::GetIndex() const
Cluster::Cluster(Segment* pSegment, long idx, long long element_start
                 /* long long element_size */)
    : m_pSegment(pSegment),
      m_element_start(element_start),
      m_index(idx),
      m_pos(element_start),
      m_element_size(-1 /* element_size */),
      m_timecode(-1),
      m_entries(NULL),
      m_entries_size(0),
      m_entries_count(-1)  // means "has not been parsed yet"
{}

Cluster::~Cluster() {
  if (m_entries_count <= 0)
    return;

  BlockEntry** i = m_entries;
  BlockEntry** const j = m_entries + m_entries_count;

  while (i != j) {
    BlockEntry* p = *i++;
    assert(p);

    delete p;
  }

  delete[] m_entries;
 }
