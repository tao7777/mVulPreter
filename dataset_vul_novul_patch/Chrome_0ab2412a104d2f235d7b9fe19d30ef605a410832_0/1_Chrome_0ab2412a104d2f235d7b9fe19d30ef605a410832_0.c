void Document::InitSecurityContext(const DocumentInit& initializer) {
  DCHECK(!GetSecurityOrigin());

  if (!initializer.HasSecurityContext()) {
    cookie_url_ = KURL(kParsedURLString, g_empty_string);
    SetSecurityOrigin(SecurityOrigin::CreateUnique());
    InitContentSecurityPolicy();
    SetFeaturePolicy(g_empty_string);
    return;
  }

  EnforceSandboxFlags(initializer.GetSandboxFlags());
  SetInsecureRequestPolicy(initializer.GetInsecureRequestPolicy());
  if (initializer.InsecureNavigationsToUpgrade()) {
    for (auto to_upgrade : *initializer.InsecureNavigationsToUpgrade())
       AddInsecureNavigationUpgrade(to_upgrade);
   }
 
   if (IsSandboxed(kSandboxOrigin)) {
     cookie_url_ = url_;
     SetSecurityOrigin(SecurityOrigin::CreateUnique());
    Document* owner = initializer.OwnerDocument();
    if (owner) {
      if (owner->GetSecurityOrigin()->IsPotentiallyTrustworthy())
         GetSecurityOrigin()->SetUniqueOriginIsPotentiallyTrustworthy(true);
       if (owner->GetSecurityOrigin()->CanLoadLocalResources())
         GetSecurityOrigin()->GrantLoadLocalResources();
     }
   } else if (Document* owner = initializer.OwnerDocument()) {
     cookie_url_ = owner->CookieURL();
     SetSecurityOrigin(owner->GetSecurityOrigin());
   } else {
     cookie_url_ = url_;
     SetSecurityOrigin(SecurityOrigin::Create(url_));
  }

  if (initializer.IsHostedInReservedIPRange()) {
    SetAddressSpace(GetSecurityOrigin()->IsLocalhost()
                        ? kWebAddressSpaceLocal
                        : kWebAddressSpacePrivate);
  } else if (GetSecurityOrigin()->IsLocal()) {
    SetAddressSpace(kWebAddressSpaceLocal);
  } else {
    SetAddressSpace(kWebAddressSpacePublic);
  }

  if (ImportsController()) {
     SetContentSecurityPolicy(
         ImportsController()->Master()->GetContentSecurityPolicy());
   } else {
    InitContentSecurityPolicy();
   }
 
   if (GetSecurityOrigin()->HasSuborigin())
    EnforceSuborigin(*GetSecurityOrigin()->GetSuborigin());

  if (Settings* settings = initializer.GetSettings()) {
    if (!settings->GetWebSecurityEnabled()) {
      GetSecurityOrigin()->GrantUniversalAccess();
    } else if (GetSecurityOrigin()->IsLocal()) {
      if (settings->GetAllowUniversalAccessFromFileURLs()) {
        GetSecurityOrigin()->GrantUniversalAccess();
      } else if (!settings->GetAllowFileAccessFromFileURLs()) {
        GetSecurityOrigin()->BlockLocalAccessFromLocalOrigin();
      }
    }
  }

  if (GetSecurityOrigin()->IsUnique() &&
      SecurityOrigin::Create(url_)->IsPotentiallyTrustworthy())
    GetSecurityOrigin()->SetUniqueOriginIsPotentiallyTrustworthy(true);

  if (GetSecurityOrigin()->HasSuborigin())
    EnforceSuborigin(*GetSecurityOrigin()->GetSuborigin());

   SetFeaturePolicy(g_empty_string);
 }
