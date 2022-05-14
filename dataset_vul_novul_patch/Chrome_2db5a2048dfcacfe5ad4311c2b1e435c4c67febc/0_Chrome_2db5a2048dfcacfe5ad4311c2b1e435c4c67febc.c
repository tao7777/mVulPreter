bool DefaultTabHandler::CanCloseContentsAt(int index) {
bool DefaultTabHandler::CanCloseContents(std::vector<int>* indices) {
  return delegate_->AsBrowser()->CanCloseContents(indices);
 }
