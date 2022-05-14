 void DistillerNativeJavaScript::EnsureServiceConnected() {
  if (!distiller_js_service_ || !distiller_js_service_.is_bound()) {
     render_frame_->GetServiceRegistry()->ConnectToRemoteService(
         mojo::GetProxy(&distiller_js_service_));
   }
 }
