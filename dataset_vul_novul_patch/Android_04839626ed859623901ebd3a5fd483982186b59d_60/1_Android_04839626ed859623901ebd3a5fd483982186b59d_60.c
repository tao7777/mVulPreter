long Cues::GetCount() const
{
    if (m_cue_points == NULL)
        return -1;
 
    return m_count;  //TODO: really ignore preload count?
}
