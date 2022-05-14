 void PermissionsRequestFunction::InstallUIAbort(bool user_initiated) {
   SendResponse(true);
 
   Release();  // Balanced in RunImpl().
}
