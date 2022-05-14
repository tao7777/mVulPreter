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
 
    if (resource_type == HTTP_AUTH_RESOURCE_BLOCKED_CROSS)
      return false;
   }
 
   return true;
}
