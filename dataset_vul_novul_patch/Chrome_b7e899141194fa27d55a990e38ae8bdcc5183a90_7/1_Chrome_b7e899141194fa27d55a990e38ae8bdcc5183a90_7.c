std::string GetStoreIdFromProfile(Profile* profile) {
   DCHECK(profile);
   return profile->IsOffTheRecord() ?
       kOffTheRecordProfileStoreId : kOriginalProfileStoreId;
}
