const Cluster* Segment::FindOrPreloadCluster(long long requested_pos)
  Cluster** const ii = m_clusters;
  Cluster** i = ii;
 
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
 
    if (pos < requested_pos)
      i = k + 1;
    else if (pos > requested_pos)
      j = k;
    else
      return pCluster;
  }

  assert(i == j);
  // assert(Cluster::HasBlockEntries(this, tp.m_pos));

  Cluster* const pCluster = Cluster::Create(this, -1, requested_pos);
  //-1);
  assert(pCluster);

  const ptrdiff_t idx = i - m_clusters;

  PreloadCluster(pCluster, idx);
  assert(m_clusters);
  assert(m_clusterPreloadCount > 0);
  assert(m_clusters[idx] == pCluster);

  return pCluster;
 }
