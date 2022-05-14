WebNotificationData ToWebNotificationData(
    const PlatformNotificationData& platform_data) {
  WebNotificationData web_data;
  web_data.title = platform_data.title;

  switch (platform_data.direction) {
    case PlatformNotificationData::DIRECTION_LEFT_TO_RIGHT:
      web_data.direction = WebNotificationData::DirectionLeftToRight;
      break;
    case PlatformNotificationData::DIRECTION_RIGHT_TO_LEFT:
      web_data.direction = WebNotificationData::DirectionRightToLeft;
      break;
    case PlatformNotificationData::DIRECTION_AUTO:
      web_data.direction = WebNotificationData::DirectionAuto;
      break;
  }

  web_data.lang = blink::WebString::fromUTF8(platform_data.lang);
  web_data.body = platform_data.body;
  web_data.tag = blink::WebString::fromUTF8(platform_data.tag);
  web_data.icon = blink::WebURL(platform_data.icon);
  web_data.vibrate = platform_data.vibration_pattern;
  web_data.timestamp = platform_data.timestamp.ToJsTime();
  web_data.silent = platform_data.silent;
  web_data.requireInteraction = platform_data.require_interaction;
  web_data.data = platform_data.data;
  blink::WebVector<blink::WebNotificationAction> resized(
      platform_data.actions.size());
  web_data.actions.swap(resized);
  for (size_t i = 0; i < platform_data.actions.size(); ++i) {
     web_data.actions[i].action =
         blink::WebString::fromUTF8(platform_data.actions[i].action);
     web_data.actions[i].title = platform_data.actions[i].title;
   }
 
   return web_data;
}
