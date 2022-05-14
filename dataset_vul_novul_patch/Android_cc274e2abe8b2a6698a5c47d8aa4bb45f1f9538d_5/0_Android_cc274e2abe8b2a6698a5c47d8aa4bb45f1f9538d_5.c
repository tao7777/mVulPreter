 long long Segment::CreateInstance(IMkvReader* pReader, long long pos,
                                   Segment*& pSegment) {
  if (pReader == NULL || pos < 0)
    return E_PARSE_FAILED;
 
   pSegment = NULL;
 
 long long total, available;

 const long status = pReader->Length(&total, &available);

 if (status < 0) // error
 return status;

 if (available < 0)
 return -1;

 if ((total >= 0) && (available > total))
 return -1;


 for (;;) {
 if ((total >= 0) && (pos >= total))
 return E_FILE_FORMAT_INVALID;

 long len;
 long long result = GetUIntLength(pReader, pos, len);

 if (result) // error, or too few available bytes
 return result;

 if ((total >= 0) && ((pos + len) > total))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > available)

       return pos + len;
 
     const long long idpos = pos;
    const long long id = ReadID(pReader, pos, len);
 
    if (id < 0)
      return E_FILE_FORMAT_INVALID;
 
     pos += len;  // consume ID
 

    result = GetUIntLength(pReader, pos, len);

 if (result) // error, or too few available bytes
 return result;

 if ((total >= 0) && ((pos + len) > total))
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > available)
 return pos + len;

 long long size = ReadUInt(pReader, pos, len);

 if (size < 0) // error
 return size;

    pos += len; // consume length of size of element


 const long long unknown_size = (1LL << (7 * len)) - 1;

 if (id == 0x08538067) { // Segment ID
 if (size == unknown_size)
        size = -1;

 else if (total < 0)
        size = -1;

 else if ((pos + size) > total)
        size = -1;

      pSegment = new (std::nothrow) Segment(pReader, idpos,
                                            pos, size);

 if (pSegment == 0)
 return -1; // generic error

 return 0; // success
 }

 if (size == unknown_size)
 return E_FILE_FORMAT_INVALID;

 if ((total >= 0) && ((pos + size) > total))
 return E_FILE_FORMAT_INVALID;

 if ((pos + size) > available)
 return pos + size;

    pos += size; // consume payload
 }
}
