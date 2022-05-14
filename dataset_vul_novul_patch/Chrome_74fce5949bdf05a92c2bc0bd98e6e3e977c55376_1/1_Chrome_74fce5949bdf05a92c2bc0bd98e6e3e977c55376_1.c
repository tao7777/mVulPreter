bool isUserInteractionEventForSlider(Event* event, LayoutObject* layoutObject) {
  if (isUserInteractionEvent(event))
    return true;

  LayoutSliderItem slider = LayoutSliderItem(toLayoutSlider(layoutObject));
  if (!slider.isNull() && !slider.inDragMode())
    return false;
 
   const AtomicString& type = event->type();
   return type == EventTypeNames::mouseover ||
         type == EventTypeNames::mouseout || type == EventTypeNames::mousemove;
 }
