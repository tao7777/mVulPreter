long Track::GetNext(const BlockEntry* pCurrEntry,
 const BlockEntry*& pNextEntry) const {
  assert(pCurrEntry);
  assert(!pCurrEntry->EOS()); //?

 const Block* const pCurrBlock = pCurrEntry->GetBlock();
  assert(pCurrBlock && pCurrBlock->GetTrackNumber() == m_info.number);
 if (!pCurrBlock || pCurrBlock->GetTrackNumber() != m_info.number)
 return -1;

 const Cluster* pCluster = pCurrEntry->GetCluster();
  assert(pCluster);
  assert(!pCluster->EOS());

 long status = pCluster->GetNext(pCurrEntry, pNextEntry);

 if (status < 0) // error
 return status;

 for (int i = 0;;) {
 while (pNextEntry) {
 const Block* const pNextBlock = pNextEntry->GetBlock();
      assert(pNextBlock);

 if (pNextBlock->GetTrackNumber() == m_info.number)
 return 0;

      pCurrEntry = pNextEntry;

      status = pCluster->GetNext(pCurrEntry, pNextEntry);

 if (status < 0) // error
 return status;
 }

    pCluster = m_pSegment->GetNext(pCluster);

 if (pCluster == NULL) {
      pNextEntry = GetEOS();
 return 1;

     }
 
     if (pCluster->EOS()) {
       if (m_pSegment->DoneParsing()) {
         pNextEntry = GetEOS();
         return 1;
       }
 

      pNextEntry = NULL;
 return E_BUFFER_NOT_FULL;
 }

    status = pCluster->GetFirst(pNextEntry);

 if (status < 0) // error
 return status;

 if (pNextEntry == NULL) // empty cluster
 continue;

 ++i;

 if (i >= 100)
 break;
 }


  pNextEntry = GetEOS(); // so we can return a non-NULL value
 return 1;
}
