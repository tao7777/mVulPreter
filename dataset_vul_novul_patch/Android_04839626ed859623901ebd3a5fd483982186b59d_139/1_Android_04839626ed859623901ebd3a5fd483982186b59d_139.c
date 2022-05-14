long long mkvparser::GetUIntLength(
    IMkvReader* pReader,
    long long pos,
    long& len)
{
    assert(pReader);
    assert(pos >= 0);
 
    long long total, available;
 
    int status = pReader->Length(&total, &available);
    assert(status >= 0);
    assert((total < 0) || (available <= total));
 
    len = 1;
 
    if (pos >= available)
        return pos;  //too few bytes available
 
//// TODO(vigneshv): This function assumes that unsigned values never have their
//// high bit set.
     unsigned char b;
 
    status = pReader->Read(pos, 1, &b);
 
     if (status < 0)
        return status;
 
    assert(status == 0);
 
    if (b == 0)  //we can't handle u-int values larger than 8 bytes
        return E_FILE_FORMAT_INVALID;
 
    unsigned char m = 0x80;
    while (!(b & m))
    {
        m >>= 1;
        ++len;
    }
    return 0;  //success
 }
