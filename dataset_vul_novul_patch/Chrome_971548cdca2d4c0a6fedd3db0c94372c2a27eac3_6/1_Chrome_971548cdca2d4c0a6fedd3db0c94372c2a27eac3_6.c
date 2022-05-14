 MediaStreamDispatcherHost::MediaStreamDispatcherHost(
     int render_process_id,
     int render_frame_id,
     MediaStreamManager* media_stream_manager)
     : render_process_id_(render_process_id),
       render_frame_id_(render_frame_id),
       media_stream_manager_(media_stream_manager),
       salt_and_origin_callback_(
           base::BindRepeating(&GetMediaDeviceSaltAndOrigin)),
       weak_factory_(this) {
   DCHECK_CURRENTLY_ON(BrowserThread::IO);
  bindings_.set_connection_error_handler(
      base::Bind(&MediaStreamDispatcherHost::CancelAllRequests,
                 weak_factory_.GetWeakPtr()));
 }
