NavigateParams::NavigateParams(
    Browser* a_browser,
    const GURL& a_url,
    content::PageTransition a_transition)
    : url(a_url),
      target_contents(NULL),
       source_contents(NULL),
       disposition(CURRENT_TAB),
       transition(a_transition),
      is_renderer_initiated(false),
       tabstrip_index(-1),
       tabstrip_add_types(TabStripModel::ADD_ACTIVE),
       window_action(NO_ACTION),
      user_gesture(true),
      path_behavior(RESPECT),
      ref_behavior(IGNORE_REF),
      browser(a_browser),
      profile(NULL) {
}
