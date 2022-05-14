bool HpackDecoder::DecodeNextIndexedHeader(HpackInputStream* input_stream) {
  uint32 index = 0;
  if (!input_stream->DecodeNextUint32(&index))
    return false;

  HpackEntry* entry = header_table_.GetByIndex(index);
   if (entry == NULL)
     return false;
 
  return HandleHeaderRepresentation(entry->name(), entry->value());
 }
