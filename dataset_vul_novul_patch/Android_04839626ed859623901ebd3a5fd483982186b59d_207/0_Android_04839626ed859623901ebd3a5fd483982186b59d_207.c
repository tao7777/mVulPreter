Track::Track(
Track::Track(Segment* pSegment, long long element_start, long long element_size)
    : m_pSegment(pSegment),
      m_element_start(element_start),
      m_element_size(element_size),
      content_encoding_entries_(NULL),
      content_encoding_entries_end_(NULL) {}

Track::~Track() {
  Info& info = const_cast<Info&>(m_info);
  info.Clear();

  ContentEncoding** i = content_encoding_entries_;
  ContentEncoding** const j = content_encoding_entries_end_;

  while (i != j) {
    ContentEncoding* const encoding = *i++;
    delete encoding;
  }

  delete[] content_encoding_entries_;
 }
