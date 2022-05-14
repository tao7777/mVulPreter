void LauncherView::ButtonPressed(views::Button* sender,
                                 const views::Event& event) {
   if (dragging_)
     return;
 
  if (sender == overflow_button_) {
    ShowOverflowBubble();
    return;
  }
 
   if (!delegate_)
     return;
  int view_index = view_model_->GetIndexOfView(sender);
  if (view_index == -1)
    return;

  switch (model_->items()[view_index].type) {
    case TYPE_TABBED:
    case TYPE_APP_PANEL:
    case TYPE_APP_SHORTCUT:
    case TYPE_PLATFORM_APP:
      delegate_->ItemClicked(model_->items()[view_index], event.flags());
      break;

    case TYPE_APP_LIST:
      Shell::GetInstance()->ToggleAppList();
      break;

    case TYPE_BROWSER_SHORTCUT:
      if (event.flags() & ui::EF_CONTROL_DOWN)
        delegate_->CreateNewWindow();
      else
        delegate_->CreateNewTab();
      break;
  }
}
