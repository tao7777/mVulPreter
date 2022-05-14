void ImageLoader::DoUpdateFromElement(BypassMainWorldBehavior bypass_behavior,
                                      UpdateFromElementBehavior update_behavior,
                                      const KURL& url,
                                      ReferrerPolicy referrer_policy,
                                      UpdateType update_type) {
  pending_task_.reset();
  std::unique_ptr<IncrementLoadEventDelayCount> load_delay_counter;
  load_delay_counter.swap(delay_until_do_update_from_element_);

  Document& document = element_->GetDocument();
  if (!document.IsActive())
    return;

  AtomicString image_source_url = element_->ImageSourceURL();
  ImageResourceContent* new_image_content = nullptr;
  if (!url.IsNull() && !url.IsEmpty()) {
    ResourceLoaderOptions resource_loader_options;
    resource_loader_options.initiator_info.name = GetElement()->localName();
    ResourceRequest resource_request(url);
    if (update_behavior == kUpdateForcedReload) {
      resource_request.SetCacheMode(mojom::FetchCacheMode::kBypassCache);
      resource_request.SetPreviewsState(WebURLRequest::kPreviewsNoTransform);
    }

    if (referrer_policy != kReferrerPolicyDefault) {
      resource_request.SetHTTPReferrer(SecurityPolicy::GenerateReferrer(
           referrer_policy, url, document.OutgoingReferrer()));
     }
 
    // Correct the RequestContext if necessary.
     if (IsHTMLPictureElement(GetElement()->parentNode()) ||
        !GetElement()->FastGetAttribute(HTMLNames::srcsetAttr).IsNull()) {
       resource_request.SetRequestContext(
           WebURLRequest::kRequestContextImageSet);
    } else if (IsHTMLObjectElement(GetElement())) {
      resource_request.SetRequestContext(WebURLRequest::kRequestContextObject);
    } else if (IsHTMLEmbedElement(GetElement())) {
      resource_request.SetRequestContext(WebURLRequest::kRequestContextEmbed);
    }
 
     bool page_is_being_dismissed =
         document.PageDismissalEventBeingDispatched() != Document::kNoDismissal;
    if (page_is_being_dismissed) {
      resource_request.SetHTTPHeaderField(HTTPNames::Cache_Control,
                                          "max-age=0");
      resource_request.SetKeepalive(true);
      resource_request.SetRequestContext(WebURLRequest::kRequestContextPing);
    }

    FetchParameters params(resource_request, resource_loader_options);
    ConfigureRequest(params, bypass_behavior, *element_,
                     document.GetClientHintsPreferences());

    if (update_behavior != kUpdateForcedReload && document.GetFrame())
      document.GetFrame()->MaybeAllowImagePlaceholder(params);

    new_image_content = ImageResourceContent::Fetch(params, document.Fetcher());

    if (page_is_being_dismissed)
      new_image_content = nullptr;

    ClearFailedLoadURL();
  } else {
    if (!image_source_url.IsNull()) {
      DispatchErrorEvent();
    }
    NoImageResourceToLoad();
  }

  ImageResourceContent* old_image_content = image_content_.Get();
  if (old_image_content != new_image_content)
    RejectPendingDecodes(update_type);

  if (update_behavior == kUpdateSizeChanged && element_->GetLayoutObject() &&
      element_->GetLayoutObject()->IsImage() &&
      new_image_content == old_image_content) {
    ToLayoutImage(element_->GetLayoutObject())->IntrinsicSizeChanged();
  } else {
    if (pending_load_event_.IsActive())
      pending_load_event_.Cancel();

    if (pending_error_event_.IsActive() && new_image_content)
      pending_error_event_.Cancel();

    UpdateImageState(new_image_content);

    UpdateLayoutObject();
    if (new_image_content) {
      new_image_content->AddObserver(this);
    }
    if (old_image_content) {
      old_image_content->RemoveObserver(this);
    }
  }

  if (LayoutImageResource* image_resource = GetLayoutImageResource())
    image_resource->ResetAnimation();
}
