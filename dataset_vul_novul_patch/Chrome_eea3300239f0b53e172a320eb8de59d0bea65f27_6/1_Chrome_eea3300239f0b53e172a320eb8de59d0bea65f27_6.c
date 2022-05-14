std::string SanitizeFrontendPath(const std::string& path) {
  for (size_t i = 0; i < path.length(); i++) {
    if (path[i] != '/' && path[i] != '-' && path[i] != '_'
        && path[i] != '.' && path[i] != '@'
        && !(path[i] >= '0' && path[i] <= '9')
        && !(path[i] >= 'a' && path[i] <= 'z')
        && !(path[i] >= 'A' && path[i] <= 'Z')) {
      return std::string();
    }
  }
  return path;
}
