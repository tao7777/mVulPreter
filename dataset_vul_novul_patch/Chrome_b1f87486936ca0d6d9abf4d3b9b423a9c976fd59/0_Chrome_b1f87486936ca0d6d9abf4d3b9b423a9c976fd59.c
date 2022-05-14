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
    // cover blob:file: and filesystem:file: URIs (see also
    // https://crbug.com/697111).
     DCHECK(!origin.scheme().empty());
     return GURL(origin.scheme() + ":");
   } else if (url.has_scheme()) {
    // In some cases, it is not safe to use just the scheme as a site URL, as
    // that might allow two URLs created by different sites to to share a
    // process.  See https://crbug.com/863623.
    //
    // TODO(alexmos,creis): This should eventually be expanded to certain other
    // schemes, such as data: and file:.
    if (url.SchemeIsBlob()) {
      // We get here for blob URLs of form blob:null/guid.  Use the full URL
      // with the guid in that case, which isolates all blob URLs with unique
      // origins from each other.  Remove hash from the URL, since
      // same-document navigations shouldn't use a different site URL.
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
