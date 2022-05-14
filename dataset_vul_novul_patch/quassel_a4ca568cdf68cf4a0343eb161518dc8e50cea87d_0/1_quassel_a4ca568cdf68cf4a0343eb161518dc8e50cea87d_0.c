void CtcpHandler::handleAction(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param) {
   Q_UNUSED(ctcptype)
   emit displayMsg(Message::Action, typeByTarget(target), target, param, prefix);
 }
