 DevToolsUI::DevToolsUI(content::WebUI* web_ui)
    : WebUIController(web_ui), bindings_(web_ui->GetWebContents()) {
   web_ui->SetBindings(0);
   Profile* profile = Profile::FromWebUI(web_ui);
   content::URLDataSource::Add(
       profile,
       new DevToolsDataSource(profile->GetRequestContext()));
 
  if (!profile->IsOffTheRecord())
     return;
  GURL url = web_ui->GetWebContents()->GetVisibleURL();
  GURL site = content::SiteInstance::GetSiteForURL(profile, url);
  content::BrowserContext::GetStoragePartitionForSite(profile, site)->
      GetFileSystemContext()->EnableTemporaryFileSystemInIncognito();
 }
