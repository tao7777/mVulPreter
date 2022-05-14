 void GeneratePathsToCheck(const GURL& url, std::vector<std::string>* paths) {
   paths->clear();

  std::string canon_path;
  std::string canon_query;
  CanonicalizeUrl(url, NULL, &canon_path, &canon_query);

  const std::string path = canon_path;   // const sidesteps GCC bugs below!
  const std::string query = canon_query;
   if (path.empty())
     return;
 
  const size_t kMaxPathsToCheck = 4;
  for (std::string::const_iterator i(path.begin());
       i != path.end() && paths->size() < kMaxPathsToCheck; ++i) {
    if (*i == '/')
      paths->push_back(std::string(path.begin(), i + 1));
  }

   if (paths->back() != path)
     paths->push_back(path);
 
  if (!query.empty())
    paths->push_back(path + "?" + query);
 }
