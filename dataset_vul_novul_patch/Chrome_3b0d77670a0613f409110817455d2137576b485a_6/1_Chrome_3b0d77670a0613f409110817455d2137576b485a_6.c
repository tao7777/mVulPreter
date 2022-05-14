int NaClIPCAdapter::TakeClientFileDescriptor() {
  return io_thread_data_.channel_->TakeClientFileDescriptor();
}
