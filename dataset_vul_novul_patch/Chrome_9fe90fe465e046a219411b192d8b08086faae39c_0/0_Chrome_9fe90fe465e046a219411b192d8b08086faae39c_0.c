 OmniboxPopupViewGtk::~OmniboxPopupViewGtk() {
  // Stop listening to our signals before we destroy the model. I suspect that
  // we can race window destruction, otherwise.
  signal_registrar_.reset();

  model_.reset();
  g_object_unref(layout_);
  gtk_widget_destroy(window_);

  for (ImageMap::iterator it = images_.begin(); it != images_.end(); ++it)
    delete it->second;
}
