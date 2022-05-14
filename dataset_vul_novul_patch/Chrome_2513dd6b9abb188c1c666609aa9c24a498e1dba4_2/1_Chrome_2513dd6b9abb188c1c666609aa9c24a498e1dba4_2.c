 void HWNDMessageHandler::OnExitSizeMove() {
   delegate_->HandleEndWMSizeMove();
   SetMsgHandled(FALSE);
 }
