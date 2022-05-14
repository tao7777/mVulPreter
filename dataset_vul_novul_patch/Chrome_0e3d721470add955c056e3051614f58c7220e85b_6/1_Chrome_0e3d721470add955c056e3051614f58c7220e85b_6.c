 SynchronousCompositorImpl* SynchronousCompositorImpl::FromID(int process_id,
                                                              int routing_id) {
  if (g_factory == NULL)
    return NULL;
   RenderViewHost* rvh = RenderViewHost::FromID(process_id, routing_id);
   if (!rvh)
    return NULL;
   WebContents* contents = WebContents::FromRenderViewHost(rvh);
   if (!contents)
    return NULL;
   return FromWebContents(contents);
 }
