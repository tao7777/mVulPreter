 void ServiceWorkerScriptContext::OnInstallEvent(
     int active_version_embedded_worker_id) {
  proxy_->dispatchInstallEvent(current_request_id_);
 }
