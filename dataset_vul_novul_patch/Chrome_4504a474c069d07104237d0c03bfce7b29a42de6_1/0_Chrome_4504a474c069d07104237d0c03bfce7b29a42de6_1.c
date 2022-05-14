void HTMLMediaElement::NoneSupported(const String& message) {
void HTMLMediaElement::NoneSupported(const String& input_message) {
  BLINK_MEDIA_LOG << "NoneSupported(" << (void*)this << ", message='"
                  << input_message << "')";
 
   StopPeriodicTimers();
   load_state_ = kWaitingForSource;
   current_source_node_ = nullptr;
 
  String empty_string;
  const String& message = MediaShouldBeOpaque() ? empty_string : input_message;

 
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
