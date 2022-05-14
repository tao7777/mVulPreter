void ChromeOSSendHandwritingStroke(InputMethodStatusConnection* connection,
                                   const HandwritingStroke& stroke) {
  g_return_if_fail(connection);
  connection->SendHandwritingStroke(stroke);
 }
