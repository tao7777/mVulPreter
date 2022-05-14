AccessControlStatus ScriptResource::CalculateAccessControlStatus() const {
AccessControlStatus ScriptResource::CalculateAccessControlStatus(
    const SecurityOrigin* security_origin) const {
  if (GetResponse().WasFetchedViaServiceWorker()) {
    if (GetCORSStatus() == CORSStatus::kServiceWorkerOpaque)
      return kOpaqueResource;
    return kSharableCrossOrigin;
  }
 
  if (security_origin && PassesAccessControlCheck(*security_origin))
     return kSharableCrossOrigin;
 
   return kNotSharableCrossOrigin;
}
