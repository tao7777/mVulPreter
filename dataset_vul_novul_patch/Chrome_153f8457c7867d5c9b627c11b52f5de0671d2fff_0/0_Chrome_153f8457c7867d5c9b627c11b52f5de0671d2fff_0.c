void WebMediaPlayerImpl::OnError(PipelineStatus status) {
  DVLOG(1) << __func__;
  DCHECK(main_task_runner_->BelongsToCurrentThread());
  DCHECK_NE(status, PIPELINE_OK);

  if (suppress_destruction_errors_)
    return;
 
 #if defined(OS_ANDROID)
   if (status == PipelineStatus::DEMUXER_ERROR_DETECTED_HLS) {
    demuxer_found_hls_ = true;

     renderer_factory_selector_->SetUseMediaPlayer(true);
 
     pipeline_controller_.Stop();
    SetMemoryReportingState(false);

    main_task_runner_->PostTask(
        FROM_HERE, base::Bind(&WebMediaPlayerImpl::StartPipeline, AsWeakPtr()));
    return;
  }
#endif

  ReportPipelineError(load_type_, status, media_log_.get());
  media_log_->AddEvent(media_log_->CreatePipelineErrorEvent(status));
  media_metrics_provider_->OnError(status);
  if (watch_time_reporter_)
    watch_time_reporter_->OnError(status);

  if (ready_state_ == WebMediaPlayer::kReadyStateHaveNothing) {
    SetNetworkState(WebMediaPlayer::kNetworkStateFormatError);
  } else {
    SetNetworkState(PipelineErrorToNetworkState(status));
  }

  pipeline_controller_.Stop();

  UpdatePlayState();
}
