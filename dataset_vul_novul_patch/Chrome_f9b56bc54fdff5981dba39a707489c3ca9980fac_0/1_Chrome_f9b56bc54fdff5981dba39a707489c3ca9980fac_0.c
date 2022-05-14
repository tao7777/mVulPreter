bool LookupMatchInTopDomains(base::StringPiece skeleton) {
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
