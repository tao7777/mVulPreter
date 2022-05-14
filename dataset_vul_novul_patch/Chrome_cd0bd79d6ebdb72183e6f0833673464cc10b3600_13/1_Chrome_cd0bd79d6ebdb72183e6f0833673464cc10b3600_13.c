 int WebGraphicsContext3DCommandBufferImpl::GetGPUProcessID() {
  return host_ ? host_->gpu_process_id() : 0;
 }
