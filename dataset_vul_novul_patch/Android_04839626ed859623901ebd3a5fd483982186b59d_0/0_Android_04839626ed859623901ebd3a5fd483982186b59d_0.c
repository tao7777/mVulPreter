Chapters::Atom::Atom()
long long SegmentInfo::GetDuration() const {
  if (m_duration < 0)
    return -1;

  assert(m_timecodeScale >= 1);

  const double dd = double(m_duration) * double(m_timecodeScale);
  const long long d = static_cast<long long>(dd);

  return d;
 }
