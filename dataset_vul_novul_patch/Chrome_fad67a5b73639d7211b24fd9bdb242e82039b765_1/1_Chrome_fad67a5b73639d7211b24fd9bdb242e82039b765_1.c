AccessControlStatus ScriptResource::CalculateAccessControlStatus() const {
  if (GetCORSStatus() == CORSStatus::kServiceWorkerOpaque)
    return kOpaqueResource;
 
  if (IsSameOriginOrCORSSuccessful())
     return kSharableCrossOrigin;
 
   return kNotSharableCrossOrigin;
}
