 void HTMLMediaElement::ChangeNetworkStateFromLoadingToIdle() {
   progress_event_timer_.Stop();
 
  if (!MediaShouldBeOpaque()) {
    // Schedule one last progress event so we guarantee that at least one is
    // fired for files that load very quickly.
    if (GetWebMediaPlayer() && GetWebMediaPlayer()->DidLoadingProgress())
      ScheduleEvent(EventTypeNames::progress);
    ScheduleEvent(EventTypeNames::suspend);
    SetNetworkState(kNetworkIdle);
  }
 }
