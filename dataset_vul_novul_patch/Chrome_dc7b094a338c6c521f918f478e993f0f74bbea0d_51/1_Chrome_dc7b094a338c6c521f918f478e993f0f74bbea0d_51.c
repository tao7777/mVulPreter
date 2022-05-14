void ChromeOSSetImePropertyActivated(
    InputMethodStatusConnection* connection, const char* key, bool activated) {
  DLOG(INFO) << "SetImePropertyeActivated: " << key << ": " << activated;
  DCHECK(key);
  g_return_if_fail(connection);
  connection->SetImePropertyActivated(key, activated);
}
