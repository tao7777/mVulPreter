 void PermissionsRequestFunction::InstallUIAbort(bool user_initiated) {
  results_ = Request::Results::Create(false);
   SendResponse(true);
 
   Release();  // Balanced in RunImpl().
}
