void MimeHandlerViewContainer::OnReady() {
  if (!render_frame() || !is_embedded_)
    return;

  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();

  blink::WebAssociatedURLLoaderOptions options;
  DCHECK(!loader_);
  loader_.reset(frame->CreateAssociatedURLLoader(options));

   blink::WebURLRequest request(original_url_);
   request.SetRequestContext(blink::WebURLRequest::kRequestContextObject);
   loader_->LoadAsynchronously(request, this);
 }
