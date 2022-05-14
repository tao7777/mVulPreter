InputHandlerProxy::InputHandlerProxy(cc::InputHandler* input_handler,
                                     InputHandlerProxyClient* client,
                                     bool force_input_to_main_thread)
    : client_(client),
      input_handler_(input_handler),
      synchronous_input_handler_(nullptr),
      allow_root_animate_(true),
#if DCHECK_IS_ON()
      expect_scroll_update_end_(false),
#endif
      gesture_scroll_on_impl_thread_(false),
      scroll_sequence_ignored_(false),
      smooth_scroll_enabled_(false),
      touch_result_(kEventDispositionUndefined),
      mouse_wheel_result_(kEventDispositionUndefined),
       current_overscroll_params_(nullptr),
       has_ongoing_compositor_scroll_or_pinch_(false),
       is_first_gesture_scroll_update_(false),
       tick_clock_(base::DefaultTickClock::GetInstance()),
       snap_fling_controller_(std::make_unique<cc::SnapFlingController>(this)),
       compositor_touch_action_enabled_(
          base::FeatureList::IsEnabled(features::kCompositorTouchAction)),
      force_input_to_main_thread_(force_input_to_main_thread) {
  DCHECK(client);
  input_handler_->BindToClient(this);
  cc::ScrollElasticityHelper* scroll_elasticity_helper =
      input_handler_->CreateScrollElasticityHelper();
  if (scroll_elasticity_helper) {
    scroll_elasticity_controller_.reset(
        new InputScrollElasticityController(scroll_elasticity_helper));
  }
  compositor_event_queue_ = std::make_unique<CompositorThreadEventQueue>();
  scroll_predictor_ = std::make_unique<ScrollPredictor>(
      base::FeatureList::IsEnabled(features::kResamplingScrollEvents));

  if (base::FeatureList::IsEnabled(features::kSkipTouchEventFilter) &&
      GetFieldTrialParamValueByFeature(
          features::kSkipTouchEventFilter,
          features::kSkipTouchEventFilterFilteringProcessParamName) ==
          features::
              kSkipTouchEventFilterFilteringProcessParamValueBrowserAndRenderer) {
    skip_touch_filter_discrete_ = true;
    if (GetFieldTrialParamValueByFeature(
            features::kSkipTouchEventFilter,
            features::kSkipTouchEventFilterTypeParamName) ==
        features::kSkipTouchEventFilterTypeParamValueAll) {
      skip_touch_filter_all_ = true;
    }
  }
}
