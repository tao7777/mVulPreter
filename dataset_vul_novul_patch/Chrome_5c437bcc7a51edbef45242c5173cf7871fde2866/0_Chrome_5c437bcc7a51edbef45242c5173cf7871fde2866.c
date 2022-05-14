bool WebRequestPermissions::CanExtensionAccessURL(
    const extensions::InfoMap* extension_info_map,
    const std::string& extension_id,
    const GURL& url,
    bool crosses_incognito,
    HostPermissionsCheck host_permissions_check) {
  if (!extension_info_map)
    return true;

  const extensions::Extension* extension =
      extension_info_map->extensions().GetByID(extension_id);
  if (!extension)
    return false;

  if (crosses_incognito && !extension_info_map->CanCrossIncognito(extension))
    return false;

  switch (host_permissions_check) {
    case DO_NOT_CHECK_HOST:
      break;
     case REQUIRE_HOST_PERMISSION:
      if (!url.SchemeIs(url::kAboutScheme) &&
          !extension->permissions_data()->HasHostPermission(url) &&
          !url::IsSameOriginWith(url, extension->url())) {
         return false;
       }
       break;
    case REQUIRE_ALL_URLS:
      if (!extension->permissions_data()->HasEffectiveAccessToAllHosts())
        return false;
      break;
  }

  return true;
}
