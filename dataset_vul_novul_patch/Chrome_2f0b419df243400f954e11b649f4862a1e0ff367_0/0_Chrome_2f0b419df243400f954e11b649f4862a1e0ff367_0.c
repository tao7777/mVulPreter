 InputImeEventRouter* InputImeEventRouterFactory::GetRouter(Profile* profile) {
   if (!profile)
     return nullptr;
  // The |router_map_| is keyed by the original profile.
  // Refers to the comments in |RemoveProfile| method for the reason.
  profile = profile->GetOriginalProfile();
   InputImeEventRouter* router = router_map_[profile];
   if (!router) {
    // The router must attach to the profile from which the extension can
    // receive events. If |profile| has an off-the-record profile, attach the
    // off-the-record profile. e.g. In guest mode, the extension is running with
    // the incognito profile instead of its original profile.
    router = new InputImeEventRouter(profile->HasOffTheRecordProfile()
                                         ? profile->GetOffTheRecordProfile()
                                         : profile);
     router_map_[profile] = router;
   }
   return router;
}
