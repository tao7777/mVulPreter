void Dispatcher::AddStatus(const std::string& pattern) {
  mg_set_uri_callback(context_, (root_ + pattern).c_str(), &SendStatus, NULL);
 }
