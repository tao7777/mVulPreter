void Vp9Parser::SetStream(const uint8_t* stream, off_t stream_size) {
  DCHECK(stream);
  stream_ = stream;
  bytes_left_ = stream_size;
  frames_.clear();
}
