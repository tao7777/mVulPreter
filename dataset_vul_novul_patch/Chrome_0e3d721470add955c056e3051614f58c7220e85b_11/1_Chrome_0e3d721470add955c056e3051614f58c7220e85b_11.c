 SynchronousCompositorImpl::SynchronousCompositorImpl(WebContents* contents)
    : compositor_client_(NULL),
      output_surface_(NULL),
       begin_frame_source_(nullptr),
       contents_(contents),
       routing_id_(contents->GetRoutingID()),
      input_handler_(NULL),
      is_active_(false),
       renderer_needs_begin_frames_(false),
       weak_ptr_factory_(this) {
   DCHECK(contents);
  DCHECK_NE(routing_id_, MSG_ROUTING_NONE);
}
