 bool PermissionsRemoveFunction::RunImpl() {
   scoped_ptr<Remove::Params> params(Remove::Params::Create(*args_));
  EXTENSION_FUNCTION_VALIDATE(params);
 
  ExtensionPrefs* prefs = ExtensionSystem::Get(profile_)->extension_prefs();
   scoped_refptr<PermissionSet> permissions =
      helpers::UnpackPermissionSet(params->permissions,
                                   prefs->AllowFileAccess(extension_->id()),
                                   &error_);
   if (!permissions.get())
     return false;
 
  const extensions::Extension* extension = GetExtension();

  APIPermissionSet apis = permissions->apis();
  for (APIPermissionSet::const_iterator i = apis.begin();
       i != apis.end(); ++i) {
    if (!i->info()->supports_optional()) {
      error_ = ErrorUtils::FormatErrorMessage(
          kNotWhitelistedError, i->name());
      return false;
    }
  }

  const PermissionSet* required = extension->required_permission_set();
  scoped_refptr<PermissionSet> intersection(
      PermissionSet::CreateIntersection(permissions.get(), required));
  if (!intersection->IsEmpty()) {
    error_ = kCantRemoveRequiredPermissionsError;
    results_ = Remove::Results::Create(false);
    return false;
  }

  PermissionsUpdater(profile()).RemovePermissions(extension, permissions.get());
  results_ = Remove::Results::Create(true);
  return true;
}
