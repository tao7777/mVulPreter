void CtcpHandler::handleAction(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param) {
void CtcpHandler::handleAction(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param, QString &/*reply*/) {
   Q_UNUSED(ctcptype)
   emit displayMsg(Message::Action, typeByTarget(target), target, param, prefix);
 }
