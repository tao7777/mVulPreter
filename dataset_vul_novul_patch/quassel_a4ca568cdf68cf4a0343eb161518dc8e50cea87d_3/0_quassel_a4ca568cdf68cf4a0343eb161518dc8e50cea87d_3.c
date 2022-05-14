void CtcpHandler::handleVersion(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param) {
void CtcpHandler::handleVersion(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param, QString &reply) {
   Q_UNUSED(target)
   if(ctcptype == CtcpQuery) {
    reply = QString("Quassel IRC %1 (built on %2) -- http://www.quassel-irc.org").arg(Quassel::buildInfo().plainVersionString).arg(Quassel::buildInfo().buildDate);
     emit displayMsg(Message::Server, BufferInfo::StatusBuffer, "", tr("Received CTCP VERSION request by %1").arg(prefix));
   } else {
      str.append(tr(" with arguments: %1").arg(param));
    emit displayMsg(Message::Error, BufferInfo::StatusBuffer, "", str);
   }
 }
