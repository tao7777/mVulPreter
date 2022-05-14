double AudioTrack::GetSamplingRate() const
  while (i != j) {
    Track* const pTrack = *i++;
    delete pTrack;
  }

  delete[] m_trackEntries;
 }
