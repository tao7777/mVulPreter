 void RenderProcessHostImpl::RegisterProcessHostForSite(
    BrowserContext* browser_context,
    RenderProcessHost* process,
    const GURL& url) {
   SiteProcessMap* map =
       GetSiteProcessMapForBrowserContext(browser_context);
 
  // Only register valid, non-empty sites.  Empty or invalid sites will not
  // use process-per-site mode.  We cannot check whether the process has
  // appropriate bindings here, because the bindings have not yet been granted.
   std::string site = SiteInstance::GetSiteForURL(browser_context, url)
       .possibly_invalid_spec();
  if (!site.empty())
    map->RegisterProcess(site, process);
 }
