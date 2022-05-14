long Chapters::Display::Parse(
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
        if (id == 0x05)  // ChapterString ID
        {
            status = UnserializeString(pReader, pos, size, m_string);
            if (status)
              return status;
        }
        else if (id == 0x037C)  // ChapterLanguage ID
        {
            status = UnserializeString(pReader, pos, size, m_language);
            if (status)
              return status;
        }
        else if (id == 0x037E)  // ChapterCountry ID
        {
            status = UnserializeString(pReader, pos, size, m_country);
            if (status)
              return status;
        }
        pos += size;
        assert(pos <= stop);
    }
    assert(pos == stop);
    return 0;
}
