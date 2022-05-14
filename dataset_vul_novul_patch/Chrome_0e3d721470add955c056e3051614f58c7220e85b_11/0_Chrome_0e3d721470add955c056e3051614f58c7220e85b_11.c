 SynchronousCompositorImpl::SynchronousCompositorImpl(WebContents* contents)
    : compositor_client_(nullptr),
      output_surface_(nullptr),
       begin_frame_source_(nullptr),
       contents_(contents),
       routing_id_(contents->GetRoutingID()),
      input_handler_(nullptr),
      registered_with_client_(false),
      is_active_(true),
       renderer_needs_begin_frames_(false),
       weak_ptr_factory_(this) {
   DCHECK(contents);
  DCHECK_NE(routing_id_, MSG_ROUTING_NONE);
}
