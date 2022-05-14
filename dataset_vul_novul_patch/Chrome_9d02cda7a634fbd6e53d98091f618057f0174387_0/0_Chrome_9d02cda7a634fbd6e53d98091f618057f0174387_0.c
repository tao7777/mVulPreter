std::string JoinPrefs(std::string parent, const char* child) {
std::string JoinPrefs(const std::string& parent, const char* child) {
   return parent + "." + child;
 }
