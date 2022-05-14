 void MediaControlVolumeSliderElement::defaultEventHandler(Event* event) {
  if (event->isMouseEvent() &&
      toMouseEvent(event)->button() !=
          static_cast<short>(WebPointerProperties::Button::Left))
    return;
   if (!isConnected() || !document().isActive())
     return;
 
   MediaControlInputElement::defaultEventHandler(event);
 
  if (event->type() == EventTypeNames::mouseover ||
      event->type() == EventTypeNames::mouseout ||
      event->type() == EventTypeNames::mousemove)
    return;
   if (event->type() == EventTypeNames::mousedown)
     Platform::current()->recordAction(
         UserMetricsAction("Media.Controls.VolumeChangeBegin"));

  if (event->type() == EventTypeNames::mouseup)
     Platform::current()->recordAction(
         UserMetricsAction("Media.Controls.VolumeChangeEnd"));
 
  double volume = value().toDouble();
  mediaElement().setVolume(volume);
  mediaElement().setMuted(false);
 }
