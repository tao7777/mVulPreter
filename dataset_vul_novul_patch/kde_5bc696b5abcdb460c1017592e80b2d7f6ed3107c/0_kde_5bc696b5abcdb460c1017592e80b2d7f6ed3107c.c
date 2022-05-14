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

    if (timeout <= 0) {
        timeout = 60000 * count / AVERAGE_WORD_LENGTH / WORD_PER_MINUTE;

        timeout = 2000 + qMax(timeout, 3000);
    }

    const QString source = QStringLiteral("notification %1").arg(id);
     const QString source = QStringLiteral("notification %1").arg(id);
 
     QString bodyFinal = (partOf == 0 ? body : _body);
    bodyFinal = NotificationSanitizer::parse(bodyFinal);
 
     Plasma::DataEngine::Data notificationData;
     notificationData.insert(QStringLiteral("id"), QString::number(id));
    bodyFinal = bodyFinal.simplified();
    bodyFinal.replace(QRegularExpression(QStringLiteral("<br/>\\s*<br/>(\\s|<br/>)*")), QLatin1String("<br/>"));
    bodyFinal.replace(QRegularExpression(QStringLiteral("&(?!(?:apos|quot|[gl]t|amp);|#)")), QLatin1String("&amp;"));
    bodyFinal.replace(QLatin1String("&apos;"), QChar('\''));

    Plasma::DataEngine::Data notificationData;
    notificationData.insert(QStringLiteral("id"), QString::number(id));
    notificationData.insert(QStringLiteral("eventId"), eventId);
    notificationData.insert(QStringLiteral("appName"), appname_str);
    notificationData.insert(QStringLiteral("appIcon"), app_icon);
    notificationData.insert(QStringLiteral("summary"), summary);
    notificationData.insert(QStringLiteral("body"), bodyFinal);
    notificationData.insert(QStringLiteral("actions"), actions);
    notificationData.insert(QStringLiteral("isPersistent"), isPersistent);
    notificationData.insert(QStringLiteral("expireTimeout"), timeout);

    bool configurable = false;
    if (!appRealName.isEmpty()) {

        if (m_configurableApplications.contains(appRealName)) {
            configurable = m_configurableApplications.value(appRealName);
        } else {
            QScopedPointer<KConfig> config(new KConfig(appRealName + QStringLiteral(".notifyrc"), KConfig::NoGlobals));
            config->addConfigSources(QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                     QStringLiteral("knotifications5/") + appRealName + QStringLiteral(".notifyrc")));

            const QRegularExpression regexp(QStringLiteral("^Event/([^/]*)$"));
            configurable = !config->groupList().filter(regexp).isEmpty();
            m_configurableApplications.insert(appRealName, configurable);
        }
    }
    notificationData.insert(QStringLiteral("appRealName"), appRealName);
    notificationData.insert(QStringLiteral("configurable"), configurable);

    QImage image;
    if (hints.contains(QStringLiteral("image-data"))) {
        QDBusArgument arg = hints[QStringLiteral("image-data")].value<QDBusArgument>();
        image = decodeNotificationSpecImageHint(arg);
    } else if (hints.contains(QStringLiteral("image_data"))) {
        QDBusArgument arg = hints[QStringLiteral("image_data")].value<QDBusArgument>();
        image = decodeNotificationSpecImageHint(arg);
    } else if (hints.contains(QStringLiteral("image-path"))) {
        QString path = findImageForSpecImagePath(hints[QStringLiteral("image-path")].toString());
        if (!path.isEmpty()) {
            image.load(path);
        }
    } else if (hints.contains(QStringLiteral("image_path"))) {
        QString path = findImageForSpecImagePath(hints[QStringLiteral("image_path")].toString());
        if (!path.isEmpty()) {
            image.load(path);
        }
    } else if (hints.contains(QStringLiteral("icon_data"))) {
        QDBusArgument arg = hints[QStringLiteral("icon_data")].value<QDBusArgument>();
        image = decodeNotificationSpecImageHint(arg);
    }
    notificationData.insert(QStringLiteral("image"), image.isNull() ? QVariant() : image);

    if (hints.contains(QStringLiteral("urgency"))) {
        notificationData.insert(QStringLiteral("urgency"), hints[QStringLiteral("urgency")].toInt());
    }

    setData(source, notificationData);

    m_activeNotifications.insert(source, app_name + summary);

    return id;
}
