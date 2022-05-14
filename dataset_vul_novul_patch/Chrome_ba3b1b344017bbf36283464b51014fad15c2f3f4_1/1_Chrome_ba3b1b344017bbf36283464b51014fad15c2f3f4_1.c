Page* ChromeClientImpl::CreateWindow(LocalFrame* frame,
                                     const FrameLoadRequest& r,
                                     const WebWindowFeatures& features,
                                     NavigationPolicy navigation_policy,
                                     SandboxFlags sandbox_flags) {
  if (!web_view_->Client())
    return nullptr;
 
   if (!frame->GetPage() || frame->GetPage()->Paused())
     return nullptr;
  DCHECK(frame->GetDocument());
  Fullscreen::FullyExitFullscreen(*frame->GetDocument());
 
   const AtomicString& frame_name =
       !EqualIgnoringASCIICase(r.FrameName(), "_blank") ? r.FrameName()
                                                       : g_empty_atom;
  WebViewImpl* new_view =
      static_cast<WebViewImpl*>(web_view_->Client()->CreateView(
          WebLocalFrameImpl::FromFrame(frame),
          WrappedResourceRequest(r.GetResourceRequest()), features, frame_name,
          static_cast<WebNavigationPolicy>(navigation_policy),
          r.GetShouldSetOpener() == kNeverSetOpener,
          static_cast<WebSandboxFlags>(sandbox_flags)));
  if (!new_view)
    return nullptr;
  return new_view->GetPage();
}
