 void ResetScreenHandler::UpdateStatusChanged(
     const UpdateEngineClient::Status& status) {
  VLOG(1) << "Update status change to " << status.status;
   if (status.status == UpdateEngineClient::UPDATE_STATUS_ERROR) {
     base::DictionaryValue params;
    params.SetInteger("uiState", kErrorUIStateRollback);
    ShowScreen(OobeUI::kScreenErrorMessage, &params);
  } else if (status.status ==
      UpdateEngineClient::UPDATE_STATUS_UPDATED_NEED_REBOOT) {
    DBusThreadManager::Get()->GetPowerManagerClient()->RequestRestart();
  }
}
