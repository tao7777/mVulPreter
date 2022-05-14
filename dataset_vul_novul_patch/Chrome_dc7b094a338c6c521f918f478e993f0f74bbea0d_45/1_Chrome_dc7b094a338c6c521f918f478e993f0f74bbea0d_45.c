bool ChromeOSChangeInputMethod(
    InputMethodStatusConnection* connection, const char* name) {
  DCHECK(name);
  DLOG(INFO) << "ChangeInputMethod: " << name;
  g_return_val_if_fail(connection, false);
  return connection->ChangeInputMethod(name);
}
