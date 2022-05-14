  void SetState(MediaStreamType stream_type, MediaRequestState new_state) {
    if (stream_type == NUM_MEDIA_TYPES) {
      for (int i = MEDIA_NO_SERVICE + 1; i < NUM_MEDIA_TYPES; ++i) {
        state_[static_cast<MediaStreamType>(i)] = new_state;
      }
    } else {
      state_[stream_type] = new_state;
    }

    MediaObserver* media_observer =
        GetContentClient()->browser()->GetMediaObserver();
     if (!media_observer)
       return;
 
    if (stream_type == NUM_MEDIA_TYPES) {
      for (int i = MEDIA_NO_SERVICE + 1; i < NUM_MEDIA_TYPES; ++i) {
        media_observer->OnMediaRequestStateChanged(
            target_process_id_, target_frame_id_, page_request_id,
            security_origin.GetURL(), static_cast<MediaStreamType>(i),
            new_state);
      }
    } else {
      media_observer->OnMediaRequestStateChanged(
          target_process_id_, target_frame_id_, page_request_id,
          security_origin.GetURL(), stream_type, new_state);
    }
   }
