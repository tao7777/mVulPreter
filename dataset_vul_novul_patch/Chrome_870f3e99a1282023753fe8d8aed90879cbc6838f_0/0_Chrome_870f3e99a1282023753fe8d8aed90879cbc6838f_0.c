 bool IsTraceEventArgsWhitelisted(const char* category_group_name,
                                 const char* event_name,
                                 ArgumentNameFilterPredicate* arg_filter) {
   if (base::MatchPattern(category_group_name, "toplevel") &&
       base::MatchPattern(event_name, "*")) {
     return true;
   }
 
  if (base::MatchPattern(category_group_name, "benchmark") &&
      base::MatchPattern(event_name, "granularly_whitelisted")) {
    *arg_filter = base::Bind(&IsArgNameWhitelisted);
    return true;
  }

   return false;
 }
