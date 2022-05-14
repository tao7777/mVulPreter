 void PepperRendererConnection::OnMsgDidDeleteInProcessInstance(
     PP_Instance instance) {
   in_process_host_->DeleteInstance(instance);
 }
