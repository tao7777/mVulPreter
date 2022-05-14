long Chapters::Edition::Parse(
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
 
        if (id == 0x36)  // Atom ID
        {
            status = ParseAtom(pReader, pos, size);
 
            if (status < 0)  // error
                return status;
        }
 
        pos += size;
        assert(pos <= stop);
     }
 
    assert(pos == stop);
    return 0;
 }
