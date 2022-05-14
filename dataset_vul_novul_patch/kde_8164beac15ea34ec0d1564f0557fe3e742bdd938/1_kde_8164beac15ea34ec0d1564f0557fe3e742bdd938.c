uint NotificationsEngine::Notify(const QString &app_name, uint replaces_id,
                                 const QString &app_icon, const QString &summary, const QString &body,
                                 const QStringList &actions, const QVariantMap &hints, int timeout)
{
    uint partOf = 0;
    const QString appRealName = hints[QStringLiteral("x-kde-appname")].toString();
    const QString eventId = hints[QStringLiteral("x-kde-eventId")].toString();
    const bool skipGrouping = hints[QStringLiteral("x-kde-skipGrouping")].toBool();

    if (!replaces_id && m_activeNotifications.values().contains(app_name + summary) && !skipGrouping && !m_alwaysReplaceAppsList.contains(app_name)) {
        partOf = m_activeNotifications.key(app_name + summary).midRef(13).toUInt();
    }

     qDebug() << "Currrent active notifications:" << m_activeNotifications;
     qDebug() << "Guessing partOf as:" << partOf;
     qDebug() << " New Notification: " << summary << body << timeout << "& Part of:" << partOf;
    QString _body;
 
     if (partOf > 0) {
         const QString source = QStringLiteral("notification %1").arg(partOf);
         Plasma::DataContainer *container = containerForSource(source);
         if (container) {
            _body = container->data()[QStringLiteral("body")].toString();
            if (_body != body) {
                _body.append("\n").append(body);
            } else {
                _body = body;
             }
 
             replaces_id = partOf;
            CloseNotification(partOf);
        }
    }

    uint id = replaces_id ? replaces_id : m_nextId++;

    if (m_alwaysReplaceAppsList.contains(app_name)) {
        if (m_notificationsFromReplaceableApp.contains(app_name)) {
            id = m_notificationsFromReplaceableApp.value(app_name);
        } else {
            m_notificationsFromReplaceableApp.insert(app_name, id);
        }
    }

    QString appname_str = app_name;
    if (appname_str.isEmpty()) {
        appname_str = i18n("Unknown Application");
    }

    bool isPersistent = timeout == 0;

 
     const int AVERAGE_WORD_LENGTH = 6;
     const int WORD_PER_MINUTE = 250;
    int count = summary.length() + body.length();
 
        timeout = 60000 * count / AVERAGE_WORD_LENGTH / WORD_PER_MINUTE;

        timeout = 2000 + qMax(timeout, 3000);
    }
