void CtcpHandler::parse(Message::Type messageType, const QString &prefix, const QString &target, const QByteArray &message) {
  QByteArray ctcp;

  QByteArray dequotedMessage = lowLevelDequote(message);

  CtcpType ctcptype = messageType == Message::Notice
    ? CtcpReply
    : CtcpQuery;

  Message::Flags flags = (messageType == Message::Notice && !network()->isChannelName(target))
    ? Message::Redirected
    : Message::None;

   int xdelimPos = -1;
   int xdelimEndPos = -1;
   int spacePos = -1;
   while((xdelimPos = dequotedMessage.indexOf(XDELIM)) != -1) {
     if(xdelimPos > 0)
       displayMsg(messageType, target, userDecode(target, dequotedMessage.left(xdelimPos)), prefix, flags);
    xdelimEndPos = dequotedMessage.indexOf(XDELIM, xdelimPos + 1);
    if(xdelimEndPos == -1) {
      xdelimEndPos = dequotedMessage.count();
    }
    ctcp = xdelimDequote(dequotedMessage.mid(xdelimPos + 1, xdelimEndPos - xdelimPos - 1));
    dequotedMessage = dequotedMessage.mid(xdelimEndPos + 1);

    QString ctcpcmd = userDecode(target, ctcp.left(spacePos));
    QString ctcpparam = userDecode(target, ctcp.mid(spacePos + 1));

    spacePos = ctcp.indexOf(' ');
    if(spacePos != -1) {
      ctcpcmd = userDecode(target, ctcp.left(spacePos));
      ctcpparam = userDecode(target, ctcp.mid(spacePos + 1));
    } else {
      ctcpcmd = userDecode(target, ctcp);
      ctcpparam = QString();
       ctcpparam = QString();
     }
 
    handle(ctcpcmd, Q_ARG(CtcpType, ctcptype), Q_ARG(QString, prefix), Q_ARG(QString, target), Q_ARG(QString, ctcpparam));
   }
 
   if(!dequotedMessage.isEmpty())
void CtcpHandler::query(const QString &bufname, const QString &ctcpTag, const QString &message) {
  QList<QByteArray> params;
  params << serverEncode(bufname) << lowLevelQuote(pack(serverEncode(ctcpTag), userEncode(bufname, message)));
  emit putCmd("PRIVMSG", params);
}

void CtcpHandler::reply(const QString &bufname, const QString &ctcpTag, const QString &message) {
  QList<QByteArray> params;
  params << serverEncode(bufname) << lowLevelQuote(pack(serverEncode(ctcpTag), userEncode(bufname, message)));
  emit putCmd("NOTICE", params);
}

void CtcpHandler::handleAction(CtcpType ctcptype, const QString &prefix, const QString &target, const QString &param) {
  Q_UNUSED(ctcptype)
  emit displayMsg(Message::Action, typeByTarget(target), target, param, prefix);
}
   emit putCmd("NOTICE", params);
 }
