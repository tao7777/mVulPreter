const Cluster* Segment::GetNext(const Cluster* pCurr) {
  assert(pCurr);
  assert(pCurr != &m_eos);
  assert(m_clusters);

 long idx = pCurr->m_index;

 if (idx >= 0) {
    assert(m_clusterCount > 0);
    assert(idx < m_clusterCount);
    assert(pCurr == m_clusters[idx]);

 ++idx;

 if (idx >= m_clusterCount)
 return &m_eos; // caller will LoadCluster as desired

 Cluster* const pNext = m_clusters[idx];
    assert(pNext);
    assert(pNext->m_index >= 0);
    assert(pNext->m_index == idx);

 return pNext;
 }

  assert(m_clusterPreloadCount > 0);

 long long pos = pCurr->m_element_start;

  assert(m_size >= 0); // TODO
 const long long stop = m_start + m_size; // end of segment

 {
 long len;

 long long result = GetUIntLength(m_pReader, pos, len);
    assert(result == 0);
    assert((pos + len) <= stop); // TODO

     if (result != 0)
       return NULL;
 
    const long long id = ReadUInt(m_pReader, pos, len);
    assert(id == 0x0F43B675);  // Cluster ID
    if (id != 0x0F43B675)
       return NULL;
 
     pos += len;  // consume ID

    result = GetUIntLength(m_pReader, pos, len);
    assert(result == 0); // TODO
    assert((pos + len) <= stop); // TODO

 const long long size = ReadUInt(m_pReader, pos, len);
    assert(size > 0); // TODO

    pos += len; // consume length of size of element
    assert((pos + size) <= stop); // TODO


    pos += size; // consume payload
 }

 long long off_next = 0;

 while (pos < stop) {
 long len;

 long long result = GetUIntLength(m_pReader, pos, len);
    assert(result == 0);
    assert((pos + len) <= stop); // TODO
 if (result != 0)
 return NULL;

 
     const long long idpos = pos;  // pos of next (potential) cluster
 
    const long long id = ReadUInt(m_pReader, idpos, len);
    assert(id > 0);  // TODO
 
     pos += len;  // consume ID
 
    result = GetUIntLength(m_pReader, pos, len);
    assert(result == 0); // TODO
    assert((pos + len) <= stop); // TODO

 const long long size = ReadUInt(m_pReader, pos, len);
    assert(size >= 0); // TODO

    pos += len; // consume length of size of element
    assert((pos + size) <= stop); // TODO


 if (size == 0) // weird
 continue;

 if (id == 0x0F43B675) { // Cluster ID
 const long long off_next_ = idpos - m_start;

 long long pos_;
 long len_;

 const long status = Cluster::HasBlockEntries(this, off_next_, pos_, len_);

      assert(status >= 0);

 if (status > 0) {
        off_next = off_next_;
 break;
 }
 }

    pos += size; // consume payload
 }

 if (off_next <= 0)
 return 0;

 Cluster** const ii = m_clusters + m_clusterCount;
 Cluster** i = ii;

 Cluster** const jj = ii + m_clusterPreloadCount;
 Cluster** j = jj;

 while (i < j) {

 Cluster** const k = i + (j - i) / 2;
    assert(k < jj);

 Cluster* const pNext = *k;
    assert(pNext);
    assert(pNext->m_index < 0);


    pos = pNext->GetPosition();

 if (pos < off_next)
      i = k + 1;
 else if (pos > off_next)
      j = k;
 else
 return pNext;
 }


   assert(i == j);
 
   Cluster* const pNext = Cluster::Create(this, -1, off_next);
  assert(pNext);
 
   const ptrdiff_t idx_next = i - m_clusters;  // insertion position
 
  PreloadCluster(pNext, idx_next);
   assert(m_clusters);
   assert(idx_next < m_clusterSize);
   assert(m_clusters[idx_next] == pNext);

 return pNext;
}
