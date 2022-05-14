 bool GesturePoint::IsInClickTimeWindow() const {
   double duration = last_touch_time_ - first_touch_time_;
  return duration >= kMinimumTouchDownDurationInSecondsForClick &&
         duration < kMaximumTouchDownDurationInSecondsForClick;
 }
