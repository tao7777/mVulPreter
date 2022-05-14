 bool PermissionsContainsFunction::RunImpl() {
   scoped_ptr<Contains::Params> params(Contains::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params);
 
  ExtensionPrefs* prefs = ExtensionSystem::Get(profile_)->extension_prefs();
   scoped_refptr<PermissionSet> permissions =
      helpers::UnpackPermissionSet(params->permissions,
                                   prefs->AllowFileAccess(extension_->id()),
                                   &error_);
   if (!permissions.get())
     return false;
 
  results_ = Contains::Results::Create(
      GetExtension()->GetActivePermissions()->Contains(*permissions));
  return true;
}
