 views::NonClientFrameView* ShellWindowViews::CreateNonClientFrameView(
     views::Widget* widget) {
  ShellWindowFrameView* frame_view = new ShellWindowFrameView();
   frame_view->Init(window_);
   return frame_view;
 }
