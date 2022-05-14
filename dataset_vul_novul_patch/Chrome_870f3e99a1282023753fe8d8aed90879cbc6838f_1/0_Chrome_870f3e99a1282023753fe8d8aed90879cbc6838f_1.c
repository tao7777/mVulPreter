bool IsTraceEventArgsWhitelisted(const char* category_group_name,
bool IsTraceArgumentNameWhitelisted(const char* const* granular_filter,
                                    const char* arg_name) {
  for (int i = 0; granular_filter[i] != nullptr; ++i) {
    if (base::MatchPattern(arg_name, granular_filter[i]))
      return true;
  }

  return false;
}

bool IsTraceEventArgsWhitelisted(
    const char* category_group_name,
    const char* event_name,
    base::trace_event::ArgumentNameFilterPredicate* arg_name_filter) {
  DCHECK(arg_name_filter);
   base::CStringTokenizer category_group_tokens(
       category_group_name, category_group_name + strlen(category_group_name),
       ",");
   while (category_group_tokens.GetNext()) {
     const std::string& category_group_token = category_group_tokens.token();
    for (int i = 0; kEventArgsWhitelist[i].category_name != nullptr; ++i) {
      const WhitelistEntry& whitelist_entry = kEventArgsWhitelist[i];
      DCHECK(whitelist_entry.event_name);
 
       if (base::MatchPattern(category_group_token.c_str(),
                             whitelist_entry.category_name) &&
          base::MatchPattern(event_name, whitelist_entry.event_name)) {
        if (whitelist_entry.arg_name_filter) {
          *arg_name_filter = base::Bind(&IsTraceArgumentNameWhitelisted,
                                        whitelist_entry.arg_name_filter);
        }
         return true;
       }
     }
  }

  return false;
}
