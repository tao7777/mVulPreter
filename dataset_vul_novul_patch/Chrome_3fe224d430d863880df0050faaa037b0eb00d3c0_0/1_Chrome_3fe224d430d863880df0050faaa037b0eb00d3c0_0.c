   static aura::Window* OpenTestWindow(aura::Window* parent, bool modal) {
    DCHECK(!modal || (modal && parent));
     views::Widget* widget =
         views::Widget::CreateWindowWithParent(new TestWindow(modal), parent);
     widget->Show();
    return widget->GetNativeView();
  }
