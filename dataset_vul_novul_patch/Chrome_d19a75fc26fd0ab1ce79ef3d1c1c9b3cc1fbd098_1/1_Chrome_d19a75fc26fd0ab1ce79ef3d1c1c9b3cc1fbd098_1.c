 base::string16 GetApplicationNameForProtocol(const GURL& url) {
  base::string16 application_name;
   if (base::win::GetVersion() >= base::win::VERSION_WIN8) {
    application_name = GetAppForProtocolUsingAssocQuery(url);
     if (!application_name.empty())
       return application_name;
   }

  return GetAppForProtocolUsingRegistry(url);
}
