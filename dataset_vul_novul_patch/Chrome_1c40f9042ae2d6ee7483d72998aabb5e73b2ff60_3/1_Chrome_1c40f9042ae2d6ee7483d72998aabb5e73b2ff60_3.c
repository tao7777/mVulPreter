InspectorPageAgent::ResourceType InspectorPageAgent::CachedResourceType(
    const Resource& cached_resource) {
  switch (cached_resource.GetType()) {
     case Resource::kImage:
       return InspectorPageAgent::kImageResource;
     case Resource::kFont:
      return InspectorPageAgent::kFontResource;
    case Resource::kMedia:
      return InspectorPageAgent::kMediaResource;
    case Resource::kManifest:
      return InspectorPageAgent::kManifestResource;
    case Resource::kTextTrack:
      return InspectorPageAgent::kTextTrackResource;
    case Resource::kCSSStyleSheet:
    case Resource::kXSLStyleSheet:
      return InspectorPageAgent::kStylesheetResource;
    case Resource::kScript:
      return InspectorPageAgent::kScriptResource;
    case Resource::kImportResource:
    case Resource::kMainResource:
      return InspectorPageAgent::kDocumentResource;
    default:
      break;
  }
  return InspectorPageAgent::kOtherResource;
}
