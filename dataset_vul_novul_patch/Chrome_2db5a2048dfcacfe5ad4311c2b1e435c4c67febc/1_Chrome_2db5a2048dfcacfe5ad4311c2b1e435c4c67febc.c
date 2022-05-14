bool DefaultTabHandler::CanCloseContentsAt(int index) {
  return delegate_->AsBrowser()->CanCloseContentsAt(index);
 }
