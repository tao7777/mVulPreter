void AutofillDialogViews::SectionContainer::SetActive(bool active) {
  bool is_active = active && proxy_button_->visible();
   if (is_active == !!background())
     return;
 
  set_background(
      is_active ? views::Background::CreateSolidBackground(kLightShadingColor)
                : NULL);
   SchedulePaint();
 }
