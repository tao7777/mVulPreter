void HWNDMessageHandler::OnSize(UINT param, const CSize& size) {
  RedrawWindow(hwnd(), NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
  ResetWindowRegion(false, true);

#if defined(USE_AURA)
  // We want the scroll styles to be present on the window. However we don't
  // want Windows to draw the scrollbars. To achieve this we hide the scroll
  // bars and readd them to the window style in a posted task to ensure that we
  // don't get nested WM_SIZE messages.
  if (needs_scroll_styles_ && !in_size_move_loop_) {
     ShowScrollBar(hwnd(), SB_BOTH, FALSE);
     base::MessageLoop::current()->PostTask(
        FROM_HERE, base::Bind(&AddScrollStylesToWindow, hwnd()));
   }
#endif
 }
