void MediaControlTimelineElement::defaultEventHandler(Event* event) {
  if (event->isMouseEvent() &&
      toMouseEvent(event)->button() !=
          static_cast<short>(WebPointerProperties::Button::Left))
    return;

  if (!isConnected() || !document().isActive())
    return;

  if (event->type() == EventTypeNames::mousedown) {
    Platform::current()->recordAction(
        UserMetricsAction("Media.Controls.ScrubbingBegin"));
    mediaControls().beginScrubbing();
  }

  if (event->type() == EventTypeNames::mouseup) {
    Platform::current()->recordAction(
        UserMetricsAction("Media.Controls.ScrubbingEnd"));
    mediaControls().endScrubbing();
  }
 
   MediaControlInputElement::defaultEventHandler(event);
 
  if (event->type() == EventTypeNames::mouseover ||
      event->type() == EventTypeNames::mouseout ||
      event->type() == EventTypeNames::mousemove)
     return;
 
   double time = value().toDouble();
  if (event->type() == EventTypeNames::input) {
    if (mediaElement().seekable()->contain(time))
      mediaElement().setCurrentTime(time);
  }
 
   LayoutSliderItem slider = LayoutSliderItem(toLayoutSlider(layoutObject()));
   if (!slider.isNull() && slider.inDragMode())
    mediaControls().updateCurrentTimeDisplay();
}
