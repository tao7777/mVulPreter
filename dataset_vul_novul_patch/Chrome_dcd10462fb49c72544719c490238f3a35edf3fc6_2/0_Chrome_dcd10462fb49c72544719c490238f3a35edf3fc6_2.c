 void CreateDistillerJavaScriptService(
     content::RenderFrameHost* render_frame_host,
    DistillerUIHandle* distiller_ui_handle,
     mojo::InterfaceRequest<DistillerJavaScriptService> request) {
  new DistillerJavaScriptServiceImpl(render_frame_host, distiller_ui_handle,
       request.Pass());
 }
