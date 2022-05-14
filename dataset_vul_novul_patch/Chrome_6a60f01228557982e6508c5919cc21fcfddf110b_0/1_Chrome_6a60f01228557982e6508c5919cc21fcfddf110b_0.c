 ComponentControllerImpl::ComponentControllerImpl(WebContentRunner* runner)
    : runner_(runner),
      controller_binding_(this),
      frame_observer_binding_(this) {
   DCHECK(runner);
 }
