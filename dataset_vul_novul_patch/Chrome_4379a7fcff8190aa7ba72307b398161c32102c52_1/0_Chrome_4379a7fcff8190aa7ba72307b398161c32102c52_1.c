NavigationPolicy GetNavigationPolicy(const WebInputEvent* current_event,
}  // anonymous namespace

//// Check that the desired NavigationPolicy |policy| is compatible with the
//// observed input event |current_event|.
NavigationPolicy EffectiveNavigationPolicy(NavigationPolicy policy,
                                           const WebInputEvent* current_event,
                                           const WebWindowFeatures& features) {
   bool as_popup = !features.tool_bar_visible || !features.status_bar_visible ||
                   !features.scrollbars_visible || !features.menu_bar_visible ||
                   !features.resizable;
  NavigationPolicy user_policy =
       as_popup ? kNavigationPolicyNewPopup : kNavigationPolicyNewForegroundTab;
