long Chapters::ParseEdition(
    long long pos,
    long long size)
{
    if (!ExpandEditionsArray())
        return -1;
 
    Edition& e = m_editions[m_editions_count++];
    e.Init();
    return e.Parse(m_pSegment->m_pReader, pos, size);
 }
