 bool GesturePoint::IsOverMinFlickSpeed() {
  return velocity_calculator_.VelocitySquared() >
      GestureConfiguration::min_flick_speed_squared();
 }
