 bool GesturePoint::IsInSecondClickTimeWindow() const {
   double duration =  last_touch_time_ - last_tap_time_;
  return duration < GestureConfiguration::max_seconds_between_double_click();
 }
