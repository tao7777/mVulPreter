long long Block::GetTimeCode(const Cluster* pCluster) const
  const long long tc0 = pCluster->GetTimeCode();
  assert(tc0 >= 0);
 
  const long long tc = tc0 + m_timecode;
 
  return tc;  // unscaled timecode units
 }
