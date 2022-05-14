bool SeekHead::ParseEntry(
bool SeekHead::ParseEntry(IMkvReader* pReader, long long start, long long size_,
                          Entry* pEntry) {
  if (size_ <= 0)
    return false;
 
  long long pos = start;
  const long long stop = start + size_;
 
  long len;
 
  // parse the container for the level-1 element ID
 
  const long long seekIdId = ReadUInt(pReader, pos, len);
  // seekIdId;
 
  if (seekIdId != 0x13AB)  // SeekID ID
    return false;
 
  if ((pos + len) > stop)
    return false;
 
  pos += len;  // consume SeekID id
 
  const long long seekIdSize = ReadUInt(pReader, pos, len);
 
  if (seekIdSize <= 0)
    return false;
 
  if ((pos + len) > stop)
    return false;
 
  pos += len;  // consume size of field
 
  if ((pos + seekIdSize) > stop)
    return false;
 
  // Note that the SeekId payload really is serialized
  // as a "Matroska integer", not as a plain binary value.
  // In fact, Matroska requires that ID values in the
  // stream exactly match the binary representation as listed
  // in the Matroska specification.
  //
  // This parser is more liberal, and permits IDs to have
  // any width.  (This could make the representation in the stream
  // different from what's in the spec, but it doesn't matter here,
  // since we always normalize "Matroska integer" values.)
 
  pEntry->id = ReadUInt(pReader, pos, len);  // payload
 
  if (pEntry->id <= 0)
    return false;
 
  if (len != seekIdSize)
    return false;
 
  pos += seekIdSize;  // consume SeekID payload
 
  const long long seekPosId = ReadUInt(pReader, pos, len);
 
  if (seekPosId != 0x13AC)  // SeekPos ID
    return false;
 
  if ((pos + len) > stop)
    return false;
 
  pos += len;  // consume id
 
  const long long seekPosSize = ReadUInt(pReader, pos, len);
 
  if (seekPosSize <= 0)
    return false;
 
  if ((pos + len) > stop)
    return false;
 
  pos += len;  // consume size
 
  if ((pos + seekPosSize) > stop)
    return false;
 
  pEntry->pos = UnserializeUInt(pReader, pos, seekPosSize);
 
  if (pEntry->pos < 0)
    return false;
 
  pos += seekPosSize;  // consume payload
 
  if (pos != stop)
    return false;
 
  return true;
 }
