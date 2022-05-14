void InputHandlerProxy::InjectScrollbarGestureScroll(
    const WebInputEvent::Type type,
    const blink::WebFloatPoint& position_in_widget,
    const cc::InputHandlerPointerResult& pointer_result,
    const LatencyInfo& latency_info,
    const base::TimeTicks original_timestamp) {
  gfx::Vector2dF scroll_delta(pointer_result.scroll_offset.x(),
                              pointer_result.scroll_offset.y());
  std::unique_ptr<WebGestureEvent> synthetic_gesture_event =
      GenerateInjectedScrollGesture(
          type, original_timestamp, blink::WebGestureDevice::kScrollbar,
          position_in_widget, scroll_delta,
          pointer_result.scroll_units);

  WebScopedInputEvent web_scoped_gesture_event(
      synthetic_gesture_event.release());

  LatencyInfo scrollbar_latency_info(latency_info);
  scrollbar_latency_info.set_source_event_type(ui::SourceEventType::SCROLLBAR);

   DCHECK(!scrollbar_latency_info.FindLatency(
       ui::INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_IMPL_COMPONENT, nullptr));
 
   std::unique_ptr<EventWithCallback> gesture_event_with_callback_update =
       std::make_unique<EventWithCallback>(
           std::move(web_scoped_gesture_event), scrollbar_latency_info,
          original_timestamp, original_timestamp, nullptr);

  bool needs_animate_input = compositor_event_queue_->empty();
  compositor_event_queue_->Queue(std::move(gesture_event_with_callback_update),
                                 original_timestamp);

  if (needs_animate_input)
    input_handler_->SetNeedsAnimateInput();
}
