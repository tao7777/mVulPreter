void CastConfigDelegateChromeos::StopCasting(const std::string& activity_id) {
void CastConfigDelegateChromeos::StopCasting() {
   ExecuteJavaScript("backgroundSetup.stopCastMirroring('user-stop');");

  // TODO(jdufault): Remove this after stopCastMirroring is properly exported.
  // The current beta/release versions of the cast extension do not export
  // stopCastMirroring, so we will also try to call the minified version.
  // See crbug.com/489929.
  ExecuteJavaScript("backgroundSetup.Qu('user-stop');");
 }
