void BrowserViewRenderer::DidOverscroll(gfx::Vector2dF accumulated_overscroll,
void BrowserViewRenderer::DidOverscroll(
    const gfx::Vector2dF& accumulated_overscroll,
    const gfx::Vector2dF& latest_overscroll_delta,
    const gfx::Vector2dF& current_fling_velocity) {
   const float physical_pixel_scale = dip_scale_ * page_scale_factor_;
   if (accumulated_overscroll == latest_overscroll_delta)
     overscroll_rounding_error_ = gfx::Vector2dF();
  gfx::Vector2dF scaled_overscroll_delta =
      gfx::ScaleVector2d(latest_overscroll_delta, physical_pixel_scale);
  gfx::Vector2d rounded_overscroll_delta = gfx::ToRoundedVector2d(
      scaled_overscroll_delta + overscroll_rounding_error_);
  overscroll_rounding_error_ =
      scaled_overscroll_delta - rounded_overscroll_delta;
  gfx::Vector2dF fling_velocity_pixels =
      gfx::ScaleVector2d(current_fling_velocity, physical_pixel_scale);

  client_->DidOverscroll(rounded_overscroll_delta, fling_velocity_pixels);
}
