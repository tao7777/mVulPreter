void DistillerNativeJavaScript::DistillerClosePanel() {
  EnsureServiceConnected();
  distiller_js_service_->HandleDistillerClosePanelCall();
}
