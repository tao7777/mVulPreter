bool ChromeOSStopInputMethodProcess(InputMethodStatusConnection* connection) {
  g_return_val_if_fail(connection, false);
  return connection->StopInputMethodProcess();
}
