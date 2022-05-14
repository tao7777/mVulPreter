GURL SiteInstance::GetSiteForURL(BrowserContext* browser_context,
                                 const GURL& real_url) {
  if (real_url.SchemeIs(kGuestScheme))
    return real_url;

  GURL url = SiteInstanceImpl::GetEffectiveURL(browser_context, real_url);
  url::Origin origin = url::Origin::Create(url);

  auto* policy = ChildProcessSecurityPolicyImpl::GetInstance();
  url::Origin isolated_origin;
  if (policy->GetMatchingIsolatedOrigin(origin, &isolated_origin))
    return isolated_origin.GetURL();

  if (!origin.host().empty() && origin.scheme() != url::kFileScheme) {
    std::string domain = net::registry_controlled_domains::GetDomainAndRegistry(
        origin.host(),
        net::registry_controlled_domains::INCLUDE_PRIVATE_REGISTRIES);
    std::string site = origin.scheme();
    site += url::kStandardSchemeSeparator;
    site += domain.empty() ? origin.host() : domain;
    return GURL(site);
  }

  if (!origin.unique()) {
    DCHECK(!origin.scheme().empty());
     return GURL(origin.scheme() + ":");
   } else if (url.has_scheme()) {
    // that might allow two URLs created by different sites to share a process.
    // See https://crbug.com/863623 and https://crbug.com/863069.
    // schemes, such as file:.
    // TODO(creis): This currently causes problems with tests on Android and
    // Android WebView.  For now, skip it when Site Isolation is not enabled,
    // since there's no need to isolate data and blob URLs from each other in
    // that case.
    bool is_site_isolation_enabled =
        SiteIsolationPolicy::UseDedicatedProcessesForAllSites() ||
        SiteIsolationPolicy::AreIsolatedOriginsEnabled();
    if (is_site_isolation_enabled &&
        (url.SchemeIsBlob() || url.scheme() == url::kDataScheme)) {
      // origins from each other.  We also get here for browser-initiated
      // navigations to data URLs, which have a unique origin and should only
      // share a process when they are identical.  Remove hash from the URL in
      // either case, since same-document navigations shouldn't use a different
      // site URL.
       if (url.has_ref()) {
         GURL::Replacements replacements;
         replacements.ClearRef();
        url = url.ReplaceComponents(replacements);
      }
      return url;
    }

    DCHECK(!url.scheme().empty());
    return GURL(url.scheme() + ":");
  }

  DCHECK(!url.is_valid()) << url;
  return GURL();
}
