void LauncherView::ShowOverflowMenu() {
#if !defined(OS_MACOSX)
  if (!delegate_)
    return;
 
  std::vector<LauncherItem> items;
  GetOverflowItems(&items);
  if (items.empty())
    return;
 
  MenuDelegateImpl menu_delegate;
  ui::SimpleMenuModel menu_model(&menu_delegate);
  for (size_t i = 0; i < items.size(); ++i)
    menu_model.AddItem(static_cast<int>(i), delegate_->GetTitle(items[i]));
  views::MenuModelAdapter menu_adapter(&menu_model);
  overflow_menu_runner_.reset(new views::MenuRunner(menu_adapter.CreateMenu()));
  gfx::Rect bounds(overflow_button_->size());
  gfx::Point origin;
  ConvertPointToScreen(overflow_button_, &origin);
  if (overflow_menu_runner_->RunMenuAt(GetWidget(), NULL,
          gfx::Rect(origin, size()), views::MenuItemView::TOPLEFT, 0) ==
      views::MenuRunner::MENU_DELETED)
    return;
 
   Shell::GetInstance()->UpdateShelfVisibility();
  if (menu_delegate.activated_command_id() == -1)
    return;
  LauncherID activated_id = items[menu_delegate.activated_command_id()].id;
  LauncherItems::const_iterator window_iter = model_->ItemByID(activated_id);
  if (window_iter == model_->items().end())
    return;  // Window was deleted while menu was up.
  delegate_->ItemClicked(*window_iter, ui::EF_NONE);
#endif  // !defined(OS_MACOSX)
 }
