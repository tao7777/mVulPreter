PaymentRequest::PaymentRequest(
    content::RenderFrameHost* render_frame_host,
    content::WebContents* web_contents,
    std::unique_ptr<ContentPaymentRequestDelegate> delegate,
    PaymentRequestWebContentsManager* manager,
    PaymentRequestDisplayManager* display_manager,
     mojo::InterfaceRequest<mojom::PaymentRequest> request,
     ObserverForTest* observer_for_testing)
     : web_contents_(web_contents),
       delegate_(std::move(delegate)),
       manager_(manager),
       display_manager_(display_manager),
      display_handle_(nullptr),
      binding_(this, std::move(request)),
      top_level_origin_(url_formatter::FormatUrlForSecurityDisplay(
          web_contents_->GetLastCommittedURL())),
      frame_origin_(url_formatter::FormatUrlForSecurityDisplay(
          render_frame_host->GetLastCommittedURL())),
      observer_for_testing_(observer_for_testing),
      journey_logger_(delegate_->IsIncognito(),
                      ukm::GetSourceIdForWebContentsDocument(web_contents)),
      weak_ptr_factory_(this) {
  binding_.set_connection_error_handler(base::BindOnce(
      &PaymentRequest::OnConnectionTerminated, weak_ptr_factory_.GetWeakPtr()));
}
