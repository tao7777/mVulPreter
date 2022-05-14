void ChromeOSCancelHandwriting(InputMethodStatusConnection* connection,
                               int n_strokes) {
  g_return_if_fail(connection);
  connection->CancelHandwriting(n_strokes);
 }
