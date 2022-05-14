long Segment::DoLoadClusterUnknownSize(
    long long& pos,
    long& len)
{
    assert(m_pos < 0);
    assert(m_pUnknownSize);
 
 #if 0
     assert(m_pUnknownSize->GetElementSize() < 0);  //TODO: verify this

 const long long element_start = m_pUnknownSize->m_element_start;

    pos = -m_pos;
    assert(pos > element_start);


 long long total, avail;

 long status = m_pReader->Length(&total, &avail);

 if (status < 0) //error
 return status;

    assert((total < 0) || (avail <= total));

 const long long segment_stop = (m_size < 0) ? -1 : m_start + m_size;

 
     long long element_size = -1;
 
    for (;;)  //determine cluster size
    {
         if ((total >= 0) && (pos >= total))
         {
             element_size = total - element_start;
            assert(element_size > 0);

 break;
 }

 if ((segment_stop >= 0) && (pos >= segment_stop))
 {
            element_size = segment_stop - element_start;
            assert(element_size > 0);

 break;
 }


 if ((pos + 1) > avail)
 {
            len = 1;
 return E_BUFFER_NOT_FULL;
 }

 long long result = GetUIntLength(m_pReader, pos, len);

 if (result < 0) //error
 return static_cast<long>(result);

 if (result > 0) //weird
 return E_BUFFER_NOT_FULL;

 if ((segment_stop >= 0) && ((pos + len) > segment_stop))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long idpos = pos;
 const long long id = ReadUInt(m_pReader, idpos, len);

 if (id < 0) //error (or underflow)
 return static_cast<long>(id);


 
        if ((id == 0x0F43B675) || (id == 0x0C53BB6B)) //Cluster ID or Cues ID
        {
             element_size = pos - element_start;
             assert(element_size > 0);
 
 break;
 }

#ifdef _DEBUG
 switch (id)
 {
 case 0x20: //BlockGroup
 case 0x23: //Simple Block
 case 0x67: //TimeCode
 case 0x2B: //PrevSize
 break;

 default:
                assert(false);
 break;
 }
#endif

        pos += len; //consume ID (of sub-element)


 if ((pos + 1) > avail)
 {
            len = 1;
 return E_BUFFER_NOT_FULL;
 }

        result = GetUIntLength(m_pReader, pos, len);

 if (result < 0) //error
 return static_cast<long>(result);

 if (result > 0) //weird
 return E_BUFFER_NOT_FULL;

 if ((segment_stop >= 0) && ((pos + len) > segment_stop))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long size = ReadUInt(m_pReader, pos, len);

 if (size < 0) //error
 return static_cast<long>(size);

        pos += len; //consume size field of element


 if (size == 0) //weird
 continue;

 const long long unknown_size = (1LL << (7 * len)) - 1;

 if (size == unknown_size)
 return E_FILE_FORMAT_INVALID; //not allowed for sub-elements

 if ((segment_stop >= 0) && ((pos + size) > segment_stop)) //weird
 return E_FILE_FORMAT_INVALID;

        pos += size; //consume payload of sub-element
        assert((segment_stop < 0) || (pos <= segment_stop));
 } //determine cluster size

    assert(element_size >= 0);

    m_pos = element_start + element_size;
    m_pUnknownSize = 0;

 
     return 2;  //continue parsing
 #else
    const long status = m_pUnknownSize->Parse(pos, len);
 
    if (status < 0)  //error or underflow
        return status;
 
    if (status == 0)  //parsed a block
        return 2;     //continue parsing
 
    assert(status > 0);   //nothing left to parse of this cluster
 
    const long long start = m_pUnknownSize->m_element_start;
 
    const long long size = m_pUnknownSize->GetElementSize();
    assert(size >= 0);
 
    pos = start + size;
    m_pos = pos;
 
    m_pUnknownSize = 0;
 
    return 2;  //continue parsing
 #endif
 }
