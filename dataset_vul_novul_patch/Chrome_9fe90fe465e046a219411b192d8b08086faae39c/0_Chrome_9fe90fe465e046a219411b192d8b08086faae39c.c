OmniboxPopupViewGtk::OmniboxPopupViewGtk(const gfx::Font& font,
                                          OmniboxView* omnibox_view,
                                          AutocompleteEditModel* edit_model,
                                          GtkWidget* location_bar)
    : signal_registrar_(new ui::GtkSignalRegistrar),
      model_(new AutocompletePopupModel(this, edit_model)),
       omnibox_view_(omnibox_view),
       location_bar_(location_bar),
       window_(gtk_window_new(GTK_WINDOW_POPUP)),
      layout_(NULL),
      theme_service_(ThemeServiceGtk::GetFrom(edit_model->profile())),
      font_(font.DeriveFont(kEditFontAdjust)),
      ignore_mouse_drag_(false),
      opened_(false) {
  gtk_widget_set_can_focus(window_, FALSE);
  gtk_window_set_resizable(GTK_WINDOW(window_), FALSE);
  gtk_widget_set_app_paintable(window_, TRUE);
  gtk_widget_set_double_buffered(window_, TRUE);

  layout_ = gtk_widget_create_pango_layout(window_, NULL);
  pango_layout_set_auto_dir(layout_, FALSE);
  pango_layout_set_ellipsize(layout_, PANGO_ELLIPSIZE_END);

  gtk_widget_add_events(window_, GDK_BUTTON_MOTION_MASK |
                                  GDK_POINTER_MOTION_MASK |
                                  GDK_BUTTON_PRESS_MASK |
                                  GDK_BUTTON_RELEASE_MASK);
  signal_registrar_->Connect(window_, "motion-notify-event",
                             G_CALLBACK(HandleMotionThunk), this);
  signal_registrar_->Connect(window_, "button-press-event",
                             G_CALLBACK(HandleButtonPressThunk), this);
  signal_registrar_->Connect(window_, "button-release-event",
                             G_CALLBACK(HandleButtonReleaseThunk), this);
  signal_registrar_->Connect(window_, "expose-event",
                             G_CALLBACK(HandleExposeThunk), this);
 
   registrar_.Add(this,
                  chrome::NOTIFICATION_BROWSER_THEME_CHANGED,
                 content::Source<ThemeService>(theme_service_));
  theme_service_->InitThemesFor(this);

 }
