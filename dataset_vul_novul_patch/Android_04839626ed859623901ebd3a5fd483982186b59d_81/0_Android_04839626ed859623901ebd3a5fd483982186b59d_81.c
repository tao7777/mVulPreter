long Track::GetFirst(const BlockEntry*& pBlockEntry) const
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
 
