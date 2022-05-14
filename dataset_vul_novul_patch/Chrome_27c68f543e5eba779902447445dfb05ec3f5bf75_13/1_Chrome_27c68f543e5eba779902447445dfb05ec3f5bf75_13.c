Vp9Parser::Result Vp9Parser::ParseNextFrame(Vp9FrameHeader* fhdr) {
  if (frames_.empty()) {
    if (!stream_)
      return kEOStream;
    if (!ParseSuperframe()) {
      DVLOG(1) << "Failed parsing superframes";
      return kInvalidStream;
    }
  }
  DCHECK(!frames_.empty());
  FrameInfo frame_info = frames_.front();
  frames_.pop_front();
   memset(fhdr, 0, sizeof(*fhdr));
  if (!ParseUncompressedHeader(frame_info.ptr, frame_info.size, fhdr))
    return kInvalidStream;
  return kOk;
}
