void BrowserDevToolsAgentHost::AttachSession(DevToolsSession* session) {
bool BrowserDevToolsAgentHost::AttachSession(DevToolsSession* session) {
  if (session->restricted())
    return false;

   session->SetBrowserOnly(true);
   session->AddHandler(
       base::WrapUnique(new protocol::TargetHandler(true /* browser_only */)));
   if (only_discovery_)
    return true;
 
   session->AddHandler(base::WrapUnique(new protocol::BrowserHandler()));
   session->AddHandler(base::WrapUnique(new protocol::IOHandler(
      GetIOContext())));
  session->AddHandler(base::WrapUnique(new protocol::MemoryHandler()));
  session->AddHandler(base::WrapUnique(new protocol::SecurityHandler()));
  session->AddHandler(base::WrapUnique(new protocol::SystemInfoHandler()));
  session->AddHandler(base::WrapUnique(new protocol::TetheringHandler(
      socket_callback_, tethering_task_runner_)));
  session->AddHandler(base::WrapUnique(new protocol::TracingHandler(
       protocol::TracingHandler::Browser,
       FrameTreeNode::kFrameTreeNodeInvalidId,
       GetIOContext())));
  return true;
 }
