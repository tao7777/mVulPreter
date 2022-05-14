std::string GetStoreIdFromProfile(Profile* profile) {
const char* GetStoreIdFromProfile(Profile* profile) {
   DCHECK(profile);
   return profile->IsOffTheRecord() ?
       kOffTheRecordProfileStoreId : kOriginalProfileStoreId;
}
