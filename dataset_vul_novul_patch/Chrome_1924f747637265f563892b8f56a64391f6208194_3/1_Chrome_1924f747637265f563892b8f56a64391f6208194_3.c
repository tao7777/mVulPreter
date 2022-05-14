void CastConfigDelegateChromeos::StopCasting(const std::string& activity_id) {
   ExecuteJavaScript("backgroundSetup.stopCastMirroring('user-stop');");
 }
