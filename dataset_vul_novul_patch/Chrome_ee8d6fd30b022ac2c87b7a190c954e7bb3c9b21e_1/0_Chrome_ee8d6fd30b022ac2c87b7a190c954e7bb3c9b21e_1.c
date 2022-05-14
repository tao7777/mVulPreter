void AutomationProvider::WindowGetViewBounds(int handle, int view_id,
                                             bool screen_coordinates,
                                             bool* success,
                                             gfx::Rect* bounds) {
  *success = false;

  GtkWindow* window = window_tracker_->GetResource(handle);
  if (window) {
    GtkWidget* widget = ViewIDUtil::GetWidget(GTK_WIDGET(window),
                                              static_cast<ViewID>(view_id));
     if (!widget)
       return;
     *success = true;
    *bounds = gfx::Rect(widget->allocation.width, widget->allocation.height);
     gint x, y;
     if (screen_coordinates) {
       gfx::Point point = gtk_util::GetWidgetScreenPosition(widget);
      x = point.x();
      y = point.y();
    } else {
      gtk_widget_translate_coordinates(widget, GTK_WIDGET(window),
                                       0, 0, &x, &y);
    }
    bounds->set_origin(gfx::Point(x, y));
  }
}
