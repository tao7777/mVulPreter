const BlockEntry* Segment::GetBlock(
  const long count = m_clusterCount + m_clusterPreloadCount;
 
  Cluster** const jj = ii + count;
  Cluster** j = jj;
 
  while (i < j) {
    // INVARIANT:
    //[ii, i) < pTP->m_pos
    //[i, j) ?
    //[j, jj)  > pTP->m_pos
 
    Cluster** const k = i + (j - i) / 2;
    assert(k < jj);
 
    Cluster* const pCluster = *k;
     assert(pCluster);
 
    // const long long pos_ = pCluster->m_pos;
    // assert(pos_);
    // const long long pos = pos_ * ((pos_ < 0) ? -1 : 1);
 
    const long long pos = pCluster->GetPosition();
    assert(pos >= 0);
 
    if (pos < tp.m_pos)
      i = k + 1;
    else if (pos > tp.m_pos)
      j = k;
    else
      return pCluster->GetEntry(cp, tp);
  }

  assert(i == j);
  // assert(Cluster::HasBlockEntries(this, tp.m_pos));

  Cluster* const pCluster = Cluster::Create(this, -1, tp.m_pos);  //, -1);
  assert(pCluster);

  const ptrdiff_t idx = i - m_clusters;

  PreloadCluster(pCluster, idx);
  assert(m_clusters);
  assert(m_clusterPreloadCount > 0);
  assert(m_clusters[idx] == pCluster);

  return pCluster->GetEntry(cp, tp);
 }
