void DomDistillerViewerSource::StartDataRequest(
    const std::string& path,
    int render_process_id,
    int render_frame_id,
    const content::URLDataSource::GotDataCallback& callback) {
  content::RenderFrameHost* render_frame_host =
      content::RenderFrameHost::FromID(render_process_id, render_frame_id);
  if (!render_frame_host)
    return;
  content::RenderViewHost* render_view_host =
      render_frame_host->GetRenderViewHost();
  DCHECK(render_view_host);
  CHECK_EQ(0, render_view_host->GetEnabledBindings());

  if (kViewerCssPath == path) {
    std::string css = viewer::GetCss();
    callback.Run(base::RefCountedString::TakeString(&css));
    return;
  } else if (base::StartsWith(path, kViewerSaveFontScalingPath,
                              base::CompareCase::SENSITIVE)) {
    double scale = 1.0;
    if (base::StringToDouble(
        path.substr(strlen(kViewerSaveFontScalingPath)), &scale)) {
      dom_distiller_service_->GetDistilledPagePrefs()->SetFontScaling(scale);
    }
  }
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(render_frame_host);
  DCHECK(web_contents);
  const std::string path_after_query_separator =
      path.size() > 0 ? path.substr(1) : "";
  RequestViewerHandle* request_viewer_handle =
      new RequestViewerHandle(web_contents, scheme_, path_after_query_separator,
                              dom_distiller_service_->GetDistilledPagePrefs());
  scoped_ptr<ViewerHandle> viewer_handle = viewer::CreateViewRequest(
      dom_distiller_service_, path, request_viewer_handle,
      web_contents->GetContainerBounds().size());

  GURL current_url(url_utils::GetValueForKeyInUrlPathQuery(path, kUrlKey));
  std::string unsafe_page_html = viewer::GetUnsafeArticleTemplateHtml(
      url_utils::GetOriginalUrlFromDistillerUrl(current_url).spec(),
      dom_distiller_service_->GetDistilledPagePrefs()->GetTheme(),
      dom_distiller_service_->GetDistilledPagePrefs()->GetFontFamily());

   render_frame_host->GetServiceRegistry()->AddService(
       base::Bind(&CreateDistillerJavaScriptService,
           render_frame_host,
          distiller_ui_handle_.get()));
 
   DistillerPageNotifierServicePtr page_notifier_service;
  render_frame_host->GetServiceRegistry()->ConnectToRemoteService(
      mojo::GetProxy(&page_notifier_service));
  DCHECK(page_notifier_service);
  page_notifier_service->NotifyIsDistillerPage();

  if (viewer_handle) {
    request_viewer_handle->TakeViewerHandle(viewer_handle.Pass());
  } else {
    request_viewer_handle->FlagAsErrorPage();
  }

  callback.Run(base::RefCountedString::TakeString(&unsafe_page_html));
};
