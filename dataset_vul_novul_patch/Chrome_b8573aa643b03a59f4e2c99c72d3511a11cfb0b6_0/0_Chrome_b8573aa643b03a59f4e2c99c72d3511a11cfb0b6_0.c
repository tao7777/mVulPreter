 bool GesturePoint::IsInClickTimeWindow() const {
   double duration = last_touch_time_ - first_touch_time_;
  return duration >=
      GestureConfiguration::min_touch_down_duration_in_seconds_for_click() &&
      duration <
      GestureConfiguration::max_touch_down_duration_in_seconds_for_click();
 }
