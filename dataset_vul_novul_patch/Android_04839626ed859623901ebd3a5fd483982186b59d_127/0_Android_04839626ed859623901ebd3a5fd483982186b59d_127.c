long long Cluster::GetTimeCode() const
  const long status = Load(pos, len);

  if (status < 0)  // error
    return status;

  return m_timecode;
}

long long Cluster::GetTime() const {
  const long long tc = GetTimeCode();

  if (tc < 0)
    return tc;

  const SegmentInfo* const pInfo = m_pSegment->GetInfo();
  assert(pInfo);

  const long long scale = pInfo->GetTimeCodeScale();
  assert(scale >= 1);

  const long long t = m_timecode * scale;

  return t;
}

long long Cluster::GetFirstTime() const {
  const BlockEntry* pEntry;

  const long status = GetFirst(pEntry);

  if (status < 0)  // error
    return status;

  if (pEntry == NULL)  // empty cluster
    return GetTime();

  const Block* const pBlock = pEntry->GetBlock();
  assert(pBlock);

  return pBlock->GetTime(this);
}

long long Cluster::GetLastTime() const {
  const BlockEntry* pEntry;

  const long status = GetLast(pEntry);

  if (status < 0)  // error
    return status;

  if (pEntry == NULL)  // empty cluster
    return GetTime();

  const Block* const pBlock = pEntry->GetBlock();
  assert(pBlock);

  return pBlock->GetTime(this);
}

long Cluster::CreateBlock(long long id,
                          long long pos,  // absolute pos of payload
                          long long size, long long discard_padding) {
  assert((id == 0x20) || (id == 0x23));  // BlockGroup or SimpleBlock

  if (m_entries_count < 0) {  // haven't parsed anything yet
    assert(m_entries == NULL);
    assert(m_entries_size == 0);

    m_entries_size = 1024;
    m_entries = new BlockEntry* [m_entries_size];

    m_entries_count = 0;
  } else {
    assert(m_entries);
    assert(m_entries_size > 0);
    assert(m_entries_count <= m_entries_size);

    if (m_entries_count >= m_entries_size) {
      const long entries_size = 2 * m_entries_size;

      BlockEntry** const entries = new BlockEntry* [entries_size];
      assert(entries);

      BlockEntry** src = m_entries;
      BlockEntry** const src_end = src + m_entries_count;

      BlockEntry** dst = entries;

      while (src != src_end)
        *dst++ = *src++;

      delete[] m_entries;

      m_entries = entries;
      m_entries_size = entries_size;
    }
  }

  if (id == 0x20)  // BlockGroup ID
    return CreateBlockGroup(pos, size, discard_padding);
  else  // SimpleBlock ID
    return CreateSimpleBlock(pos, size);
}

long Cluster::CreateBlockGroup(long long start_offset, long long size,
                               long long discard_padding) {
  assert(m_entries);
  assert(m_entries_size > 0);
  assert(m_entries_count >= 0);
  assert(m_entries_count < m_entries_size);

  IMkvReader* const pReader = m_pSegment->m_pReader;

  long long pos = start_offset;
  const long long stop = start_offset + size;

  // For WebM files, there is a bias towards previous reference times
  //(in order to support alt-ref frames, which refer back to the previous
  // keyframe).  Normally a 0 value is not possible, but here we tenatively
  // allow 0 as the value of a reference frame, with the interpretation
  // that this is a "previous" reference time.

  long long prev = 1;  // nonce
  long long next = 0;  // nonce
  long long duration = -1;  // really, this is unsigned

  long long bpos = -1;
  long long bsize = -1;

  while (pos < stop) {
    long len;
    const long long id = ReadUInt(pReader, pos, len);
    assert(id >= 0);  // TODO
    assert((pos + len) <= stop);

    pos += len;  // consume ID

    const long long size = ReadUInt(pReader, pos, len);
    assert(size >= 0);  // TODO
    assert((pos + len) <= stop);

    pos += len;  // consume size

    if (id == 0x21) {  // Block ID
      if (bpos < 0) {  // Block ID
        bpos = pos;
        bsize = size;
      }
    } else if (id == 0x1B) {  // Duration ID
      assert(size <= 8);

      duration = UnserializeUInt(pReader, pos, size);
      assert(duration >= 0);  // TODO
    } else if (id == 0x7B) {  // ReferenceBlock
      assert(size <= 8);
      const long size_ = static_cast<long>(size);

      long long time;

      long status = UnserializeInt(pReader, pos, size_, time);
      assert(status == 0);
      if (status != 0)
        return -1;

      if (time <= 0)  // see note above
        prev = time;
      else  // weird
        next = time;
    }

    pos += size;  // consume payload
    assert(pos <= stop);
  }

  assert(pos == stop);
  assert(bpos >= 0);
  assert(bsize >= 0);

  const long idx = m_entries_count;

  BlockEntry** const ppEntry = m_entries + idx;
  BlockEntry*& pEntry = *ppEntry;

  pEntry = new (std::nothrow)
      BlockGroup(this, idx, bpos, bsize, prev, next, duration, discard_padding);

  if (pEntry == NULL)
    return -1;  // generic error

  BlockGroup* const p = static_cast<BlockGroup*>(pEntry);

  const long status = p->Parse();

  if (status == 0) {  // success
    ++m_entries_count;
    return 0;
  }

  delete pEntry;
  pEntry = 0;

  return status;
}

long Cluster::CreateSimpleBlock(long long st, long long sz) {
  assert(m_entries);
  assert(m_entries_size > 0);
  assert(m_entries_count >= 0);
  assert(m_entries_count < m_entries_size);

  const long idx = m_entries_count;

  BlockEntry** const ppEntry = m_entries + idx;
  BlockEntry*& pEntry = *ppEntry;

  pEntry = new (std::nothrow) SimpleBlock(this, idx, st, sz);

  if (pEntry == NULL)
    return -1;  // generic error

  SimpleBlock* const p = static_cast<SimpleBlock*>(pEntry);

  const long status = p->Parse();

  if (status == 0) {
    ++m_entries_count;
    return 0;
  }

  delete pEntry;
  pEntry = 0;

  return status;
}

long Cluster::GetFirst(const BlockEntry*& pFirst) const {
  if (m_entries_count <= 0) {
     long long pos;
     long len;
 
    const long status = Parse(pos, len);
 
