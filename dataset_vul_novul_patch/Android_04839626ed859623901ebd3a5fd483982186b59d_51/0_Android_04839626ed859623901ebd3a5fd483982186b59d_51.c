long long AudioTrack::GetChannels() const
  Track** i = m_trackEntries;
  Track** const j = m_trackEntriesEnd;

  while (i != j) {
    Track* const pTrack = *i++;

    if (pTrack == NULL)
      continue;

    if (tn == pTrack->GetNumber())
      return pTrack;
  }

  return NULL;  // not found
 }
