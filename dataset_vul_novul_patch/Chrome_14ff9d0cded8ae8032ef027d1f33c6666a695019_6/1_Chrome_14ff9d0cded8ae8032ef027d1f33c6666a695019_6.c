 NotificationsNativeHandler::NotificationsNativeHandler(ScriptContext* context)
     : ObjectBackedNativeHandler(context) {
   RouteFunction(
      "GetNotificationImageSizes",
       base::Bind(&NotificationsNativeHandler::GetNotificationImageSizes,
                  base::Unretained(this)));
 }
