 void ShellWindowFrameView::Init(views::Widget* frame) {
   frame_ = frame;

  if (!is_frameless_) {
    ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
    close_button_ = new views::ImageButton(this);
    close_button_->SetImage(views::CustomButton::BS_NORMAL,
        rb.GetNativeImageNamed(IDR_CLOSE_BAR).ToImageSkia());
    close_button_->SetImage(views::CustomButton::BS_HOT,
        rb.GetNativeImageNamed(IDR_CLOSE_BAR_H).ToImageSkia());
    close_button_->SetImage(views::CustomButton::BS_PUSHED,
        rb.GetNativeImageNamed(IDR_CLOSE_BAR_P).ToImageSkia());
    close_button_->SetAccessibleName(
        l10n_util::GetStringUTF16(IDS_APP_ACCNAME_CLOSE));
    AddChildView(close_button_);
  }
 
 #if defined(USE_ASH)
   aura::Window* window = frame->GetNativeWindow();
  int outside_bounds = ui::GetDisplayLayout() == ui::LAYOUT_TOUCH ?
      kResizeOutsideBoundsSizeTouch :
      kResizeOutsideBoundsSize;
  window->set_hit_test_bounds_override_outer(
      gfx::Insets(-outside_bounds, -outside_bounds,
                  -outside_bounds, -outside_bounds));
  window->set_hit_test_bounds_override_inner(
      gfx::Insets(kResizeInsideBoundsSize, kResizeInsideBoundsSize,
                  kResizeInsideBoundsSize, kResizeInsideBoundsSize));
#endif
 }
