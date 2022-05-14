long Cluster::ParseBlockGroup(long long payload_size, long long& pos,
 long& len) {
 const long long payload_start = pos;
 const long long payload_stop = pos + payload_size;

 IMkvReader* const pReader = m_pSegment->m_pReader;

 long long total, avail;

 long status = pReader->Length(&total, &avail);

 if (status < 0) // error
 return status;

  assert((total < 0) || (avail <= total));

 if ((total >= 0) && (payload_stop > total))
 return E_FILE_FORMAT_INVALID;

 if (payload_stop > avail) {
    len = static_cast<long>(payload_size);
 return E_BUFFER_NOT_FULL;
 }

 long long discard_padding = 0;

 while (pos < payload_stop) {

 if ((pos + 1) > avail) {
      len = 1;
 return E_BUFFER_NOT_FULL;
 }

 long long result = GetUIntLength(pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((pos + len) > payload_stop)
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long id = ReadUInt(pReader, pos, len);

 if (id < 0) // error
 return static_cast<long>(id);

 if (id == 0) // not a value ID
 return E_FILE_FORMAT_INVALID;

    pos += len; // consume ID field


 if ((pos + 1) > avail) {
      len = 1;
 return E_BUFFER_NOT_FULL;
 }

    result = GetUIntLength(pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((pos + len) > payload_stop)
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long size = ReadUInt(pReader, pos, len);

 if (size < 0) // error
 return static_cast<long>(size);

    pos += len; // consume size field


 if (pos > payload_stop)
 return E_FILE_FORMAT_INVALID;

 if (size == 0) // weird
 continue;

 const long long unknown_size = (1LL << (7 * len)) - 1;

 if (size == unknown_size)
 return E_FILE_FORMAT_INVALID;

 if (id == 0x35A2) { // DiscardPadding
      status = UnserializeInt(pReader, pos, size, discard_padding);

 if (status < 0) // error
 return status;
 }

 if (id != 0x21) { // sub-part of BlockGroup is not a Block
      pos += size; // consume sub-part of block group

 if (pos > payload_stop)
 return E_FILE_FORMAT_INVALID;

 continue;
 }

 const long long block_stop = pos + size;

 if (block_stop > payload_stop)
 return E_FILE_FORMAT_INVALID;


 if ((pos + 1) > avail) {
      len = 1;
 return E_BUFFER_NOT_FULL;
 }

    result = GetUIntLength(pReader, pos, len);

 if (result < 0) // error
 return static_cast<long>(result);

 if (result > 0) // weird
 return E_BUFFER_NOT_FULL;

 if ((pos + len) > block_stop)
 return E_FILE_FORMAT_INVALID;

 if ((pos + len) > avail)
 return E_BUFFER_NOT_FULL;

 const long long track = ReadUInt(pReader, pos, len);

 if (track < 0) // error
 return static_cast<long>(track);


     if (track == 0)
       return E_FILE_FORMAT_INVALID;
 
     pos += len;  // consume track number
 
     if ((pos + 2) > block_stop)
 return E_FILE_FORMAT_INVALID;

 if ((pos + 2) > avail) {
      len = 2;
 return E_BUFFER_NOT_FULL;
 }

    pos += 2; // consume timecode

 if ((pos + 1) > block_stop)
 return E_FILE_FORMAT_INVALID;

 if ((pos + 1) > avail) {
      len = 1;
 return E_BUFFER_NOT_FULL;
 }

 unsigned char flags;

    status = pReader->Read(pos, 1, &flags);

 if (status < 0) { // error or underflow
      len = 1;
 return status;
 }

 ++pos; // consume flags byte
    assert(pos <= avail);

 if (pos >= block_stop)
 return E_FILE_FORMAT_INVALID;

 const int lacing = int(flags & 0x06) >> 1;

 if ((lacing != 0) && (block_stop > avail)) {
      len = static_cast<long>(block_stop - pos);
 return E_BUFFER_NOT_FULL;

     }
 
     pos = block_stop;  // consume block-part of block group
    if (pos > payload_stop)
      return E_FILE_FORMAT_INVALID;
   }
 
  if (pos != payload_stop)
    return E_FILE_FORMAT_INVALID;
 
   status = CreateBlock(0x20,  // BlockGroup ID
                        payload_start, payload_size, discard_padding);
 if (status != 0)
 return status;

  m_pos = payload_stop;

 return 0; // success
}
