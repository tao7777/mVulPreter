CastCastView::CastCastView(CastConfigDelegate* cast_config_delegate)
    : cast_config_delegate_(cast_config_delegate) {

  set_background(views::Background::CreateSolidBackground(kBackgroundColor));
  ui::ResourceBundle& bundle = ui::ResourceBundle::GetSharedInstance();
  SetLayoutManager(new views::BoxLayout(views::BoxLayout::kHorizontal,
                                        kTrayPopupPaddingHorizontal, 0,
                                        kTrayPopupPaddingBetweenItems));
  icon_ = new FixedSizedImageView(0, kTrayPopupItemHeight);
  icon_->SetImage(
      bundle.GetImageNamed(IDR_AURA_UBER_TRAY_CAST_ENABLED).ToImageSkia());
  AddChildView(icon_);

  label_container_ = new views::View;
  label_container_->SetLayoutManager(
      new views::BoxLayout(views::BoxLayout::kVertical, 0, 0, 0));

   title_ = new views::Label;
   title_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
   title_->SetFontList(bundle.GetFontList(ui::ResourceBundle::BoldFont));
   label_container_->AddChildView(title_);
 
   details_ = new views::Label;
   details_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
   details_->SetMultiLine(false);
   details_->SetEnabledColor(kHeaderTextColorNormal);
   label_container_->AddChildView(details_);
 
   AddChildView(label_container_);

  base::string16 stop_button_text =
      ui::ResourceBundle::GetSharedInstance().GetLocalizedString(
          IDS_ASH_STATUS_TRAY_CAST_STOP);
  stop_button_ = new TrayPopupLabelButton(this, stop_button_text);
  AddChildView(stop_button_);

  UpdateLabel();
}
