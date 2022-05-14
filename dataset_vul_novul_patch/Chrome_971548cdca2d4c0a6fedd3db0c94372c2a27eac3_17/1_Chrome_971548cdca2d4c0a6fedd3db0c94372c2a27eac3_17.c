   std::string MakeMediaAccessRequest(int index) {
     const int render_process_id = 1;
     const int render_frame_id = 1;
     const int page_request_id = 1;
     const url::Origin security_origin;
     MediaStreamManager::MediaAccessRequestCallback callback =
         base::BindOnce(&MediaStreamManagerTest::ResponseCallback,
                        base::Unretained(this), index);
     StreamControls controls(true, true);
     return media_stream_manager_->MakeMediaAccessRequest(
        render_process_id, render_frame_id, page_request_id, controls,
        security_origin, std::move(callback));
   }
