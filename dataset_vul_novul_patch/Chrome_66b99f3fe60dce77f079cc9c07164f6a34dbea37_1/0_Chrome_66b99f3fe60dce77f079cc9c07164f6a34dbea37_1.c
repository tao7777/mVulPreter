void PepperRendererConnection::OnMsgDidCreateInProcessInstance(
     PP_Instance instance,
     const PepperRendererInstanceData& instance_data) {
   PepperRendererInstanceData data = instance_data;
  // It's important that we supply the render process ID ourselves since the
  // message may be coming from a compromised renderer.
   data.render_process_id = render_process_id_;
  // 'instance' is possibly invalid. The host must be careful not to trust it.
   in_process_host_->AddInstance(instance, data);
 }
