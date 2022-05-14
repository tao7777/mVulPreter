void DistillerNativeJavaScript::DistillerSendFeedback(bool good) {
  EnsureServiceConnected();
  distiller_js_service_->HandleDistillerFeedbackCall(good);
}
