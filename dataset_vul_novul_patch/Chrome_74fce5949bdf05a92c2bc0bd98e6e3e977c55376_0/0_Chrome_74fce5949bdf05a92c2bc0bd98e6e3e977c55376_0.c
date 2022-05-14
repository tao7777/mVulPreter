 void MediaControlVolumeSliderElement::defaultEventHandler(Event* event) {
   if (!isConnected() || !document().isActive())
     return;
 
   MediaControlInputElement::defaultEventHandler(event);
 
   if (event->type() == EventTypeNames::mousedown)
     Platform::current()->recordAction(
         UserMetricsAction("Media.Controls.VolumeChangeBegin"));

  if (event->type() == EventTypeNames::mouseup)
     Platform::current()->recordAction(
         UserMetricsAction("Media.Controls.VolumeChangeEnd"));
 
  if (event->type() == EventTypeNames::input) {
    double volume = value().toDouble();
    mediaElement().setVolume(volume);
    mediaElement().setMuted(false);
  }
 }
