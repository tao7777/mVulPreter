void WebViewTestClient::DidFocus() {
void WebViewTestClient::DidFocus(blink::WebLocalFrame* calling_frame) {
   test_runner()->SetFocus(web_view_test_proxy_base_->web_view(), true);
 }
