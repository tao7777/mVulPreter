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
     DCHECK(!url.scheme().empty());
     return GURL(url.scheme() + ":");
   }

  DCHECK(!url.is_valid()) << url;
  return GURL();
}
