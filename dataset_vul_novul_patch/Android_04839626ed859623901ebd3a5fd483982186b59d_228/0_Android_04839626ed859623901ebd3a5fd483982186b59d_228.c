Chapters::Edition::~Edition()
Chapters::Atom::~Atom() {}

unsigned long long Chapters::Atom::GetUID() const { return m_uid; }

const char* Chapters::Atom::GetStringUID() const { return m_string_uid; }

long long Chapters::Atom::GetStartTimecode() const { return m_start_timecode; }

long long Chapters::Atom::GetStopTimecode() const { return m_stop_timecode; }

long long Chapters::Atom::GetStartTime(const Chapters* pChapters) const {
  return GetTime(pChapters, m_start_timecode);
 }
