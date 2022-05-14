 void HWNDMessageHandler::OnEnterSizeMove() {
  in_size_move_loop_ = true;

  // Please refer to the comments in the OnSize function about the scrollbar
  // hack.
  // Hide the Windows scrollbar if the scroll styles are present to ensure
  // that a paint flicker does not occur while sizing.
  if (needs_scroll_styles_)
    ShowScrollBar(hwnd(), SB_BOTH, FALSE);

   delegate_->HandleBeginWMSizeMove();
   SetMsgHandled(FALSE);
 }
