void Cues::PreloadCuePoint(
      continue;
     }
 
    assert(m_preload_count > 0);

    CuePoint* const pCP = m_cue_points[m_count];
    assert(pCP);
    assert((pCP->GetTimeCode() >= 0) || (-pCP->GetTimeCode() == idpos));
    if (pCP->GetTimeCode() < 0 && (-pCP->GetTimeCode() != idpos))
      return false;

    pCP->Load(pReader);
    ++m_count;
    --m_preload_count;

    m_pos += size;  // consume payload
    assert(m_pos <= stop);

    return true;  // yes, we loaded a cue point
  }

  // return (m_pos < stop);
  return false;  // no, we did not load a cue point
 }
