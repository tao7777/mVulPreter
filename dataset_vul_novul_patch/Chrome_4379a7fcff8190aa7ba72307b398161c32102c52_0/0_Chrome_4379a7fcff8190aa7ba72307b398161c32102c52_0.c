NavigationPolicy EffectiveNavigationPolicy(NavigationPolicy policy,
   if (policy == kNavigationPolicyNewBackgroundTab &&
      user_policy != kNavigationPolicyNewBackgroundTab &&
       !UIEventWithKeyState::NewTabModifierSetFromIsolatedWorld()) {
    // Don't allow background tabs to be opened via script setting the
    // event modifiers.
     return kNavigationPolicyNewForegroundTab;
   }

  if (policy == kNavigationPolicyDownload &&
      user_policy != kNavigationPolicyDownload) {
    // Don't allow downloads to be triggered via script setting the event
    // modifiers.
    return kNavigationPolicyNewForegroundTab;
  }

   return policy;
 }
