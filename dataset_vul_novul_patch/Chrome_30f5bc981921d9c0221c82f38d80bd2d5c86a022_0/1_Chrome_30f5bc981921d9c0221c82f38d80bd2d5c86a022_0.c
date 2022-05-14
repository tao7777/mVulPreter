    AsOmniboxEventProviderType() const {
  if (name_ == "HistoryURL")
    return metrics::OmniboxEventProto::HISTORY_URL;
  if (name_ == "HistoryContents")
    return metrics::OmniboxEventProto::HISTORY_CONTENTS;
  if (name_ == "HistoryQuickProvider")
    return metrics::OmniboxEventProto::HISTORY_QUICK;
  if (name_ == "Search")
    return metrics::OmniboxEventProto::SEARCH;
  if (name_ == "Keyword")
    return metrics::OmniboxEventProto::KEYWORD;
  if (name_ == "Builtin")
    return metrics::OmniboxEventProto::BUILTIN;
  if (name_ == "ShortcutsProvider")
    return metrics::OmniboxEventProto::SHORTCUTS;
  if (name_ == "ExtensionApps")
    return metrics::OmniboxEventProto::EXTENSION_APPS;
  NOTREACHED();
  return metrics::OmniboxEventProto::UNKNOWN_PROVIDER;
}
