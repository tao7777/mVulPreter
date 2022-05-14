MediaControlsHeaderView::MediaControlsHeaderView() {
MediaControlsHeaderView::MediaControlsHeaderView(
    base::OnceClosure close_button_cb)
    : close_button_cb_(std::move(close_button_cb)) {
  auto* layout = SetLayoutManager(std::make_unique<views::FlexLayout>());
  layout->SetInteriorMargin(kHeaderViewInsets);
 
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
  app_name_view->SetBorder(views::CreateEmptyBorder(kAppNamePadding));
   app_name_view_ = AddChildView(std::move(app_name_view));

  // Space between app name and close button.
  auto spacer = std::make_unique<NonAccessibleView>();
  spacer->SetPreferredSize(kSpacerPreferredSize);
  spacer->SetProperty(views::kFlexBehaviorKey,
                      views::FlexSpecification::ForSizeRule(
                          views::MinimumFlexSizeRule::kScaleToMinimum,
                          views::MaximumFlexSizeRule::kUnbounded));
  AddChildView(std::move(spacer));

  auto close_button = CreateVectorImageButton(this);
  SetImageFromVectorIcon(close_button.get(), vector_icons::kCloseRoundedIcon,
                         kCloseButtonIconSize, gfx::kGoogleGrey700);
  close_button->SetPreferredSize(kCloseButtonSize);
  close_button->SetFocusBehavior(View::FocusBehavior::ALWAYS);
  base::string16 close_button_label(
      l10n_util::GetStringUTF16(IDS_ASH_LOCK_SCREEN_MEDIA_CONTROLS_CLOSE));
  close_button->SetAccessibleName(close_button_label);
  close_button->SetVisible(false);
  close_button_ = AddChildView(std::move(close_button));
 }
