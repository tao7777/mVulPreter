long mkvparser::ParseElementHeader(
    IMkvReader* pReader,
    long long& pos,
    long long stop,
    long long& id,
    long long& size)
{
    if ((stop >= 0) && (pos >= stop))
        return E_FILE_FORMAT_INVALID;
 
    long len;
 
    id = ReadUInt(pReader, pos, len);
 
    if (id < 0)
        return E_FILE_FORMAT_INVALID;
 
    pos += len;  //consume id
 
    if ((stop >= 0) && (pos >= stop))
        return E_FILE_FORMAT_INVALID;
 
    size = ReadUInt(pReader, pos, len);
 
    if (size < 0)
        return E_FILE_FORMAT_INVALID;
 
    pos += len;  //consume length of size
 
 
    if ((stop >= 0) && ((pos + size) > stop))
        return E_FILE_FORMAT_INVALID;
    return 0;  //success
 }
