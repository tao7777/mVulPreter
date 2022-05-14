DOMArrayBuffer* FileReaderLoader::ArrayBufferResult() {
  DCHECK_EQ(read_type_, kReadAsArrayBuffer);
  if (array_buffer_result_)
    return array_buffer_result_;

   if (!raw_data_ || error_code_ != FileErrorCode::kOK)
     return nullptr;
 
  if (!finished_loading_) {
    return DOMArrayBuffer::Create(
        ArrayBuffer::Create(raw_data_->Data(), raw_data_->ByteLength()));
   }

  array_buffer_result_ = DOMArrayBuffer::Create(raw_data_->ToArrayBuffer());
  AdjustReportedMemoryUsageToV8(-1 *
                                static_cast<int64_t>(raw_data_->ByteLength()));
  raw_data_.reset();
  return array_buffer_result_;
 }
