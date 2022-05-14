 TemplateURLRef::SearchTermsArgs::ContextualSearchParams::ContextualSearchParams(
     int version,
    size_t start,
    size_t end,
    const std::string& selection,
     const std::string& content,
     const std::string& base_page_url,
     const std::string& encoding,
    int now_on_tap_version)
     : version(version),
       start(start),
       end(end),
       selection(selection),
       content(content),
       base_page_url(base_page_url),
       encoding(encoding),
      now_on_tap_version(now_on_tap_version) {}
