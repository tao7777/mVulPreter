std::string SanitizeFrontendQueryParam(
    const std::string& key,
    const std::string& value) {
  if (key == "can_dock" || key == "debugFrontend" || key == "experiments" ||
      key == "isSharedWorker" || key == "v8only" || key == "remoteFrontend")
    return "true";
  if (key == "ws" || key == "service-backend")
    return SanitizeEndpoint(value);
  if (key == "dockSide" && value == "undocked")
    return value;
  if (key == "panel" && (value == "elements" || value == "console"))
    return value;
  if (key == "remoteBase")
    return SanitizeRemoteBase(value);
  if (key == "remoteFrontendUrl")
    return SanitizeRemoteFrontendURL(value);
  return std::string();
}
