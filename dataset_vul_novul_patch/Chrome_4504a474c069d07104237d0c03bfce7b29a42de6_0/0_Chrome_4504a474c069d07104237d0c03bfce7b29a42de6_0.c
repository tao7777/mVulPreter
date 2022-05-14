 void HTMLMediaElement::MediaLoadingFailed(WebMediaPlayer::NetworkState error,
                                          const String& input_message) {
   BLINK_MEDIA_LOG << "MediaLoadingFailed(" << (void*)this << ", "
                  << static_cast<int>(error) << ", message='" << input_message
                   << "')";
 
  bool should_be_opaque = MediaShouldBeOpaque();
  if (should_be_opaque)
    error = WebMediaPlayer::kNetworkStateNetworkError;
  String empty_string;
  const String& message = should_be_opaque ? empty_string : input_message;

   StopPeriodicTimers();
 
  if (ready_state_ < kHaveMetadata &&
      load_state_ == kLoadingFromSourceElement) {
    if (current_source_node_) {
      current_source_node_->ScheduleErrorEvent();
    } else {
      BLINK_MEDIA_LOG << "mediaLoadingFailed(" << (void*)this
                      << ") - error event not sent, <source> was removed";
    }


    ForgetResourceSpecificTracks();

    if (HavePotentialSourceChild()) {
      BLINK_MEDIA_LOG << "mediaLoadingFailed(" << (void*)this
                      << ") - scheduling next <source>";
      ScheduleNextSourceChild();
    } else {
      BLINK_MEDIA_LOG << "mediaLoadingFailed(" << (void*)this
                      << ") - no more <source> elements, waiting";
      WaitForSourceChange();
    }

    return;
  }

  if (error == WebMediaPlayer::kNetworkStateNetworkError &&
      ready_state_ >= kHaveMetadata) {
    MediaEngineError(MediaError::Create(MediaError::kMediaErrNetwork, message));
  } else if (error == WebMediaPlayer::kNetworkStateDecodeError) {
    MediaEngineError(MediaError::Create(MediaError::kMediaErrDecode, message));
  } else if ((error == WebMediaPlayer::kNetworkStateFormatError ||
              error == WebMediaPlayer::kNetworkStateNetworkError) &&
             load_state_ == kLoadingFromSrcAttr) {
    if (message.IsEmpty()) {
      NoneSupported(BuildElementErrorMessage(
          error == WebMediaPlayer::kNetworkStateFormatError ? "Format error"
                                                            : "Network error"));
    } else {
      NoneSupported(message);
    }
  }

  UpdateDisplayState();
}
