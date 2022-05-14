long Track::GetFirst(const BlockEntry*& pBlockEntry) const {
 const Cluster* pCluster = m_pSegment->GetFirst();

 for (int i = 0;;) {
 if (pCluster == NULL) {
      pBlockEntry = GetEOS();
 return 1;

     }
 
     if (pCluster->EOS()) {
#if 0
            if (m_pSegment->Unparsed() <= 0) {  //all clusters have been loaded
                pBlockEntry = GetEOS();
                return 1;
            }
#else
       if (m_pSegment->DoneParsing()) {
         pBlockEntry = GetEOS();
         return 1;
       }
#endif
 
       pBlockEntry = 0;
       return E_BUFFER_NOT_FULL;
 }

 long status = pCluster->GetFirst(pBlockEntry);

 if (status < 0) // error
 return status;

 if (pBlockEntry == 0) { // empty cluster
      pCluster = m_pSegment->GetNext(pCluster);
 continue;
 }

 for (;;) {
 const Block* const pBlock = pBlockEntry->GetBlock();
      assert(pBlock);

 const long long tn = pBlock->GetTrackNumber();

 if ((tn == m_info.number) && VetEntry(pBlockEntry))
 return 0;

 const BlockEntry* pNextEntry;

      status = pCluster->GetNext(pBlockEntry, pNextEntry);

 if (status < 0) // error
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


  pBlockEntry = GetEOS(); // so we can return a non-NULL value
 return 1;
}
