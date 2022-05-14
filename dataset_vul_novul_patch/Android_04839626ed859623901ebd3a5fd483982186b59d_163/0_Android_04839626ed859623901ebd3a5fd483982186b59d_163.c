long Chapters::Edition::Parse(
  Segment* const pSegment = pChapters->m_pSegment;
 
  if (pSegment == NULL)  // weird
    return -1;
 
  const SegmentInfo* const pInfo = pSegment->GetInfo();
 
  if (pInfo == NULL)
    return -1;
 
  const long long timecode_scale = pInfo->GetTimeCodeScale();
 
  if (timecode_scale < 1)  // weird
    return -1;
 
  if (timecode < 0)
    return -1;
 
  const long long result = timecode_scale * timecode;

  return result;
}

long Chapters::Atom::ParseDisplay(IMkvReader* pReader, long long pos,
                                  long long size) {
  if (!ExpandDisplaysArray())
    return -1;

  Display& d = m_displays[m_displays_count++];
  d.Init();

  return d.Parse(pReader, pos, size);
}

bool Chapters::Atom::ExpandDisplaysArray() {
  if (m_displays_size > m_displays_count)
    return true;  // nothing else to do

  const int size = (m_displays_size == 0) ? 1 : 2 * m_displays_size;

  Display* const displays = new (std::nothrow) Display[size];

  if (displays == NULL)
    return false;

  for (int idx = 0; idx < m_displays_count; ++idx) {
    m_displays[idx].ShallowCopy(displays[idx]);
  }

  delete[] m_displays;
  m_displays = displays;

  m_displays_size = size;
  return true;
}

Chapters::Display::Display() {}

Chapters::Display::~Display() {}

const char* Chapters::Display::GetString() const { return m_string; }

const char* Chapters::Display::GetLanguage() const { return m_language; }

const char* Chapters::Display::GetCountry() const { return m_country; }

void Chapters::Display::Init() {
  m_string = NULL;
  m_language = NULL;
  m_country = NULL;
}

void Chapters::Display::ShallowCopy(Display& rhs) const {
  rhs.m_string = m_string;
  rhs.m_language = m_language;
  rhs.m_country = m_country;
}

void Chapters::Display::Clear() {
  delete[] m_string;
  m_string = NULL;

  delete[] m_language;
  m_language = NULL;

  delete[] m_country;
  m_country = NULL;
}

long Chapters::Display::Parse(IMkvReader* pReader, long long pos,
                              long long size) {
  const long long stop = pos + size;

  while (pos < stop) {
    long long id, size;

    long status = ParseElementHeader(pReader, pos, stop, id, size);

    if (status < 0)  // error
      return status;

    if (size == 0)  // weird
      continue;

    if (id == 0x05) {  // ChapterString ID
      status = UnserializeString(pReader, pos, size, m_string);

      if (status)
        return status;
    } else if (id == 0x037C) {  // ChapterLanguage ID
      status = UnserializeString(pReader, pos, size, m_language);

      if (status)
        return status;
    } else if (id == 0x037E) {  // ChapterCountry ID
      status = UnserializeString(pReader, pos, size, m_country);

      if (status)
        return status;
     }
 
    pos += size;
    assert(pos <= stop);
  }

  assert(pos == stop);
  return 0;
 }
