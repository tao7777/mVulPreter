 String InspectorPageAgent::CachedResourceTypeJson(
     const Resource& cached_resource) {
  return ResourceTypeJson(ToResourceType(cached_resource.GetType()));
 }
