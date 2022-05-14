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
 
  if (event->type() != EventTypeNames::input)
     return;
 
   double time = value().toDouble();

  // FIXME: This will need to take the timeline offset into consideration
  // once that concept is supported, see https://crbug.com/312699
  if (mediaElement().seekable()->contain(time))
    mediaElement().setCurrentTime(time);
 
   LayoutSliderItem slider = LayoutSliderItem(toLayoutSlider(layoutObject()));
   if (!slider.isNull() && slider.inDragMode())
    mediaControls().updateCurrentTimeDisplay();
}
