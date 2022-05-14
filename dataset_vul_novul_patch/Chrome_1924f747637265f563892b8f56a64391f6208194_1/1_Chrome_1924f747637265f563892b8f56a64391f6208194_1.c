void StopCast() {
  CastConfigDelegate* cast_config =
      Shell::GetInstance()->system_tray_delegate()->GetCastConfigDelegate();
  if (cast_config && cast_config->HasCastExtension()) {
    cast_config->GetReceiversAndActivities(
        base::Bind(&StopCastCallback, cast_config));
  }
}
