 void DevToolsSession::AddHandler(
     std::unique_ptr<protocol::DevToolsDomainHandler> handler) {
   handler->Wire(dispatcher_.get());
  handler->SetRenderer(process_, host_);
   handlers_[handler->name()] = std::move(handler);
 }
