Vp9Parser::Result Vp9Parser::ParseNextFrame(Vp9FrameHeader* fhdr) {
bool Vp9Parser::ParseFrame(const uint8_t* stream,
                           size_t frame_size,
                           Vp9FrameHeader* fhdr) {
  DCHECK(stream);
  stream_ = stream;
  size_ = frame_size;
   memset(fhdr, 0, sizeof(*fhdr));
