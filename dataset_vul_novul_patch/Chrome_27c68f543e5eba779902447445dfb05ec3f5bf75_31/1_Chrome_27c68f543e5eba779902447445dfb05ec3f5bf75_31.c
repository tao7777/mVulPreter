bool Vp9ParserTest::ParseNextFrame(Vp9FrameHeader* fhdr) {
  while (1) {
    Vp9Parser::Result res = vp9_parser_.ParseNextFrame(fhdr);
    if (res == Vp9Parser::kEOStream) {
      IvfFrameHeader ivf_frame_header;
      const uint8_t* ivf_payload;
      if (!ivf_parser_.ParseNextFrame(&ivf_frame_header, &ivf_payload))
        return false;
      vp9_parser_.SetStream(ivf_payload, ivf_frame_header.frame_size);
      continue;
    }
    return res == Vp9Parser::kOk;
  }
}
