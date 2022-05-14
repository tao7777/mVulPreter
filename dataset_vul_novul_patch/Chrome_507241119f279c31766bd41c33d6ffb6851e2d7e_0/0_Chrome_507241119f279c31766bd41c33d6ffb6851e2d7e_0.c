std::string* GetTestingDMToken() {
const char** GetTestingDMTokenStorage() {
  static const char* dm_token = "";
   return &dm_token;
 }
