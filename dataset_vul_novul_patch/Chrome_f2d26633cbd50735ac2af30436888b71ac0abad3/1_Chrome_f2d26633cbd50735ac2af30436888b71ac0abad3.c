void AutofillPopupBaseView::AddExtraInitParams(
    views::Widget::InitParams* params) {
  params->opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;
  params->shadow_type = views::Widget::InitParams::SHADOW_TYPE_NONE;
}
