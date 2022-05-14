void BrowserViewRenderer::DidDestroyCompositor(
     content::SynchronousCompositor* compositor) {
   TRACE_EVENT0("android_webview", "BrowserViewRenderer::DidDestroyCompositor");
   DCHECK(compositor_);
   compositor_ = NULL;
 }
