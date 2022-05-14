 OmniboxPopupViewGtk::~OmniboxPopupViewGtk() {
  model_.reset();
  g_object_unref(layout_);
  gtk_widget_destroy(window_);

  for (ImageMap::iterator it = images_.begin(); it != images_.end(); ++it)
    delete it->second;
}
