int Track::Info::Copy(Info& dst) const
  dst.type = type;
  dst.number = number;
  dst.defaultDuration = defaultDuration;
  dst.codecDelay = codecDelay;
  dst.seekPreRoll = seekPreRoll;
  dst.uid = uid;
  dst.lacing = lacing;
  dst.settings = settings;
 
  // We now copy the string member variables from src to dst.
  // This involves memory allocation so in principle the operation
  // can fail (indeed, that's why we have Info::Copy), so we must
  // report this to the caller.  An error return from this function
  // therefore implies that the copy was only partially successful.
 
  if (int status = CopyStr(&Info::nameAsUTF8, dst))
    return status;
 
  if (int status = CopyStr(&Info::language, dst))
    return status;
 
  if (int status = CopyStr(&Info::codecId, dst))
    return status;
 
  if (int status = CopyStr(&Info::codecNameAsUTF8, dst))
    return status;
 
  if (codecPrivateSize > 0) {
    if (codecPrivate == NULL)
      return -1;
 
    if (dst.codecPrivate)
      return -1;
 
    if (dst.codecPrivateSize != 0)
      return -1;
 
    dst.codecPrivate = new (std::nothrow) unsigned char[codecPrivateSize];
 
    if (dst.codecPrivate == NULL)
      return -1;
 
    memcpy(dst.codecPrivate, codecPrivate, codecPrivateSize);
    dst.codecPrivateSize = codecPrivateSize;
  }
 
  return 0;
}

const BlockEntry* Track::GetEOS() const { return &m_eos; }

long Track::GetType() const { return m_info.type; }

long Track::GetNumber() const { return m_info.number; }

unsigned long long Track::GetUid() const { return m_info.uid; }

const char* Track::GetNameAsUTF8() const { return m_info.nameAsUTF8; }

const char* Track::GetLanguage() const { return m_info.language; }

const char* Track::GetCodecNameAsUTF8() const { return m_info.codecNameAsUTF8; }

const char* Track::GetCodecId() const { return m_info.codecId; }

const unsigned char* Track::GetCodecPrivate(size_t& size) const {
  size = m_info.codecPrivateSize;
  return m_info.codecPrivate;
}

bool Track::GetLacing() const { return m_info.lacing; }

unsigned long long Track::GetDefaultDuration() const {
  return m_info.defaultDuration;
}

unsigned long long Track::GetCodecDelay() const { return m_info.codecDelay; }

unsigned long long Track::GetSeekPreRoll() const { return m_info.seekPreRoll; }

long Track::GetFirst(const BlockEntry*& pBlockEntry) const {
  const Cluster* pCluster = m_pSegment->GetFirst();

  for (int i = 0;;) {
    if (pCluster == NULL) {
      pBlockEntry = GetEOS();
      return 1;
     }
 
