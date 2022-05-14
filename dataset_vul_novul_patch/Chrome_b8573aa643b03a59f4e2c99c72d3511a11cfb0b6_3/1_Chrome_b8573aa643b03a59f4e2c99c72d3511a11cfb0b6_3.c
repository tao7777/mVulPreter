 bool GesturePoint::IsOverMinFlickSpeed() {
  return velocity_calculator_.VelocitySquared() > kMinFlickSpeedSquared;
 }
