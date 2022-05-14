bool Segment::DoneParsing() const
{
    if (m_size < 0)
    {
        long long total, avail;
 
        const int status = m_pReader->Length(&total, &avail);
 
        if (status < 0)  //error
            return true;  //must assume done
 
        if (total < 0)
            return false;  //assume live stream
 
        return (m_pos >= total);
    }
 
    const long long stop = m_start + m_size;
 
    return (m_pos >= stop);
 }
