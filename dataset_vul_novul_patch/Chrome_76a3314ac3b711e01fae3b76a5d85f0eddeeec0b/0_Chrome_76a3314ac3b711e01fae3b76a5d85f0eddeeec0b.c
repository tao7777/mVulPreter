 bool BaseSettingChange::Init(Profile* profile) {
  DCHECK(profile && !profile_);
   profile_ = profile;
   return true;
 }
