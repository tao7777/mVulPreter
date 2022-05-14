TabStyle::TabColors GM2TabStyle::CalculateColors() const {
  const ui::ThemeProvider* theme_provider = tab_->GetThemeProvider();

  constexpr float kMinimumActiveContrastRatio = 6.05f;
  constexpr float kMinimumInactiveContrastRatio = 4.61f;
  constexpr float kMinimumHoveredContrastRatio = 5.02f;
  constexpr float kMinimumPressedContrastRatio = 4.41f;

  float expected_opacity = 0.0f;
  if (tab_->IsActive()) {
    expected_opacity = 1.0f;
  } else if (tab_->IsSelected()) {
    expected_opacity = kSelectedTabOpacity;
  } else if (tab_->mouse_hovered()) {
     expected_opacity = GetHoverOpacity();
   }
   const SkColor bg_color = color_utils::AlphaBlend(
      tab_->controller()->GetTabBackgroundColor(TAB_ACTIVE),
      tab_->controller()->GetTabBackgroundColor(TAB_INACTIVE),
       expected_opacity);
 
   SkColor title_color = tab_->controller()->GetTabForegroundColor(
      expected_opacity > 0.5f ? TAB_ACTIVE : TAB_INACTIVE, bg_color);
  title_color = color_utils::GetColorWithMinimumContrast(title_color, bg_color);

  const SkColor base_hovered_color = theme_provider->GetColor(
      ThemeProperties::COLOR_TAB_CLOSE_BUTTON_BACKGROUND_HOVER);
  const SkColor base_pressed_color = theme_provider->GetColor(
      ThemeProperties::COLOR_TAB_CLOSE_BUTTON_BACKGROUND_PRESSED);

  const auto get_color_for_contrast_ratio = [](SkColor fg_color,
                                               SkColor bg_color,
                                               float contrast_ratio) {
    const SkAlpha blend_alpha = color_utils::GetBlendValueWithMinimumContrast(
        bg_color, fg_color, bg_color, contrast_ratio);
    return color_utils::AlphaBlend(fg_color, bg_color, blend_alpha);
  };

  const SkColor generated_icon_color = get_color_for_contrast_ratio(
      title_color, bg_color,
      tab_->IsActive() ? kMinimumActiveContrastRatio
                       : kMinimumInactiveContrastRatio);
  const SkColor generated_hovered_color = get_color_for_contrast_ratio(
      base_hovered_color, bg_color, kMinimumHoveredContrastRatio);
  const SkColor generated_pressed_color = get_color_for_contrast_ratio(
      base_pressed_color, bg_color, kMinimumPressedContrastRatio);

  const SkColor generated_hovered_icon_color =
      color_utils::GetColorWithMinimumContrast(title_color,
                                               generated_hovered_color);
  const SkColor generated_pressed_icon_color =
      color_utils::GetColorWithMinimumContrast(title_color,
                                               generated_pressed_color);

  return {bg_color,
          title_color,
          generated_icon_color,
          generated_hovered_icon_color,
          generated_pressed_icon_color,
          generated_hovered_color,
          generated_pressed_color};
}
