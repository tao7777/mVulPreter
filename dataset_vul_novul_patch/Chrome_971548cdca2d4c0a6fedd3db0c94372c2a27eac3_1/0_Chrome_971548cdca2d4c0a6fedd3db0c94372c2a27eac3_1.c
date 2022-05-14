void MediaStreamDispatcherHost::BindRequest(
void MediaStreamDispatcherHost::Create(
    int render_process_id,
    int render_frame_id,
    MediaStreamManager* media_stream_manager,
     mojom::MediaStreamDispatcherHostRequest request) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  mojo::MakeStrongBinding(
      std::make_unique<MediaStreamDispatcherHost>(
          render_process_id, render_frame_id, media_stream_manager),
      std::move(request));
 }
