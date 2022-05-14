 bool GesturePoint::IsSecondClickInsideManhattanSquare(
     const TouchEvent& event) const {
   int manhattanDistance = abs(event.x() - last_tap_position_.x()) +
                           abs(event.y() - last_tap_position_.y());
  return manhattanDistance <
      GestureConfiguration::max_touch_move_in_pixels_for_click();
 }
