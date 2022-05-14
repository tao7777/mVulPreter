PlatformNotificationData ToPlatformNotificationData(
    const WebNotificationData& web_data) {
  PlatformNotificationData platform_data;
  platform_data.title = web_data.title;

  switch (web_data.direction) {
    case WebNotificationData::DirectionLeftToRight:
      platform_data.direction =
          PlatformNotificationData::DIRECTION_LEFT_TO_RIGHT;
      break;
    case WebNotificationData::DirectionRightToLeft:
      platform_data.direction =
          PlatformNotificationData::DIRECTION_RIGHT_TO_LEFT;
      break;
    case WebNotificationData::DirectionAuto:
      platform_data.direction = PlatformNotificationData::DIRECTION_AUTO;
      break;
  }

  platform_data.lang = base::UTF16ToUTF8(base::StringPiece16(web_data.lang));
  platform_data.body = web_data.body;
  platform_data.tag = base::UTF16ToUTF8(base::StringPiece16(web_data.tag));
  platform_data.icon = blink::WebStringToGURL(web_data.icon.string());
  platform_data.vibration_pattern.assign(web_data.vibrate.begin(),
                                         web_data.vibrate.end());
  platform_data.timestamp = base::Time::FromJsTime(web_data.timestamp);
  platform_data.silent = web_data.silent;
  platform_data.require_interaction = web_data.requireInteraction;
  platform_data.data.assign(web_data.data.begin(), web_data.data.end());
  platform_data.actions.resize(web_data.actions.size());
  for (size_t i = 0; i < web_data.actions.size(); ++i) {
     platform_data.actions[i].action =
         base::UTF16ToUTF8(base::StringPiece16(web_data.actions[i].action));
     platform_data.actions[i].title = web_data.actions[i].title;
   }
 
   return platform_data;
}
