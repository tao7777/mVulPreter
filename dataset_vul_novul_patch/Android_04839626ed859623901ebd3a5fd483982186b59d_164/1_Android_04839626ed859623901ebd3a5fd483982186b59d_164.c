long Chapters::Atom::Parse(
    IMkvReader* pReader,
    long long pos,
    long long size)
{
    const long long stop = pos + size;
    while (pos < stop)
    {
        long long id, size;
        long status = ParseElementHeader(
                        pReader,
                        pos,
                        stop,
                        id,
                        size);
        if (status < 0)  // error
            return status;
        if (size == 0)  // weird
            continue;
        if (id == 0x00)  // Display ID
        {
            status = ParseDisplay(pReader, pos, size);
            if (status < 0)  // error
                return status;
        }
        else if (id == 0x1654)  // StringUID ID
        {
            status = UnserializeString(pReader, pos, size, m_string_uid);
            if (status < 0)  // error
                return status;
        }
        else if (id == 0x33C4)  // UID ID
        {
            long long val;
            status = UnserializeInt(pReader, pos, size, val);
            if (status < 0)  // error
                return status;
            m_uid = val;
        }
        else if (id == 0x11)  // TimeStart ID
        {
            const long long val = UnserializeUInt(pReader, pos, size);
            if (val < 0)  // error
                return static_cast<long>(val);
            m_start_timecode = val;
        }
        else if (id == 0x12)  // TimeEnd ID
        {
            const long long val = UnserializeUInt(pReader, pos, size);
            if (val < 0)  // error
                return static_cast<long>(val);
            m_stop_timecode = val;
        }
        pos += size;
        assert(pos <= stop);
    }
    assert(pos == stop);
    return 0;
}
