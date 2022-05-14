NavigateParams::NavigateParams(Browser* a_browser,
                               TabContentsWrapper* a_target_contents)
    : target_contents(a_target_contents),
       source_contents(NULL),
       disposition(CURRENT_TAB),
       transition(content::PAGE_TRANSITION_LINK),
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
