 views::NonClientFrameView* ShellWindowViews::CreateNonClientFrameView(
     views::Widget* widget) {
  ShellWindowFrameView* frame_view =
      new ShellWindowFrameView(use_custom_frame_);
   frame_view->Init(window_);
   return frame_view;
 }
