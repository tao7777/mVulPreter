 base::string16 GetApplicationNameForProtocol(const GURL& url) {
   if (base::win::GetVersion() >= base::win::VERSION_WIN8) {
    base::string16 application_name = GetAppForProtocolUsingAssocQuery(url);
     if (!application_name.empty())
       return application_name;
   }

  return GetAppForProtocolUsingRegistry(url);
}
