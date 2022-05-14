ResourceHostMsg_Request CreateXHRRequestWithOrigin(const char* origin) {
   ResourceHostMsg_Request request;
   request.method = "GET";
  request.url = GURL("http://bar.com/simple_page.html");
  request.first_party_for_cookies = GURL(origin);
   request.referrer_policy = blink::WebReferrerPolicyDefault;
  request.headers = base::StringPrintf("Origin: %s\r\n", origin);
   request.load_flags = 0;
   request.origin_pid = 0;
   request.resource_type = RESOURCE_TYPE_XHR;
  request.request_context = 0;
  request.appcache_host_id = kAppCacheNoHostId;
  request.download_to_file = false;
  request.should_reset_appcache = false;
  request.is_main_frame = true;
  request.parent_is_main_frame = false;
  request.parent_render_frame_id = -1;
  request.transition_type = ui::PAGE_TRANSITION_LINK;
  request.allow_download = true;
   return request;
 }
