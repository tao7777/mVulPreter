NavigationPolicy EffectiveNavigationPolicy(NavigationPolicy policy,
                                           const WebInputEvent* current_event,
                                           const WebWindowFeatures& features) {
  if (policy == kNavigationPolicyIgnore)
    return GetNavigationPolicy(current_event, features);
   if (policy == kNavigationPolicyNewBackgroundTab &&
      GetNavigationPolicy(current_event, features) !=
          kNavigationPolicyNewBackgroundTab &&
       !UIEventWithKeyState::NewTabModifierSetFromIsolatedWorld()) {
     return kNavigationPolicyNewForegroundTab;
   }
   return policy;
 }
