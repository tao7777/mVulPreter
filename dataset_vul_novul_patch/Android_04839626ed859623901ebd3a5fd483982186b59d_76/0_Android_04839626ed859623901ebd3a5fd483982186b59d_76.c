long Cluster::GetEntry(long index, const mkvparser::BlockEntry*& pEntry) const
    if ((pos + 1) > avail) {
      len = 1;
      return E_BUFFER_NOT_FULL;
     }
 
    unsigned char flags;
 
    status = pReader->Read(pos, 1, &flags);
 
