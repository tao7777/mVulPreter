 void HWNDMessageHandler::OnExitSizeMove() {
   delegate_->HandleEndWMSizeMove();
   SetMsgHandled(FALSE);
  in_size_move_loop_ = false;
  // Please refer to the notes in the OnSize function for information about
  // the scrolling hack.
  // We hide the Windows scrollbar in the OnEnterSizeMove function. We need
  // to add the scroll styles back to ensure that scrolling works in legacy
  // trackpoint drivers.
  if (needs_scroll_styles_)
    AddScrollStylesToWindow(hwnd());
 }
