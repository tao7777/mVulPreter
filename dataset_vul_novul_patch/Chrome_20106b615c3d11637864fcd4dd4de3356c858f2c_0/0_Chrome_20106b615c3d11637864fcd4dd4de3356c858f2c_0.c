void StatusBubbleGtk::InitWidgets() {
  bool ltr = !base::i18n::IsRTL();

  label_ = gtk_label_new(NULL);

  padding_ = gtk_alignment_new(0, 0, 1, 1);
  gtk_alignment_set_padding(GTK_ALIGNMENT(padding_),
      kInternalTopBottomPadding, kInternalTopBottomPadding,
       kInternalLeftRightPadding + (ltr ? 0 : kCornerSize),
       kInternalLeftRightPadding + (ltr ? kCornerSize : 0));
   gtk_container_add(GTK_CONTAINER(padding_), label_);
  gtk_widget_show_all(padding_);
 
   container_.Own(gtk_event_box_new());
   gtk_widget_set_no_show_all(container_.get(), TRUE);
  gtk_util::ActAsRoundedWindow(
      container_.get(), gtk_util::kGdkWhite, kCornerSize,
      gtk_util::ROUNDED_TOP_RIGHT,
      gtk_util::BORDER_TOP | gtk_util::BORDER_RIGHT);
  gtk_widget_set_name(container_.get(), "status-bubble");
  gtk_container_add(GTK_CONTAINER(container_.get()), padding_);

  gtk_widget_add_events(container_.get(), GDK_POINTER_MOTION_MASK |
                                          GDK_ENTER_NOTIFY_MASK);
  g_signal_connect(container_.get(), "motion-notify-event",
                   G_CALLBACK(HandleMotionNotifyThunk), this);
  g_signal_connect(container_.get(), "enter-notify-event",
                   G_CALLBACK(HandleEnterNotifyThunk), this);

  UserChangedTheme();
}
