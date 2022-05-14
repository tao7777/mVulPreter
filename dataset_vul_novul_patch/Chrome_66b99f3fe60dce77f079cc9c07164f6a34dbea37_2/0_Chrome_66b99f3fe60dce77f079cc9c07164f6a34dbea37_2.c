 void PepperRendererConnection::OnMsgDidDeleteInProcessInstance(
     PP_Instance instance) {
  // 'instance' is possibly invalid. The host must be careful not to trust it.
   in_process_host_->DeleteInstance(instance);
 }
