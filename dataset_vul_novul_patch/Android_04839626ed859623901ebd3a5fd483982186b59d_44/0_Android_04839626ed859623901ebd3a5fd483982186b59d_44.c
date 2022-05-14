int Chapters::Edition::GetAtomCount() const
long long Chapters::Atom::GetStopTime(const Chapters* pChapters) const {
  return GetTime(pChapters, m_stop_timecode);
 }
