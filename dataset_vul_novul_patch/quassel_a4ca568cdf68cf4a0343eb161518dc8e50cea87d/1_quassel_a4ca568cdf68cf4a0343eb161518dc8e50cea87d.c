void CtcpHandler::defaultHandler(const QString &cmd, CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param) {
   Q_UNUSED(ctcptype);
   Q_UNUSED(target);
  if(!_ignoreListManager->ctcpMatch(prefix, network()->networkName())) {
    QString str = tr("Received unknown CTCP %1 by %2").arg(cmd).arg(prefix);
    if(!param.isEmpty())
      str.append(tr(" with arguments: %1").arg(param));
    emit displayMsg(Message::Error, BufferInfo::StatusBuffer, "", str);
  }
 }
