void HTMLMediaElement::ProgressEventTimerFired(TimerBase*) {
   if (network_state_ != kNetworkLoading)
     return;
 
  // If this is an cross-origin request, and we haven't discovered whether
  // the media is actually playable yet, don't fire any progress events as
  // those may let the page know information about the resource that it's
  // not supposed to know.
  if (MediaShouldBeOpaque())
    return;

   double time = WTF::CurrentTime();
   double timedelta = time - previous_progress_time_;
 
  if (GetWebMediaPlayer() && GetWebMediaPlayer()->DidLoadingProgress()) {
    ScheduleEvent(EventTypeNames::progress);
    previous_progress_time_ = time;
    sent_stalled_event_ = false;
    if (GetLayoutObject())
      GetLayoutObject()->UpdateFromElement();
  } else if (timedelta > 3.0 && !sent_stalled_event_) {
    ScheduleEvent(EventTypeNames::stalled);
    sent_stalled_event_ = true;
    SetShouldDelayLoadEvent(false);
  }
}
