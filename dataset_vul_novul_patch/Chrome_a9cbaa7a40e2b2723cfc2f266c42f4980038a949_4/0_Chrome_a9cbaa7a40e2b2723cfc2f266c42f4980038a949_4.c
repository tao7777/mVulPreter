void MultibufferDataSource::StartCallback() {
  DCHECK(render_task_runner_->BelongsToCurrentThread());

  if (!init_cb_) {
    SetReader(nullptr);
    return;
  }

  bool success = reader_ && reader_->Available() > 0 && url_data() &&
                 (!assume_fully_buffered() ||
                  url_data()->length() != kPositionNotSpecified);

  if (success) {
    {
      base::AutoLock auto_lock(lock_);
      total_bytes_ = url_data()->length();
    }
    streaming_ =
        !assume_fully_buffered() && (total_bytes_ == kPositionNotSpecified ||
                                     !url_data()->range_supported());

    media_log_->SetDoubleProperty("total_bytes",
                                  static_cast<double>(total_bytes_));
    media_log_->SetBooleanProperty("streaming", streaming_);
  } else {
    SetReader(nullptr);
  }

  base::AutoLock auto_lock(lock_);
  if (stop_signal_received_)
    return;

  if (success) {
    if (total_bytes_ != kPositionNotSpecified) {
      host_->SetTotalBytes(total_bytes_);
      if (assume_fully_buffered())
        host_->AddBufferedByteRange(0, total_bytes_);
    }

     media_log_->SetBooleanProperty("single_origin", single_origin_);
     media_log_->SetBooleanProperty("range_header_supported",
                                    url_data()->range_supported());
   }

  render_task_runner_->PostTask(FROM_HERE,
                                base::Bind(std::move(init_cb_), success));

  UpdateBufferSizes();

  UpdateLoadingState_Locked(true);
}
