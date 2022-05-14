RenderProcessHost* RenderProcessHostImpl::GetProcessHostForSite(
    BrowserContext* browser_context,
    const GURL& url) {
   SiteProcessMap* map =
       GetSiteProcessMapForBrowserContext(browser_context);
 
  // See if we have an existing process with appropriate bindings for this site.
  // If not, the caller should create a new process and register it.
   std::string site = SiteInstance::GetSiteForURL(browser_context, url)
       .possibly_invalid_spec();
  RenderProcessHost* host = map->FindProcess(site);
  if (host && !IsSuitableHost(host, browser_context, url)) {
    // The registered process does not have an appropriate set of bindings for
    // the url.  Remove it from the map so we can register a better one.
    RecordAction(UserMetricsAction("BindingsMismatch_GetProcessHostPerSite"));
    map->RemoveProcess(host);
    host = NULL;
  }

  return host;
 }
