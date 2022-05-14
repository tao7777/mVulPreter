 const BlockEntry* Cluster::GetEntry(const CuePoint& cp,
                                     const CuePoint::TrackPosition& tp) const {
   assert(m_pSegment);
   const long long tc = cp.GetTimeCode();
 
   if (tp.m_block > 0) {
 const long block = static_cast<long>(tp.m_block);
 const long index = block - 1;

 while (index >= m_entries_count) {
 long long pos;
 long len;

 const long status = Parse(pos, len);

 if (status < 0) // TODO: can this happen?
 return NULL;

 if (status > 0) // nothing remains to be parsed
 return NULL;
 }

 const BlockEntry* const pEntry = m_entries[index];
    assert(pEntry);
    assert(!pEntry->EOS());

 const Block* const pBlock = pEntry->GetBlock();
    assert(pBlock);

 if ((pBlock->GetTrackNumber() == tp.m_track) &&
 (pBlock->GetTimeCode(this) == tc)) {
 return pEntry;
 }
 }

 long index = 0;

 for (;;) {
 if (index >= m_entries_count) {
 long long pos;
 long len;

 const long status = Parse(pos, len);

 if (status < 0) // TODO: can this happen?
 return NULL;

 if (status > 0) // nothing remains to be parsed
 return NULL;

      assert(m_entries);
      assert(index < m_entries_count);
 }

 const BlockEntry* const pEntry = m_entries[index];
    assert(pEntry);
    assert(!pEntry->EOS());

 const Block* const pBlock = pEntry->GetBlock();
    assert(pBlock);

 if (pBlock->GetTrackNumber() != tp.m_track) {
 ++index;
 continue;
 }

 const long long tc_ = pBlock->GetTimeCode(this);

 if (tc_ < tc) {
 ++index;
 continue;
 }

 if (tc_ > tc)
 return NULL;

 const Tracks* const pTracks = m_pSegment->GetTracks();
    assert(pTracks);

 const long tn = static_cast<long>(tp.m_track);
 const Track* const pTrack = pTracks->GetTrackByNumber(tn);

 if (pTrack == NULL)
 return NULL;

 const long long type = pTrack->GetType();

 if (type == 2) // audio
 return pEntry;

 if (type != 1) // not video
 return NULL;

 if (!pBlock->IsKey())
 return NULL;

 
     return pEntry;
   }
 }
