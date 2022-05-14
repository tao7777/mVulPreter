static bool IsJavaScriptUrl(const std::string& url) {
  return StartsWithASCII(url, "javascript:", false);
}
