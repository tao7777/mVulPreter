long Chapters::Display::Parse(IMkvReader* pReader, long long pos,
 long long size) {
 const long long stop = pos + size;

 while (pos < stop) {
 long long id, size;

 long status = ParseElementHeader(pReader, pos, stop, id, size);

 if (status < 0) // error
 return status;

 if (size == 0) // weird
 continue;

 if (id == 0x05) { // ChapterString ID
      status = UnserializeString(pReader, pos, size, m_string);

 if (status)
 return status;
 } else if (id == 0x037C) { // ChapterLanguage ID
      status = UnserializeString(pReader, pos, size, m_language);

 if (status)
 return status;
 } else if (id == 0x037E) { // ChapterCountry ID
      status = UnserializeString(pReader, pos, size, m_country);

 if (status)
 return status;

     }
 
     pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
  return 0;
}

Tags::Tags(Segment* pSegment, long long payload_start, long long payload_size,
           long long element_start, long long element_size)
    : m_pSegment(pSegment),
      m_start(payload_start),
      m_size(payload_size),
      m_element_start(element_start),
      m_element_size(element_size),
      m_tags(NULL),
      m_tags_size(0),
      m_tags_count(0) {}

Tags::~Tags() {
  while (m_tags_count > 0) {
    Tag& t = m_tags[--m_tags_count];
    t.Clear();
  }
  delete[] m_tags;
}

long Tags::Parse() {
  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long pos = m_start;  // payload start
  const long long stop = pos + m_size;  // payload stop

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)
      return status;

    if (size == 0)  // 0 length tag, read another
      continue;

    if (id == 0x3373) {  // Tag ID
      status = ParseTag(pos, size);

      if (status < 0)
        return status;
    }

    pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;

  return 0;
}

int Tags::GetTagCount() const { return m_tags_count; }

const Tags::Tag* Tags::GetTag(int idx) const {
  if (idx < 0)
    return NULL;

  if (idx >= m_tags_count)
    return NULL;

  return m_tags + idx;
}

bool Tags::ExpandTagsArray() {
  if (m_tags_size > m_tags_count)
    return true;  // nothing else to do

  const int size = (m_tags_size == 0) ? 1 : 2 * m_tags_size;

  Tag* const tags = new (std::nothrow) Tag[size];

  if (tags == NULL)
    return false;

  for (int idx = 0; idx < m_tags_count; ++idx) {
    m_tags[idx].ShallowCopy(tags[idx]);
  }

  delete[] m_tags;
  m_tags = tags;

  m_tags_size = size;
  return true;
}

long Tags::ParseTag(long long pos, long long size) {
  if (!ExpandTagsArray())
    return -1;

  Tag& t = m_tags[m_tags_count++];
  t.Init();

  return t.Parse(m_pSegment->m_pReader, pos, size);
}

Tags::Tag::Tag() {}

Tags::Tag::~Tag() {}

int Tags::Tag::GetSimpleTagCount() const { return m_simple_tags_count; }

const Tags::SimpleTag* Tags::Tag::GetSimpleTag(int index) const {
  if (index < 0)
    return NULL;

  if (index >= m_simple_tags_count)
    return NULL;

  return m_simple_tags + index;
}

void Tags::Tag::Init() {
  m_simple_tags = NULL;
  m_simple_tags_size = 0;
  m_simple_tags_count = 0;
}

void Tags::Tag::ShallowCopy(Tag& rhs) const {
  rhs.m_simple_tags = m_simple_tags;
  rhs.m_simple_tags_size = m_simple_tags_size;
  rhs.m_simple_tags_count = m_simple_tags_count;
}

void Tags::Tag::Clear() {
  while (m_simple_tags_count > 0) {
    SimpleTag& d = m_simple_tags[--m_simple_tags_count];
    d.Clear();
  }

  delete[] m_simple_tags;
  m_simple_tags = NULL;

  m_simple_tags_size = 0;
}

long Tags::Tag::Parse(IMkvReader* pReader, long long pos, long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)
      return status;

    if (size == 0)  // 0 length tag, read another
      continue;

    if (id == 0x27C8) {  // SimpleTag ID
      status = ParseSimpleTag(pReader, pos, size);

      if (status < 0)
        return status;
    }

    pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
  return 0;
}

long Tags::Tag::ParseSimpleTag(IMkvReader* pReader, long long pos,
                               long long size) {
  if (!ExpandSimpleTagsArray())
    return -1;

  SimpleTag& st = m_simple_tags[m_simple_tags_count++];
  st.Init();

  return st.Parse(pReader, pos, size);
}

bool Tags::Tag::ExpandSimpleTagsArray() {
  if (m_simple_tags_size > m_simple_tags_count)
    return true;  // nothing else to do

  const int size = (m_simple_tags_size == 0) ? 1 : 2 * m_simple_tags_size;

  SimpleTag* const displays = new (std::nothrow) SimpleTag[size];

  if (displays == NULL)
    return false;

  for (int idx = 0; idx < m_simple_tags_count; ++idx) {
    m_simple_tags[idx].ShallowCopy(displays[idx]);
  }

  delete[] m_simple_tags;
  m_simple_tags = displays;

  m_simple_tags_size = size;
  return true;
}

Tags::SimpleTag::SimpleTag() {}

Tags::SimpleTag::~SimpleTag() {}

const char* Tags::SimpleTag::GetTagName() const { return m_tag_name; }

const char* Tags::SimpleTag::GetTagString() const { return m_tag_string; }

void Tags::SimpleTag::Init() {
  m_tag_name = NULL;
  m_tag_string = NULL;
}

void Tags::SimpleTag::ShallowCopy(SimpleTag& rhs) const {
  rhs.m_tag_name = m_tag_name;
  rhs.m_tag_string = m_tag_string;
}

void Tags::SimpleTag::Clear() {
  delete[] m_tag_name;
  m_tag_name = NULL;

  delete[] m_tag_string;
  m_tag_string = NULL;
}

long Tags::SimpleTag::Parse(IMkvReader* pReader, long long pos,
                            long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // weird
      continue;

    if (id == 0x5A3) {  // TagName ID
      status = UnserializeString(pReader, pos, size, m_tag_name);

      if (status)
        return status;
    } else if (id == 0x487) {  // TagString ID
      status = UnserializeString(pReader, pos, size, m_tag_string);

      if (status)
        return status;
    }

    pos += size;
    if (pos > stop)
      return E_FILE_FORMAT_INVALID;
  }

  if (pos != stop)
    return E_FILE_FORMAT_INVALID;
   return 0;
 }
