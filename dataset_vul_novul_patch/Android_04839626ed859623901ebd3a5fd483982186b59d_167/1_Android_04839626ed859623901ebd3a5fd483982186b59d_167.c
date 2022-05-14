long long EBMLHeader::Parse(
    IMkvReader* pReader,
    long long& pos)
{
    assert(pReader);
 
    long long total, available;
 
    long status = pReader->Length(&total, &available);
 
    if (status < 0)  //error
         return status;
 
    pos = 0;
    long long end = (available >= 1024) ? 1024 : available;
 
    for (;;)
    {
        unsigned char b = 0;
        while (pos < end)
        {
            status = pReader->Read(pos, 1, &b);
            if (status < 0)  //error
                return status;
            if (b == 0x1A)
                break;
            ++pos;
        }
        if (b != 0x1A)
        {
            if (pos >= 1024)
                return E_FILE_FORMAT_INVALID;  //don't bother looking anymore
            if ((total >= 0) && ((total - available) < 5))
                return E_FILE_FORMAT_INVALID;
            return available + 5;  //5 = 4-byte ID + 1st byte of size
        }
        if ((total >= 0) && ((total - pos) < 5))
            return E_FILE_FORMAT_INVALID;
        if ((available - pos) < 5)
            return pos + 5;  //try again later
        long len;
        const long long result = ReadUInt(pReader, pos, len);
        if (result < 0)  //error
            return result;
        if (result == 0x0A45DFA3)  //EBML Header ID
        {
            pos += len;  //consume ID
            break;
        }
        ++pos;  //throw away just the 0x1A byte, and try again
     }
 
 
 
     long len;
     long long result = GetUIntLength(pReader, pos, len);
 
    if (result < 0)  //error
        return result;
 
    if (result > 0)  //need more data
        return result;
 
    assert(len > 0);
    assert(len <= 8);
 
    if ((total >= 0) && ((total -  pos) < len))
        return E_FILE_FORMAT_INVALID;
 
    if ((available - pos) < len)
        return pos + len;  //try again later
 
 
    result = ReadUInt(pReader, pos, len);
 
    if (result < 0)  //error
        return result;
 
    pos += len;  //consume size field
 
 
    if ((total >= 0) && ((total - pos) < result))
        return E_FILE_FORMAT_INVALID;
 
    if ((available - pos) < result)
        return pos + result;
 
    end = pos + result;
 
    Init();
 
    while (pos < end)
    {
        long long id, size;
 
        status = ParseElementHeader(
                    pReader,
                    pos,
                    end,
                    id,
                    size);
 
        if (status < 0) //error
            return status;
 
        if (size == 0)  //weird
            return E_FILE_FORMAT_INVALID;
 
        if (id == 0x0286)  //version
        {
            m_version = UnserializeUInt(pReader, pos, size);
 
            if (m_version <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x02F7)  //read version
        {
            m_readVersion = UnserializeUInt(pReader, pos, size);
 
            if (m_readVersion <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x02F2)  //max id length
        {
            m_maxIdLength = UnserializeUInt(pReader, pos, size);
 
            if (m_maxIdLength <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x02F3)  //max size length
        {
            m_maxSizeLength = UnserializeUInt(pReader, pos, size);
            if (m_maxSizeLength <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x0282)  //doctype
        {
            if (m_docType)
                return E_FILE_FORMAT_INVALID;
            status = UnserializeString(pReader, pos, size, m_docType);
            if (status)  //error
                return status;
        }
        else if (id == 0x0287)  //doctype version
        {
            m_docTypeVersion = UnserializeUInt(pReader, pos, size);
            if (m_docTypeVersion <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        else if (id == 0x0285)  //doctype read version
        {
            m_docTypeReadVersion = UnserializeUInt(pReader, pos, size);
            if (m_docTypeReadVersion <= 0)
                return E_FILE_FORMAT_INVALID;
        }
        pos += size;
     }
 
    assert(pos == end);
    return 0;
 }
