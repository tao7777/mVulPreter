 MediaControlsProgressView::MediaControlsProgressView(
     base::RepeatingCallback<void(double)> seek_callback)
     : seek_callback_(std::move(seek_callback)) {
   SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, kProgressViewInsets));
 
  progress_bar_ = AddChildView(std::make_unique<views::ProgressBar>(5, false));
  progress_bar_->SetBorder(views::CreateEmptyBorder(kProgressBarInsets));
 
   gfx::Font default_font;
  int font_size_delta = kProgressTimeFontSize - default_font.GetFontSize();
  gfx::Font font = default_font.Derive(font_size_delta, gfx::Font::NORMAL,
                                       gfx::Font::Weight::NORMAL);
  gfx::FontList font_list(font);

  auto time_view = std::make_unique<views::View>();
  auto* time_view_layout =
      time_view->SetLayoutManager(std::make_unique<views::FlexLayout>());
  time_view_layout->SetOrientation(views::LayoutOrientation::kHorizontal)
      .SetMainAxisAlignment(views::LayoutAlignment::kCenter)
      .SetCrossAxisAlignment(views::LayoutAlignment::kCenter)
      .SetCollapseMargins(true);

  auto progress_time = std::make_unique<views::Label>();
  progress_time->SetFontList(font_list);
  progress_time->SetEnabledColor(SK_ColorWHITE);
  progress_time->SetAutoColorReadabilityEnabled(false);
  progress_time_ = time_view->AddChildView(std::move(progress_time));

  auto time_spacing = std::make_unique<views::View>();
  time_spacing->SetPreferredSize(kTimeSpacingSize);
  time_spacing->SetProperty(views::kFlexBehaviorKey,
                            views::FlexSpecification::ForSizeRule(
                                views::MinimumFlexSizeRule::kPreferred,
                                views::MaximumFlexSizeRule::kUnbounded));
  time_view->AddChildView(std::move(time_spacing));

  auto duration = std::make_unique<views::Label>();
  duration->SetFontList(font_list);
  duration->SetEnabledColor(SK_ColorWHITE);
  duration->SetAutoColorReadabilityEnabled(false);
  duration_ = time_view->AddChildView(std::move(duration));

  AddChildView(std::move(time_view));
}
