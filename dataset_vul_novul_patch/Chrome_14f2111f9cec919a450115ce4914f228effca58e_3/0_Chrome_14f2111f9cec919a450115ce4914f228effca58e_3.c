void InputHandlerProxy::DispatchSingleInputEvent(
    std::unique_ptr<EventWithCallback> event_with_callback,
    const base::TimeTicks now) {
  const ui::LatencyInfo& original_latency_info =
      event_with_callback->latency_info();
  ui::LatencyInfo monitored_latency_info = original_latency_info;
  std::unique_ptr<cc::SwapPromiseMonitor> latency_info_swap_promise_monitor =
      input_handler_->CreateLatencyInfoSwapPromiseMonitor(
          &monitored_latency_info);
 
   current_overscroll_params_.reset();
 
  blink::WebGestureEvent::Type type = event_with_callback->event().GetType();
  if (type == blink::WebGestureEvent::kGestureScrollUpdate) {
    EnsureScrollUpdateLatencyComponent(
        &monitored_latency_info, event_with_callback->event().TimeStamp());
  }

   InputHandlerProxy::EventDisposition disposition = RouteToTypeSpecificHandler(
       event_with_callback->event(), original_latency_info);
 
   switch (type) {
     case blink::WebGestureEvent::kGestureScrollBegin:
       is_first_gesture_scroll_update_ = true;
      FALLTHROUGH;
    case blink::WebGestureEvent::kGesturePinchBegin:
    case blink::WebGestureEvent::kGestureScrollUpdate:
    case blink::WebGestureEvent::kGesturePinchUpdate:
      has_ongoing_compositor_scroll_or_pinch_ = disposition == DID_HANDLE;
      break;

    case blink::WebGestureEvent::kGestureScrollEnd:
    case blink::WebGestureEvent::kGesturePinchEnd:
      has_ongoing_compositor_scroll_or_pinch_ = false;
      break;
    default:
      break;
  }

  switch (type) {
    case blink::WebGestureEvent::kGestureScrollBegin:
      momentum_scroll_jank_tracker_ =
          std::make_unique<MomentumScrollJankTracker>();
      break;
    case blink::WebGestureEvent::kGestureScrollUpdate:
      if (momentum_scroll_jank_tracker_) {
        momentum_scroll_jank_tracker_->OnDispatchedInputEvent(
            event_with_callback.get(), now);
      }
      break;
    case blink::WebGestureEvent::kGestureScrollEnd:
      momentum_scroll_jank_tracker_.reset();
      break;
    default:
      break;
  }

  event_with_callback->RunCallbacks(disposition, monitored_latency_info,
                                     std::move(current_overscroll_params_));
 }
