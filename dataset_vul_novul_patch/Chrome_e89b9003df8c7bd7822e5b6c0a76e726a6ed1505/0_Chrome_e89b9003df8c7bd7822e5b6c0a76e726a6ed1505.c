void MimeHandlerViewContainer::OnReady() {
  if (!render_frame() || !is_embedded_)
    return;

  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();

  blink::WebAssociatedURLLoaderOptions options;
  DCHECK(!loader_);
  loader_.reset(frame->CreateAssociatedURLLoader(options));

   blink::WebURLRequest request(original_url_);
   request.SetRequestContext(blink::WebURLRequest::kRequestContextObject);
  // The plugin resource request should skip service workers since "plug-ins
  // may get their security origins from their own urls".
  // https://w3c.github.io/ServiceWorker/#implementer-concerns
  request.SetServiceWorkerMode(blink::WebURLRequest::ServiceWorkerMode::kNone);
   loader_->LoadAsynchronously(request, this);
 }
