long Cluster::ParseSimpleBlock(long long block_size, long long& pos,
 long& len) {
 const long long block_start = pos;
 const long long block_stop = pos + block_size;

 IMkvReader* const pReader = m_pSegment->m_pReader;

 long long total, avail;

 long status = pReader->Length(&total, &avail);

 if (status < 0) // error
 return status;

  assert((total < 0) || (avail <= total));


 if ((pos + 1) > avail) {
    len = 1;
 return E_BUFFER_NOT_FULL;
 }

 long long result = GetUIntLength(pReader, pos, len);

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

  status = CreateBlock(0x23, // simple block id
                       block_start, block_size,
 0); // DiscardPadding

 if (status != 0)
 return status;

  m_pos = block_stop;

 return 0; // success
}
