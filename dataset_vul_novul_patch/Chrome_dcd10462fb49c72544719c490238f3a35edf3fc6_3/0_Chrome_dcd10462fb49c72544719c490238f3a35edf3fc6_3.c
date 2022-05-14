 DistillerJavaScriptServiceImpl::DistillerJavaScriptServiceImpl(
     content::RenderFrameHost* render_frame_host,
    DistillerUIHandle* distiller_ui_handle,
     mojo::InterfaceRequest<DistillerJavaScriptService> request)
     : binding_(this, request.Pass()),
       render_frame_host_(render_frame_host),
