 base::string16 GetAppForProtocolUsingAssocQuery(const GURL& url) {
  const base::string16 url_scheme = base::ASCIIToUTF16(url.scheme());
  if (!IsValidCustomProtocol(url_scheme))
     return base::string16();
 
   wchar_t out_buffer[1024];
   DWORD buffer_size = arraysize(out_buffer);
  HRESULT hr =
      AssocQueryString(ASSOCF_IS_PROTOCOL, ASSOCSTR_FRIENDLYAPPNAME,
                       url_scheme.c_str(), NULL, out_buffer, &buffer_size);
   if (FAILED(hr)) {
     DLOG(WARNING) << "AssocQueryString failed!";
     return base::string16();
  }
  return base::string16(out_buffer);
 }
