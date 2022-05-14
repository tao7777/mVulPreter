void HTMLMediaElement::NoneSupported(const String& message) {
  BLINK_MEDIA_LOG << "NoneSupported(" << (void*)this << ", message='" << message
                  << "')";
 
   StopPeriodicTimers();
   load_state_ = kWaitingForSource;
   current_source_node_ = nullptr;
 
 
  error_ = MediaError::Create(MediaError::kMediaErrSrcNotSupported, message);

  ForgetResourceSpecificTracks();

  SetNetworkState(kNetworkNoSource);

  UpdateDisplayState();

  ScheduleEvent(EventTypeNames::error);

  ScheduleRejectPlayPromises(kNotSupportedError);

  CloseMediaSource();

  SetShouldDelayLoadEvent(false);

  if (GetLayoutObject())
    GetLayoutObject()->UpdateFromElement();
}
