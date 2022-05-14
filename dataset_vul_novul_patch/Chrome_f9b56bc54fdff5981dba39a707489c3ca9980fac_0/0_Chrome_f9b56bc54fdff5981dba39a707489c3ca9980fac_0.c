bool LookupMatchInTopDomains(base::StringPiece skeleton) {
bool LookupMatchInTopDomains(const icu::UnicodeString& ustr_skeleton) {
  std::string skeleton;
  ustr_skeleton.toUTF8String(skeleton);
   DCHECK_NE(skeleton.back(), '.');
   auto labels = base::SplitStringPiece(skeleton, ".", base::KEEP_WHITESPACE,
                                        base::SPLIT_WANT_ALL);

  if (labels.size() > kNumberOfLabelsToCheck) {
    labels.erase(labels.begin(),
                 labels.begin() + labels.size() - kNumberOfLabelsToCheck);
  }

  while (labels.size() > 1) {
    std::string partial_skeleton = base::JoinString(labels, ".");
    if (net::LookupStringInFixedSet(
            g_graph, g_graph_length, partial_skeleton.data(),
            partial_skeleton.length()) != net::kDafsaNotFound)
      return true;
    labels.erase(labels.begin());
  }
  return false;
}
