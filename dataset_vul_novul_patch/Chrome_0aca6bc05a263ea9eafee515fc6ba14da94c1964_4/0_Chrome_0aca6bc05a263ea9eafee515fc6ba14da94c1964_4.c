  testing::AssertionResult ScriptAllowedExclusivelyOnTab(
      const Extension* extension,
      const std::set<GURL>& allowed_urls,
       int tab_id) {
     std::vector<std::string> errors;
     for (const GURL& url : urls_) {
      AccessType access = GetExtensionAccess(extension, url, tab_id);
      AccessType expected_access =
          allowed_urls.count(url) ? ALLOWED_SCRIPT_ONLY : DISALLOWED;
      if (access != expected_access) {
        errors.push_back(
            base::StringPrintf("Error for url '%s': expected %d, found %d",
                               url.spec().c_str(), expected_access, access));
      }
     }
 
     if (!errors.empty())
      return testing::AssertionFailure() << base::JoinString(errors, "\n");
    return testing::AssertionSuccess();
  }
