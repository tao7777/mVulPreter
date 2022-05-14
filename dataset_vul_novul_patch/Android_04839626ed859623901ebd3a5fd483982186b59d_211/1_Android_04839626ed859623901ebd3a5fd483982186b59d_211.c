long mkvparser::UnserializeString(
    IMkvReader* pReader,
    long long pos,
    long long size_,
    char*& str)
{
     delete[] str;
     str = NULL;
 
    if (size_ >= LONG_MAX)  //we need (size+1) chars
        return E_FILE_FORMAT_INVALID;
 
    const long size = static_cast<long>(size_);
 
    str = new (std::nothrow) char[size+1];
    if (str == NULL)
        return -1;
    unsigned char* const buf = reinterpret_cast<unsigned char*>(str);
    const long status = pReader->Read(pos, size, buf);
    if (status)
    {
        delete[] str;
        str = NULL;
        return status;
    }
    str[size] = '\0';
    return 0;  //success
 }
