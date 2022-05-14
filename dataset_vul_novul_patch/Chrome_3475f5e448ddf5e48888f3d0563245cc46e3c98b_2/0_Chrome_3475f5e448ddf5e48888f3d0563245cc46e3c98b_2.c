void LauncherView::Init() {
  ResourceBundle& rb = ResourceBundle::GetSharedInstance();
  model_->AddObserver(this);

  const LauncherItems& items(model_->items());
  for (LauncherItems::const_iterator i = items.begin(); i != items.end(); ++i) {
    views::View* child = CreateViewForItem(*i);
    child->SetPaintToLayer(true);
    view_model_->Add(child, static_cast<int>(i - items.begin()));
    AddChildView(child);
  }
  UpdateFirstButtonPadding();
 
   overflow_button_ = new views::ImageButton(this);
   overflow_button_->set_accessibility_focusable(true);
  overflow_button_->SetImageAlignment(views::ImageButton::ALIGN_CENTER,
                                      views::ImageButton::ALIGN_MIDDLE);
   overflow_button_->SetImage(
       views::CustomButton::BS_NORMAL,
       rb.GetImageNamed(IDR_AURA_LAUNCHER_OVERFLOW).ToImageSkia());
  overflow_button_->SetImage(
      views::CustomButton::BS_HOT,
      rb.GetImageNamed(IDR_AURA_LAUNCHER_OVERFLOW_HOT).ToImageSkia());
  overflow_button_->SetImage(
      views::CustomButton::BS_PUSHED,
      rb.GetImageNamed(IDR_AURA_LAUNCHER_OVERFLOW_PUSHED).ToImageSkia());
  overflow_button_->SetAccessibleName(
      l10n_util::GetStringUTF16(IDS_AURA_LAUNCHER_OVERFLOW_NAME));
  overflow_button_->set_context_menu_controller(this);
  ConfigureChildView(overflow_button_);
  AddChildView(overflow_button_);

}
