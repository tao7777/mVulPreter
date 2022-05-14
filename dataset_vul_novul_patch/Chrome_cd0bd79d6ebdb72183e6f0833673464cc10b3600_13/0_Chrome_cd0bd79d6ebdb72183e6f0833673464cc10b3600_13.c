 int WebGraphicsContext3DCommandBufferImpl::GetGPUProcessID() {
  return host_ ? host_->gpu_host_id() : 0;
 }
