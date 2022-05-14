 bool GesturePoint::IsInsideManhattanSquare(const TouchEvent& event) const {
   int manhattanDistance = abs(event.x() - first_touch_position_.x()) +
                           abs(event.y() - first_touch_position_.y());
  return manhattanDistance < kMaximumTouchMoveInPixelsForClick;
 }
