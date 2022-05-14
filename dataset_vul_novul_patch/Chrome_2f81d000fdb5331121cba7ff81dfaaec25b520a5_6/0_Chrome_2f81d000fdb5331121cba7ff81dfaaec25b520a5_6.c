void ResourceDispatcherHostImpl::InitializeURLRequest(
    net::URLRequest* request,
    const Referrer& referrer,
    bool is_download,
     int render_process_host_id,
     int render_view_routing_id,
     int render_frame_routing_id,
    int frame_tree_node_id,
     PreviewsState previews_state,
     ResourceContext* context) {
   DCHECK(io_thread_task_runner_->BelongsToCurrentThread());
  DCHECK(!request->is_pending());

  Referrer::SetReferrerForRequest(request, referrer);
 
   ResourceRequestInfoImpl* info = CreateRequestInfo(
       render_process_host_id, render_view_routing_id, render_frame_routing_id,
      frame_tree_node_id, previews_state, is_download, context);
   info->AssociateWithRequest(request);
 }
