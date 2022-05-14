bool ProxyClientSocket::SanitizeProxyRedirect(HttpResponseInfo* response,
bool ProxyClientSocket::SanitizeProxyAuth(HttpResponseInfo* response) {
  DCHECK(response && response->headers.get());

  scoped_refptr<HttpResponseHeaders> old_headers = response->headers;

  const char kHeaders[] = "HTTP/1.1 407 Proxy Authentication Required\n\n";
  scoped_refptr<HttpResponseHeaders> new_headers = new HttpResponseHeaders(
      HttpUtil::AssembleRawHeaders(kHeaders, arraysize(kHeaders)));

  new_headers->ReplaceStatusLine(old_headers->GetStatusLine());
  CopyHeaderValues(old_headers, new_headers, "Connection");
  CopyHeaderValues(old_headers, new_headers, "Proxy-Authenticate");

  response->headers = new_headers;
  return true;
}

//// static
bool ProxyClientSocket::SanitizeProxyRedirect(HttpResponseInfo* response) {
   DCHECK(response && response->headers.get());
 
   std::string location;
   if (!response->headers->IsRedirect(&location))
     return false;
 
  // Return minimal headers; set "Content-Length: 0" to ignore response body.
  std::string fake_response_headers = base::StringPrintf(
      "HTTP/1.0 302 Found\n"
      "Location: %s\n"
      "Content-Length: 0\n"
      "Connection: close\n"
      "\n",
      location.c_str());
   std::string raw_headers =
       HttpUtil::AssembleRawHeaders(fake_response_headers.data(),
                                    fake_response_headers.length());
  response->headers = new HttpResponseHeaders(raw_headers);

  return true;
}
