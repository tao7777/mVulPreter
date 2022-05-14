std::string GetDMToken() {
  std::string dm_token = *GetTestingDMToken();
 
 #if !defined(OS_CHROMEOS)
 
  if (dm_token.empty() &&
       policy::ChromeBrowserCloudManagementController::IsEnabled()) {
    dm_token = policy::BrowserDMTokenStorage::Get()->RetrieveDMToken();
   }
 #endif
 
  return dm_token;
}
