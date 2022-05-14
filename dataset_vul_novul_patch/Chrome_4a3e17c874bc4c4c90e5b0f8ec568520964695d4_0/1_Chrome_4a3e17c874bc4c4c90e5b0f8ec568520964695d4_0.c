bool SerializeNotificationDatabaseData(const NotificationDatabaseData& input,
                                       std::string* output) {
  DCHECK(output);

  scoped_ptr<NotificationDatabaseDataProto::NotificationData> payload(
      new NotificationDatabaseDataProto::NotificationData());

  const PlatformNotificationData& notification_data = input.notification_data;

  payload->set_title(base::UTF16ToUTF8(notification_data.title));

  switch (notification_data.direction) {
    case PlatformNotificationData::DIRECTION_LEFT_TO_RIGHT:
      payload->set_direction(
          NotificationDatabaseDataProto::NotificationData::LEFT_TO_RIGHT);
      break;
    case PlatformNotificationData::DIRECTION_RIGHT_TO_LEFT:
      payload->set_direction(
          NotificationDatabaseDataProto::NotificationData::RIGHT_TO_LEFT);
      break;
    case PlatformNotificationData::DIRECTION_AUTO:
      payload->set_direction(
          NotificationDatabaseDataProto::NotificationData::AUTO);
      break;
  }

  payload->set_lang(notification_data.lang);
  payload->set_body(base::UTF16ToUTF8(notification_data.body));
  payload->set_tag(notification_data.tag);
  payload->set_icon(notification_data.icon.spec());

  for (size_t i = 0; i < notification_data.vibration_pattern.size(); ++i)
    payload->add_vibration_pattern(notification_data.vibration_pattern[i]);

  payload->set_timestamp(notification_data.timestamp.ToInternalValue());
  payload->set_silent(notification_data.silent);
  payload->set_require_interaction(notification_data.require_interaction);

  if (notification_data.data.size()) {
    payload->set_data(&notification_data.data.front(),
                      notification_data.data.size());
  }

  for (const PlatformNotificationAction& action : notification_data.actions) {
    NotificationDatabaseDataProto::NotificationAction* payload_action =
         payload->add_actions();
     payload_action->set_action(action.action);
     payload_action->set_title(base::UTF16ToUTF8(action.title));
   }
 
   NotificationDatabaseDataProto message;
  message.set_notification_id(input.notification_id);
  message.set_origin(input.origin.spec());
  message.set_service_worker_registration_id(
      input.service_worker_registration_id);
  message.set_allocated_notification_data(payload.release());

  return message.SerializeToString(output);
}
