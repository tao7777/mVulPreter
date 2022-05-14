 bool MockRenderThread::OnMessageReceived(const IPC::Message& msg) {
   sink_.OnMessageReceived(msg);

  bool handled = true;
   bool msg_is_ok = true;
   IPC_BEGIN_MESSAGE_MAP_EX(MockRenderThread, msg, msg_is_ok)
     IPC_MESSAGE_HANDLER(ViewHostMsg_CreateWidget, OnMsgCreateWidget)
    IPC_MESSAGE_HANDLER(ViewHostMsg_CreateWindow, OnMsgCreateWindow)
     IPC_MESSAGE_UNHANDLED(handled = false)
   IPC_END_MESSAGE_MAP_EX()
   return handled;
}
