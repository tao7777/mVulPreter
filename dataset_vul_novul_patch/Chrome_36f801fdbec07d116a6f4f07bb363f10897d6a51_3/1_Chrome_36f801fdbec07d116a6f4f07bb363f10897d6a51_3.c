void WebViewTestClient::DidFocus() {
   test_runner()->SetFocus(web_view_test_proxy_base_->web_view(), true);
 }
