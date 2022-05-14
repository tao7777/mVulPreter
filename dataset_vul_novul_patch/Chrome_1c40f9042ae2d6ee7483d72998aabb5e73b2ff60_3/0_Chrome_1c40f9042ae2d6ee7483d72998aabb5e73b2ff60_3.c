InspectorPageAgent::ResourceType InspectorPageAgent::CachedResourceType(
InspectorPageAgent::ResourceType InspectorPageAgent::ToResourceType(
    const Resource::Type resource_type) {
  switch (resource_type) {
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
