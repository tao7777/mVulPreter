bool ProxyClientSocket::SanitizeProxyRedirect(HttpResponseInfo* response,
                                              const GURL& url) {
//// static
   DCHECK(response && response->headers.get());
 
   std::string location;
   if (!response->headers->IsRedirect(&location))
     return false;
 
  std::string fake_response_headers =
      base::StringPrintf("HTTP/1.0 302 Found\n"
                         "Location: %s\n"
                         "Content-length: 0\n"
                         "Connection: close\n"
                         "\n",
                         location.c_str());
   std::string raw_headers =
       HttpUtil::AssembleRawHeaders(fake_response_headers.data(),
                                    fake_response_headers.length());
  response->headers = new HttpResponseHeaders(raw_headers);

  return true;
}
