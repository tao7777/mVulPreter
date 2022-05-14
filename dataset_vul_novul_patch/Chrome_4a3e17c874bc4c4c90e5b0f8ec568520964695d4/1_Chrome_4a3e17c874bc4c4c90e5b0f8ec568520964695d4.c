bool DeserializeNotificationDatabaseData(const std::string& input,
                                         NotificationDatabaseData* output) {
  DCHECK(output);

  NotificationDatabaseDataProto message;
  if (!message.ParseFromString(input))
    return false;

  output->notification_id = message.notification_id();
  output->origin = GURL(message.origin());
  output->service_worker_registration_id =
      message.service_worker_registration_id();

  PlatformNotificationData* notification_data = &output->notification_data;
  const NotificationDatabaseDataProto::NotificationData& payload =
      message.notification_data();

  notification_data->title = base::UTF8ToUTF16(payload.title());

  switch (payload.direction()) {
    case NotificationDatabaseDataProto::NotificationData::LEFT_TO_RIGHT:
      notification_data->direction =
          PlatformNotificationData::DIRECTION_LEFT_TO_RIGHT;
      break;
    case NotificationDatabaseDataProto::NotificationData::RIGHT_TO_LEFT:
      notification_data->direction =
          PlatformNotificationData::DIRECTION_RIGHT_TO_LEFT;
      break;
    case NotificationDatabaseDataProto::NotificationData::AUTO:
      notification_data->direction = PlatformNotificationData::DIRECTION_AUTO;
      break;
  }

  notification_data->lang = payload.lang();
  notification_data->body = base::UTF8ToUTF16(payload.body());
  notification_data->tag = payload.tag();
  notification_data->icon = GURL(payload.icon());

  if (payload.vibration_pattern().size() > 0) {
    notification_data->vibration_pattern.assign(
        payload.vibration_pattern().begin(), payload.vibration_pattern().end());
  }

  notification_data->timestamp =
      base::Time::FromInternalValue(payload.timestamp());
  notification_data->silent = payload.silent();
  notification_data->require_interaction = payload.require_interaction();

  if (payload.data().length()) {
    notification_data->data.assign(payload.data().begin(),
                                   payload.data().end());
  }

  for (const auto& payload_action : payload.actions()) {
     PlatformNotificationAction action;
     action.action = payload_action.action();
     action.title = base::UTF8ToUTF16(payload_action.title());
     notification_data->actions.push_back(action);
   }
 
  return true;
}
