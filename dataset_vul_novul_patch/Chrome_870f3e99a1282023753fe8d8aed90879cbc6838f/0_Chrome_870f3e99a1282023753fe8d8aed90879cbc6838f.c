void TraceEvent::AppendAsJSON(
    std::string* out,
    const ArgumentFilterPredicate& argument_filter_predicate) const {
  int64 time_int64 = timestamp_.ToInternalValue();
  int process_id = TraceLog::GetInstance()->process_id();
  const char* category_group_name =
      TraceLog::GetCategoryGroupName(category_group_enabled_);

  DCHECK(!strchr(name_, '"'));
  StringAppendF(out, "{\"pid\":%i,\"tid\":%i,\"ts\":%" PRId64
                     ","
                     "\"ph\":\"%c\",\"cat\":\"%s\",\"name\":\"%s\",\"args\":",
                process_id, thread_id_, time_int64, phase_, category_group_name,
                 name_);
 
  // TODO(oysteine): The dual predicates here is a bit ugly; if the filtering
  // capabilities need to grow even more precise we should rethink this
  // approach
  ArgumentNameFilterPredicate argument_name_filter_predicate;
  bool strip_args =
      arg_names_[0] && !argument_filter_predicate.is_null() &&
      !argument_filter_predicate.Run(category_group_name, name_,
                                     &argument_name_filter_predicate);
 
   if (strip_args) {
     *out += "\"__stripped__\"";
  } else {
    *out += "{";

    for (int i = 0; i < kTraceMaxNumArgs && arg_names_[i]; ++i) {
      if (i > 0)
        *out += ",";
      *out += "\"";
       *out += arg_names_[i];
       *out += "\":";
 
      if (argument_name_filter_predicate.is_null() ||
          argument_name_filter_predicate.Run(arg_names_[i])) {
        if (arg_types_[i] == TRACE_VALUE_TYPE_CONVERTABLE)
          convertable_values_[i]->AppendAsTraceFormat(out);
        else
          AppendValueAsJSON(arg_types_[i], arg_values_[i], out);
      } else {
        *out += "\"__stripped__\"";
      }
     }
 
     *out += "}";
  }

  if (phase_ == TRACE_EVENT_PHASE_COMPLETE) {
    int64 duration = duration_.ToInternalValue();
    if (duration != -1)
      StringAppendF(out, ",\"dur\":%" PRId64, duration);
    if (!thread_timestamp_.is_null()) {
      int64 thread_duration = thread_duration_.ToInternalValue();
      if (thread_duration != -1)
        StringAppendF(out, ",\"tdur\":%" PRId64, thread_duration);
    }
  }

  if (!thread_timestamp_.is_null()) {
    int64 thread_time_int64 = thread_timestamp_.ToInternalValue();
    StringAppendF(out, ",\"tts\":%" PRId64, thread_time_int64);
  }

  if (flags_ & TRACE_EVENT_FLAG_ASYNC_TTS) {
    StringAppendF(out, ", \"use_async_tts\":1");
  }

  if (flags_ & TRACE_EVENT_FLAG_HAS_ID)
    StringAppendF(out, ",\"id\":\"0x%" PRIx64 "\"", static_cast<uint64>(id_));

  if (flags_ & TRACE_EVENT_FLAG_BIND_TO_ENCLOSING)
    StringAppendF(out, ",\"bp\":\"e\"");

  if ((flags_ & TRACE_EVENT_FLAG_FLOW_OUT) ||
      (flags_ & TRACE_EVENT_FLAG_FLOW_IN)) {
    StringAppendF(out, ",\"bind_id\":\"0x%" PRIx64 "\"",
                  static_cast<uint64>(bind_id_));
  }
  if (flags_ & TRACE_EVENT_FLAG_FLOW_IN)
    StringAppendF(out, ",\"flow_in\":true");
  if (flags_ & TRACE_EVENT_FLAG_FLOW_OUT)
    StringAppendF(out, ",\"flow_out\":true");

  if (flags_ & TRACE_EVENT_FLAG_HAS_CONTEXT_ID)
    StringAppendF(out, ",\"cid\":\"0x%" PRIx64 "\"",
                  static_cast<uint64>(context_id_));

  if (phase_ == TRACE_EVENT_PHASE_INSTANT) {
    char scope = '?';
    switch (flags_ & TRACE_EVENT_FLAG_SCOPE_MASK) {
      case TRACE_EVENT_SCOPE_GLOBAL:
        scope = TRACE_EVENT_SCOPE_NAME_GLOBAL;
        break;

      case TRACE_EVENT_SCOPE_PROCESS:
        scope = TRACE_EVENT_SCOPE_NAME_PROCESS;
        break;

      case TRACE_EVENT_SCOPE_THREAD:
        scope = TRACE_EVENT_SCOPE_NAME_THREAD;
        break;
    }
    StringAppendF(out, ",\"s\":\"%c\"", scope);
  }

  *out += "}";
}
