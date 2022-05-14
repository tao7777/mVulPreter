void Document::InitSecurityContext(const DocumentInit& initializer) {
  DCHECK(!GetSecurityOrigin());

  if (!initializer.HasSecurityContext()) {
    cookie_url_ = KURL(g_empty_string);
    SetSecurityOrigin(SecurityOrigin::CreateUniqueOpaque());
    InitContentSecurityPolicy();
    ApplyFeaturePolicy({});
    return;
  }

  SandboxFlags sandbox_flags = initializer.GetSandboxFlags();
  if (fetcher_->Archive()) {
    sandbox_flags |=
        kSandboxAll &
        ~(kSandboxPopups | kSandboxPropagatesToAuxiliaryBrowsingContexts);
  }
  EnforceSandboxFlags(sandbox_flags);
  SetInsecureRequestPolicy(initializer.GetInsecureRequestPolicy());
  if (initializer.InsecureNavigationsToUpgrade()) {
    for (auto to_upgrade : *initializer.InsecureNavigationsToUpgrade())
       AddInsecureNavigationUpgrade(to_upgrade);
   }
 
  const ContentSecurityPolicy* policy_to_inherit = nullptr;
 
   if (IsSandboxed(kSandboxOrigin)) {
     cookie_url_ = url_;
    scoped_refptr<SecurityOrigin> security_origin =
        SecurityOrigin::CreateUniqueOpaque();
    Document* owner = initializer.OwnerDocument();
    if (owner) {
      if (owner->GetSecurityOrigin()->IsPotentiallyTrustworthy())
         security_origin->SetOpaqueOriginIsPotentiallyTrustworthy(true);
       if (owner->GetSecurityOrigin()->CanLoadLocalResources())
         security_origin->GrantLoadLocalResources();
      policy_to_inherit = owner->GetContentSecurityPolicy();
     }
     SetSecurityOrigin(std::move(security_origin));
   } else if (Document* owner = initializer.OwnerDocument()) {
     cookie_url_ = owner->CookieURL();
     SetSecurityOrigin(owner->GetMutableSecurityOrigin());
    policy_to_inherit = owner->GetContentSecurityPolicy();
   } else {
     cookie_url_ = url_;
     SetSecurityOrigin(SecurityOrigin::Create(url_));
  }

  if (initializer.IsHostedInReservedIPRange()) {
    SetAddressSpace(GetSecurityOrigin()->IsLocalhost()
                        ? mojom::IPAddressSpace::kLocal
                        : mojom::IPAddressSpace::kPrivate);
  } else if (GetSecurityOrigin()->IsLocal()) {
    SetAddressSpace(mojom::IPAddressSpace::kLocal);
  } else {
    SetAddressSpace(mojom::IPAddressSpace::kPublic);
  }

  if (ImportsController()) {
     SetContentSecurityPolicy(
         ImportsController()->Master()->GetContentSecurityPolicy());
   } else {
    InitContentSecurityPolicy(nullptr, policy_to_inherit,
                              initializer.PreviousDocumentCSP());
   }
 
   if (Settings* settings = initializer.GetSettings()) {
    if (!settings->GetWebSecurityEnabled()) {
      GetMutableSecurityOrigin()->GrantUniversalAccess();
    } else if (GetSecurityOrigin()->IsLocal()) {
      if (settings->GetAllowUniversalAccessFromFileURLs()) {
        GetMutableSecurityOrigin()->GrantUniversalAccess();
      } else if (!settings->GetAllowFileAccessFromFileURLs()) {
        GetMutableSecurityOrigin()->BlockLocalAccessFromLocalOrigin();
      }
    }
  }

  if (GetSecurityOrigin()->IsOpaque() &&
      SecurityOrigin::Create(url_)->IsPotentiallyTrustworthy())
    GetMutableSecurityOrigin()->SetOpaqueOriginIsPotentiallyTrustworthy(true);

  ApplyFeaturePolicy({});

  InitSecureContextState();
}
