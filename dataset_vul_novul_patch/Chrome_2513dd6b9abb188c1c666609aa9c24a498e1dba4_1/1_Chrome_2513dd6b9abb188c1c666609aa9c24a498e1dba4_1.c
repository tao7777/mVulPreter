 void HWNDMessageHandler::OnEnterSizeMove() {
   delegate_->HandleBeginWMSizeMove();
   SetMsgHandled(FALSE);
 }
