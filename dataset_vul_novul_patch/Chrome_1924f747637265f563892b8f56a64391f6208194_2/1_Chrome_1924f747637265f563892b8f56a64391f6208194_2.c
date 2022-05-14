void StopCastCallback(
    CastConfigDelegate* cast_config,
    const CastConfigDelegate::ReceiversAndActivites& receivers_activities) {
  for (auto& item : receivers_activities) {
    CastConfigDelegate::Activity activity = item.second.activity;
    if (activity.allow_stop && activity.id.empty() == false)
      cast_config->StopCasting(activity.id);
  }
}
