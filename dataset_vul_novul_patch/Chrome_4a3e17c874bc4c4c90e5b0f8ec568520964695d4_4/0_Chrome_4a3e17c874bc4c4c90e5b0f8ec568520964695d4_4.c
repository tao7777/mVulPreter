WebNotificationData createWebNotificationData(ExecutionContext* executionContext, const String& title, const NotificationOptions& options, ExceptionState& exceptionState)
{
    if (options.hasVibrate() && options.silent()) {
        exceptionState.throwTypeError("Silent notifications must not specify vibration patterns.");
        return WebNotificationData();
    }

    WebNotificationData webData;

    webData.title = title;
    webData.direction = toDirectionEnumValue(options.dir());
    webData.lang = options.lang();
    webData.body = options.body();
    webData.tag = options.tag();
 
     KURL iconUrl;
 
         iconUrl = executionContext->completeURL(options.icon());
         if (!iconUrl.isValid())
            iconUrl = KURL();
    }

    webData.icon = iconUrl;
    webData.vibrate = NavigatorVibration::sanitizeVibrationPattern(options.vibrate());
    webData.timestamp = options.hasTimestamp() ? static_cast<double>(options.timestamp()) : WTF::currentTimeMS();
    webData.silent = options.silent();
    webData.requireInteraction = options.requireInteraction();

    if (options.hasData()) {
        RefPtr<SerializedScriptValue> serializedScriptValue = SerializedScriptValueFactory::instance().create(options.data().isolate(), options.data(), nullptr, exceptionState);
        if (exceptionState.hadException())
            return WebNotificationData();

        Vector<char> serializedData;
        serializedScriptValue->toWireBytes(serializedData);

        webData.data = serializedData;
    }

    Vector<WebNotificationAction> actions;

    const size_t maxActions = Notification::maxActions();
    for (const NotificationAction& action : options.actions()) {
        if (actions.size() >= maxActions)
            break;

        WebNotificationAction webAction;
         webAction.action = action.action();
         webAction.title = action.title();
 
        KURL iconUrl;
        if (action.hasIcon() && !action.icon().isEmpty()) {
            iconUrl = executionContext->completeURL(action.icon());
            if (!iconUrl.isValid())
                iconUrl = KURL();
        }
        webAction.icon = iconUrl;

         actions.append(webAction);
     }
 
    webData.actions = actions;

    return webData;
}
