  void GrantActiveTab(const GURL& url) {
     APIPermissionSet tab_api_permissions;
     tab_api_permissions.insert(APIPermission::kTab);
     URLPatternSet tab_hosts;
    tab_hosts.AddOrigin(UserScript::ValidUserScriptSchemes(),
                        url::Origin::Create(url).GetURL());
     PermissionSet tab_permissions(std::move(tab_api_permissions),
                                   ManifestPermissionSet(), tab_hosts,
                                   tab_hosts);
    active_tab_->permissions_data()->UpdateTabSpecificPermissions(
        kTabId, tab_permissions);
   }
