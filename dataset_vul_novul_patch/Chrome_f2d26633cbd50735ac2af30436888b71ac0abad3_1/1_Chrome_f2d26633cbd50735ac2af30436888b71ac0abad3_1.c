std::unique_ptr<views::Border> AutofillPopupBaseView::CreateBorder() {
  auto border = std::make_unique<views::BubbleBorder>(
      views::BubbleBorder::NONE, views::BubbleBorder::SMALL_SHADOW,
      SK_ColorWHITE);
  border->SetCornerRadius(GetCornerRadius());
  border->set_md_shadow_elevation(
      ChromeLayoutProvider::Get()->GetShadowElevationMetric(
          views::EMPHASIS_MEDIUM));
  return border;
}
