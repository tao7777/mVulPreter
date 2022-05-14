void ExtensionInstalledBubbleGtk::ShowInternal() {
  BrowserWindowGtk* browser_window =
      BrowserWindowGtk::GetBrowserWindowForNativeWindow(
          browser_->window()->GetNativeHandle());

  GtkWidget* reference_widget = NULL;

  if (type_ == BROWSER_ACTION) {
    BrowserActionsToolbarGtk* toolbar =
        browser_window->GetToolbar()->GetBrowserActionsToolbar();

    if (toolbar->animating() && animation_wait_retries_-- > 0) {
      MessageLoopForUI::current()->PostDelayedTask(
          FROM_HERE,
          base::Bind(&ExtensionInstalledBubbleGtk::ShowInternal, this),
          base::TimeDelta::FromMilliseconds(kAnimationWaitMS));
      return;
    }

    reference_widget = toolbar->GetBrowserActionWidget(extension_);
    gtk_container_check_resize(GTK_CONTAINER(
        browser_window->GetToolbar()->widget()));
    if (reference_widget && !gtk_widget_get_visible(reference_widget)) {
      reference_widget = gtk_widget_get_visible(toolbar->chevron()) ?
          toolbar->chevron() : NULL;
    }
  } else if (type_ == PAGE_ACTION) {
    LocationBarViewGtk* location_bar_view =
        browser_window->GetToolbar()->GetLocationBarView();
    location_bar_view->SetPreviewEnabledPageAction(extension_->page_action(),
                                                   true);  // preview_enabled
    reference_widget = location_bar_view->GetPageActionWidget(
        extension_->page_action());
    gtk_container_check_resize(GTK_CONTAINER(
        browser_window->GetToolbar()->widget()));
    DCHECK(reference_widget);
  } else if (type_ == OMNIBOX_KEYWORD) {
    LocationBarViewGtk* location_bar_view =
        browser_window->GetToolbar()->GetLocationBarView();
    reference_widget = location_bar_view->location_entry_widget();
    DCHECK(reference_widget);
  }

  if (reference_widget == NULL)
    reference_widget = browser_window->GetToolbar()->GetAppMenuButton();

  GtkThemeService* theme_provider = GtkThemeService::GetFrom(
      browser_->profile());

  GtkWidget* bubble_content = gtk_hbox_new(FALSE, kHorizontalColumnSpacing);
  gtk_container_set_border_width(GTK_CONTAINER(bubble_content), kContentBorder);

  if (!icon_.isNull()) {
    GdkPixbuf* pixbuf = gfx::GdkPixbufFromSkBitmap(&icon_);
    gfx::Size size(icon_.width(), icon_.height());
    if (size.width() > kIconSize || size.height() > kIconSize) {
      if (size.width() > size.height()) {
        size.set_height(size.height() * kIconSize / size.width());
        size.set_width(kIconSize);
      } else {
        size.set_width(size.width() * kIconSize / size.height());
        size.set_height(kIconSize);
      }

      GdkPixbuf* old = pixbuf;
      pixbuf = gdk_pixbuf_scale_simple(pixbuf, size.width(), size.height(),
                                       GDK_INTERP_BILINEAR);
      g_object_unref(old);
    }

    GtkWidget* icon_column = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bubble_content), icon_column, FALSE, FALSE,
                       kIconPadding);
    GtkWidget* image = gtk_image_new_from_pixbuf(pixbuf);
    g_object_unref(pixbuf);
    gtk_box_pack_start(GTK_BOX(icon_column), image, FALSE, FALSE, 0);
  }

  GtkWidget* text_column = gtk_vbox_new(FALSE, kTextColumnVerticalSpacing);
  gtk_box_pack_start(GTK_BOX(bubble_content), text_column, FALSE, FALSE, 0);

  GtkWidget* heading_label = gtk_label_new(NULL);
   string16 extension_name = UTF8ToUTF16(extension_->name());
   base::i18n::AdjustStringForLocaleDirection(&extension_name);
   std::string heading_text = l10n_util::GetStringFUTF8(
      IDS_EXTENSION_INSTALLED_HEADING, extension_name,
      l10n_util::GetStringUTF16(IDS_SHORT_PRODUCT_NAME));
   char* markup = g_markup_printf_escaped("<span size=\"larger\">%s</span>",
       heading_text.c_str());
   gtk_label_set_markup(GTK_LABEL(heading_label), markup);
  g_free(markup);

  gtk_util::SetLabelWidth(heading_label, kTextColumnWidth);
  gtk_box_pack_start(GTK_BOX(text_column), heading_label, FALSE, FALSE, 0);

  if (type_ == PAGE_ACTION) {
    GtkWidget* info_label = gtk_label_new(l10n_util::GetStringUTF8(
        IDS_EXTENSION_INSTALLED_PAGE_ACTION_INFO).c_str());
    gtk_util::SetLabelWidth(info_label, kTextColumnWidth);
    gtk_box_pack_start(GTK_BOX(text_column), info_label, FALSE, FALSE, 0);
  }

  if (type_ == OMNIBOX_KEYWORD) {
    GtkWidget* info_label = gtk_label_new(l10n_util::GetStringFUTF8(
        IDS_EXTENSION_INSTALLED_OMNIBOX_KEYWORD_INFO,
        UTF8ToUTF16(extension_->omnibox_keyword())).c_str());
    gtk_util::SetLabelWidth(info_label, kTextColumnWidth);
    gtk_box_pack_start(GTK_BOX(text_column), info_label, FALSE, FALSE, 0);
  }

  GtkWidget* manage_label = gtk_label_new(
      l10n_util::GetStringUTF8(IDS_EXTENSION_INSTALLED_MANAGE_INFO).c_str());
  gtk_util::SetLabelWidth(manage_label, kTextColumnWidth);
  gtk_box_pack_start(GTK_BOX(text_column), manage_label, FALSE, FALSE, 0);

  GtkWidget* close_column = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(bubble_content), close_column, FALSE, FALSE, 0);
  close_button_.reset(CustomDrawButton::CloseButton(theme_provider));
  g_signal_connect(close_button_->widget(), "clicked",
                   G_CALLBACK(OnButtonClick), this);
  gtk_box_pack_start(GTK_BOX(close_column), close_button_->widget(),
      FALSE, FALSE, 0);

  BubbleGtk::ArrowLocationGtk arrow_location =
      !base::i18n::IsRTL() ?
      BubbleGtk::ARROW_LOCATION_TOP_RIGHT :
      BubbleGtk::ARROW_LOCATION_TOP_LEFT;

  gfx::Rect bounds = gtk_util::WidgetBounds(reference_widget);
  if (type_ == OMNIBOX_KEYWORD) {
    arrow_location =
        !base::i18n::IsRTL() ?
        BubbleGtk::ARROW_LOCATION_TOP_LEFT :
        BubbleGtk::ARROW_LOCATION_TOP_RIGHT;
    if (base::i18n::IsRTL())
      bounds.Offset(bounds.width(), 0);
    bounds.set_width(0);
  }

  bubble_ = BubbleGtk::Show(reference_widget,
                            &bounds,
                            bubble_content,
                            arrow_location,
                            true,  // match_system_theme
                            true,  // grab_input
                            theme_provider,
                            this);
}
