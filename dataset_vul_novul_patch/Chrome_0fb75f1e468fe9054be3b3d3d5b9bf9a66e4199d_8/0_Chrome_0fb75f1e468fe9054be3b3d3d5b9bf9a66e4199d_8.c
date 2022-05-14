void LeafUnwindBlacklist::AddModuleToBlacklist(const void* module) {
void LeafUnwindBlacklist::BlacklistModule(const void* module) {
   CHECK(module);
   blacklisted_modules_.insert(module);
 }
