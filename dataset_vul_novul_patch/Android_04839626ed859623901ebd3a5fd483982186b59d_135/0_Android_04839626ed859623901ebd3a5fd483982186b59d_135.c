const Tracks* Segment::GetTracks() const
const SegmentInfo* Segment::GetInfo() const { return m_pInfo; }

const Cues* Segment::GetCues() const { return m_pCues; }

const Chapters* Segment::GetChapters() const { return m_pChapters; }

const SeekHead* Segment::GetSeekHead() const { return m_pSeekHead; }

long long Segment::GetDuration() const {
  assert(m_pInfo);
  return m_pInfo->GetDuration();
 }
