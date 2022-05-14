bool Performance::PassesTimingAllowCheck(
    const ResourceResponse& response,
     const SecurityOrigin& initiator_security_origin,
     const AtomicString& original_timing_allow_origin,
     ExecutionContext* context) {
  const KURL& response_url = response.WasFetchedViaServiceWorker()
                                 ? response.OriginalURLViaServiceWorker()
                                 : response.Url();
   scoped_refptr<const SecurityOrigin> resource_origin =
      SecurityOrigin::Create(response_url);
   if (resource_origin->IsSameSchemeHostPort(&initiator_security_origin))
     return true;
 
  const AtomicString& timing_allow_origin_string =
      original_timing_allow_origin.IsEmpty()
          ? response.HttpHeaderField(HTTPNames::Timing_Allow_Origin)
          : original_timing_allow_origin;
  if (timing_allow_origin_string.IsEmpty() ||
      EqualIgnoringASCIICase(timing_allow_origin_string, "null"))
    return false;

  if (timing_allow_origin_string == "*") {
    UseCounter::Count(context, WebFeature::kStarInTimingAllowOrigin);
    return true;
  }

  const String& security_origin = initiator_security_origin.ToString();
  Vector<String> timing_allow_origins;
  timing_allow_origin_string.GetString().Split(',', timing_allow_origins);
  if (timing_allow_origins.size() > 1) {
    UseCounter::Count(context, WebFeature::kMultipleOriginsInTimingAllowOrigin);
  } else if (timing_allow_origins.size() == 1 &&
             timing_allow_origin_string != "*") {
    UseCounter::Count(context, WebFeature::kSingleOriginInTimingAllowOrigin);
  }
  for (const String& allow_origin : timing_allow_origins) {
    const String allow_origin_stripped = allow_origin.StripWhiteSpace();
    if (allow_origin_stripped == security_origin ||
        allow_origin_stripped == "*") {
      return true;
    }
  }

  return false;
}
