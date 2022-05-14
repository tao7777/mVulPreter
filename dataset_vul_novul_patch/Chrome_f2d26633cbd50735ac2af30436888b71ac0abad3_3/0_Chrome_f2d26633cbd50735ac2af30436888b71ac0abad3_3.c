void AutofillPopupBaseView::DoShow() {
  const bool initialize_widget = !GetWidget();
  if (initialize_widget) {
    if (parent_widget_)
      parent_widget_->AddObserver(this);

    views::Widget* widget = new views::Widget;
    views::Widget::InitParams params(views::Widget::InitParams::TYPE_POPUP);
     params.delegate = this;
     params.parent = parent_widget_ ? parent_widget_->GetNativeView()
                                    : delegate_->container_view();
    // Ensure the bubble border is not painted on an opaque background.
    params.opacity = views::Widget::InitParams::TRANSLUCENT_WINDOW;
    params.shadow_type = views::Widget::InitParams::SHADOW_TYPE_NONE;
     widget->Init(params);
     widget->AddObserver(this);
 
    widget->SetVisibilityAnimationTransition(views::Widget::ANIMATE_HIDE);

    show_time_ = base::Time::Now();
  }

  GetWidget()->GetRootView()->SetBorder(CreateBorder());
  DoUpdateBoundsAndRedrawPopup();
  GetWidget()->Show();

  if (initialize_widget)
    views::WidgetFocusManager::GetInstance()->AddFocusChangeListener(this);
}
