bool IsTraceEventArgsWhitelisted(const char* category_group_name,
bool IsTraceEventArgsWhitelisted(
    const char* category_group_name,
    const char* event_name,
    base::trace_event::ArgumentNameFilterPredicate* arg_filter) {
   if (base::MatchPattern(category_group_name, "benchmark") &&
       base::MatchPattern(event_name, "whitelisted")) {
     return true;
  }

  return false;
}
