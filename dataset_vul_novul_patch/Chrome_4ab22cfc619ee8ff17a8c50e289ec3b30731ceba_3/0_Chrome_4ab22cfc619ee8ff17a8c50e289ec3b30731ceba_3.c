void Dispatcher::AddStatus(const std::string& pattern) {
void Dispatcher::AddHealthz(const std::string& pattern) {
  mg_set_uri_callback(context_, (root_ + pattern).c_str(), &SendHealthz, NULL);
}

void Dispatcher::AddLog(const std::string& pattern) {
  mg_set_uri_callback(context_, (root_ + pattern).c_str(), &SendLog, NULL);
 }
