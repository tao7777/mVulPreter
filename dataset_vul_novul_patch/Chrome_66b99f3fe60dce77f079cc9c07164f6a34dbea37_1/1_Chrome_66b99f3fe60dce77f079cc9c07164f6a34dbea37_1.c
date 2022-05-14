void PepperRendererConnection::OnMsgDidCreateInProcessInstance(
     PP_Instance instance,
     const PepperRendererInstanceData& instance_data) {
   PepperRendererInstanceData data = instance_data;
   data.render_process_id = render_process_id_;
   in_process_host_->AddInstance(instance, data);
 }
