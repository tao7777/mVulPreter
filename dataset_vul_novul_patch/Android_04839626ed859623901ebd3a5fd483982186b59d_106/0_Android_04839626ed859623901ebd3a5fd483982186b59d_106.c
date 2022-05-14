long Track::GetNext(
    for (;;) {
      const Block* const pBlock = pBlockEntry->GetBlock();
      assert(pBlock);
 
      const long long tn = pBlock->GetTrackNumber();
 
      if ((tn == m_info.number) && VetEntry(pBlockEntry))
        return 0;
 
      const BlockEntry* pNextEntry;
 
      status = pCluster->GetNext(pBlockEntry, pNextEntry);

      if (status < 0)  // error
         return status;
 
      if (pNextEntry == 0)
        break;
 
      pBlockEntry = pNextEntry;
    }
 
    ++i;
 
    if (i >= 100)
      break;
 
    pCluster = m_pSegment->GetNext(pCluster);
  }
 
  // NOTE: if we get here, it means that we didn't find a block with
  // a matching track number.  We interpret that as an error (which
  // might be too conservative).
 
  pBlockEntry = GetEOS();  // so we can return a non-NULL value
  return 1;
}
 
long Track::GetNext(const BlockEntry* pCurrEntry,
                    const BlockEntry*& pNextEntry) const {
  assert(pCurrEntry);
  assert(!pCurrEntry->EOS());  //?

  const Block* const pCurrBlock = pCurrEntry->GetBlock();
  assert(pCurrBlock && pCurrBlock->GetTrackNumber() == m_info.number);
  if (!pCurrBlock || pCurrBlock->GetTrackNumber() != m_info.number)
    return -1;

  const Cluster* pCluster = pCurrEntry->GetCluster();
  assert(pCluster);
  assert(!pCluster->EOS());

  long status = pCluster->GetNext(pCurrEntry, pNextEntry);

  if (status < 0)  // error
    return status;

  for (int i = 0;;) {
    while (pNextEntry) {
      const Block* const pNextBlock = pNextEntry->GetBlock();
      assert(pNextBlock);

      if (pNextBlock->GetTrackNumber() == m_info.number)
        return 0;

      pCurrEntry = pNextEntry;

      status = pCluster->GetNext(pCurrEntry, pNextEntry);

      if (status < 0)  // error
        return status;
    }

    pCluster = m_pSegment->GetNext(pCluster);

    if (pCluster == NULL) {
      pNextEntry = GetEOS();
      return 1;
    }

    if (pCluster->EOS()) {
 #if 0
             if (m_pSegment->Unparsed() <= 0)   //all clusters have been loaded
             {
                pNextEntry = GetEOS();

                 return 1;
             }
 #else
      if (m_pSegment->DoneParsing()) {
        pNextEntry = GetEOS();
        return 1;
      }
 #endif
 
      // TODO: there is a potential O(n^2) problem here: we tell the
      // caller to (pre)load another cluster, which he does, but then he
      // calls GetNext again, which repeats the same search.  This is
      // a pathological case, since the only way it can happen is if
      // there exists a long sequence of clusters none of which contain a
      // block from this track.  One way around this problem is for the
      // caller to be smarter when he loads another cluster: don't call
      // us back until you have a cluster that contains a block from this
      // track. (Of course, that's not cheap either, since our caller
      // would have to scan the each cluster as it's loaded, so that
      // would just push back the problem.)
 
      pNextEntry = NULL;
      return E_BUFFER_NOT_FULL;
     }
 
    status = pCluster->GetFirst(pNextEntry);
 
    if (status < 0)  // error
      return status;

    if (pNextEntry == NULL)  // empty cluster
      continue;

    ++i;

    if (i >= 100)
      break;
  }

  // NOTE: if we get here, it means that we didn't find a block with
  // a matching track number after lots of searching, so we give
  // up trying.

  pNextEntry = GetEOS();  // so we can return a non-NULL value
  return 1;
 }
