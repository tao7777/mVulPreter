MediaControlsHeaderView::MediaControlsHeaderView() {
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kHorizontal, kMediaControlsHeaderInsets,
      kMediaControlsHeaderChildSpacing));
 
   auto app_icon_view = std::make_unique<views::ImageView>();
   app_icon_view->SetImageSize(gfx::Size(kIconSize, kIconSize));
  app_icon_view->SetVerticalAlignment(views::ImageView::Alignment::kLeading);
  app_icon_view->SetHorizontalAlignment(views::ImageView::Alignment::kLeading);
  app_icon_view->SetBorder(views::CreateEmptyBorder(kIconPadding));
  app_icon_view->SetBackground(
      views::CreateRoundedRectBackground(SK_ColorWHITE, kIconCornerRadius));
  app_icon_view_ = AddChildView(std::move(app_icon_view));

  gfx::Font default_font;
  int font_size_delta = kHeaderTextFontSize - default_font.GetFontSize();
  gfx::Font font = default_font.Derive(font_size_delta, gfx::Font::NORMAL,
                                       gfx::Font::Weight::NORMAL);
  gfx::FontList font_list(font);

  auto app_name_view = std::make_unique<views::Label>();
  app_name_view->SetFontList(font_list);
   app_name_view->SetHorizontalAlignment(gfx::ALIGN_LEFT);
   app_name_view->SetEnabledColor(SK_ColorWHITE);
   app_name_view->SetAutoColorReadabilityEnabled(false);
   app_name_view_ = AddChildView(std::move(app_name_view));
 }
