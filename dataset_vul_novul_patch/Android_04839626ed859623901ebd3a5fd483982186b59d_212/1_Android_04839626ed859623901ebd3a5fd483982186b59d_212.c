long long mkvparser::UnserializeUInt(
    IMkvReader* pReader,
    long long pos,
    long long size)
{
    assert(pReader);
    assert(pos >= 0);
 
    if ((size <= 0) || (size > 8))
        return E_FILE_FORMAT_INVALID;
 
    long long result = 0;
 
    for (long long i = 0; i < size; ++i)
    {
        unsigned char b;
 
        const long status = pReader->Read(pos, 1, &b);
 
        if (status < 0)
            return status;
 
        result <<= 8;
        result |= b;
 
        ++pos;
     }
 
    return result;
 }
