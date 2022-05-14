  testing::AssertionResult ScriptAllowedExclusivelyOnTab(
      const Extension* extension,
      const std::set<GURL>& allowed_urls,
       int tab_id) {
     std::vector<std::string> errors;
     for (const GURL& url : urls_) {
      bool allowed = IsAllowedScript(extension, url, tab_id);
      if (allowed && !allowed_urls.count(url))
        errors.push_back("Script unexpectedly disallowed on " + url.spec());
      else if (!allowed && allowed_urls.count(url))
        errors.push_back("Script unexpectedly allowed on " + url.spec());
     }
 
     if (!errors.empty())
      return testing::AssertionFailure() << base::JoinString(errors, "\n");
    return testing::AssertionSuccess();
  }
