void ResetScreenHandler::Show() {
  if (!page_is_ready()) {
    show_on_init_ = true;
    return;
  }

  PrefService* prefs = g_browser_process->local_state();
  restart_required_ = !CommandLine::ForCurrentProcess()->HasSwitch(
      switches::kFirstExecAfterBoot);
  reboot_was_requested_ = false;
   rollback_available_ = false;
   if (!restart_required_)  // First exec after boot.
     reboot_was_requested_ = prefs->GetBoolean(prefs::kFactoryResetRequested);
  if (!restart_required_ && reboot_was_requested_) {
     rollback_available_ = prefs->GetBoolean(prefs::kRollbackRequested);
     ShowWithParams();
  } else {
    chromeos::DBusThreadManager::Get()->GetUpdateEngineClient()->
        CanRollbackCheck(base::Bind(&ResetScreenHandler::OnRollbackCheck,
        weak_ptr_factory_.GetWeakPtr()));
  }
}
