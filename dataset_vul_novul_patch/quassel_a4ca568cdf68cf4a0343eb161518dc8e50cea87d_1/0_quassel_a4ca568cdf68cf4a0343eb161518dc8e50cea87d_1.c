void CtcpHandler::handlePing(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param) {
void CtcpHandler::handlePing(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param, QString &reply) {
   Q_UNUSED(target)
   if(ctcptype == CtcpQuery) {
    reply = param;
     emit displayMsg(Message::Server, BufferInfo::StatusBuffer, "", tr("Received CTCP PING request from %1").arg(prefix));
   } else {
    emit displayMsg(Message::Server, BufferInfo::StatusBuffer, "", tr("Received CTCP TIME request by %1").arg(prefix));
  }
  else {
    emit displayMsg(Message::Server, BufferInfo::StatusBuffer, "", tr("Received CTCP TIME answer from %1: %2")
   }
 }
