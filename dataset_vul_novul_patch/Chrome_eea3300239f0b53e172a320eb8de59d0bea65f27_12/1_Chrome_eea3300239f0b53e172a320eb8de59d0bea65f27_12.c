std::string SanitizeRevision(const std::string& revision) {
  for (size_t i = 0; i < revision.length(); i++) {
    if (!(revision[i] == '@' && i == 0)
        && !(revision[i] >= '0' && revision[i] <= '9')
        && !(revision[i] >= 'a' && revision[i] <= 'z')
        && !(revision[i] >= 'A' && revision[i] <= 'Z')) {
      return std::string();
    }
  }
  return revision;
}
