ResourceRequestBlockedReason BaseFetchContext::CanRequest(
    Resource::Type type,
    const ResourceRequest& resource_request,
    const KURL& url,
    const ResourceLoaderOptions& options,
    SecurityViolationReportingPolicy reporting_policy,
    FetchParameters::OriginRestriction origin_restriction,
    ResourceRequest::RedirectStatus redirect_status) const {
  ResourceRequestBlockedReason blocked_reason =
      CanRequestInternal(type, resource_request, url, options, reporting_policy,
                         origin_restriction, redirect_status);
   if (blocked_reason != ResourceRequestBlockedReason::kNone &&
       reporting_policy == SecurityViolationReportingPolicy::kReport) {
     DispatchDidBlockRequest(resource_request, options.initiator_info,
                            blocked_reason, type);
   }
   return blocked_reason;
 }
