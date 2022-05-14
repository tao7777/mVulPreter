 bool HpackDecoder::DecodeNextLiteralHeader(HpackInputStream* input_stream,
                                           bool should_index) {
  StringPiece name;
  if (!DecodeNextName(input_stream, &name))
    return false;

  StringPiece value;
   if (!DecodeNextStringLiteral(input_stream, false, &value))
     return false;
 
  HandleHeaderRepresentation(name, value);
 
   if (!should_index)
     return true;

  ignore_result(header_table_.TryAddEntry(name, value));
  return true;
}
