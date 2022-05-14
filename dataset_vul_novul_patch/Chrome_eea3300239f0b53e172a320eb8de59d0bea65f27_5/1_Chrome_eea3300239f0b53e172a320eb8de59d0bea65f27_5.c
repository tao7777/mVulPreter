std::string SanitizeEndpoint(const std::string& value) {
  if (value.find('&') != std::string::npos
      || value.find('?') != std::string::npos)
    return std::string();
  return value;
}
