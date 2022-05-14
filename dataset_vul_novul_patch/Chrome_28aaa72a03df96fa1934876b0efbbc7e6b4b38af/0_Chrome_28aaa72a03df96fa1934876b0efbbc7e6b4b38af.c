bool ResourceDispatcherHostImpl::AcceptAuthRequest(
    ResourceLoader* loader,
    net::AuthChallengeInfo* auth_info) {
  if (delegate_ && !delegate_->AcceptAuthRequest(loader->request(), auth_info))
    return false;

  if (!auth_info->is_proxy) {
    HttpAuthResourceType resource_type =
        HttpAuthResourceTypeOf(loader->request());
    UMA_HISTOGRAM_ENUMERATION("Net.HttpAuthResource",
                               resource_type,
                               HTTP_AUTH_RESOURCE_LAST);
 
    // TODO(tsepez): Return false on HTTP_AUTH_RESOURCE_BLOCKED_CROSS.
    // The code once did this, but was changed due to http://crbug.com/174129.
    // http://crbug.com/174179 has been filed to track this issue.
   }
 
   return true;
}
