 bool PermissionsContainsFunction::RunImpl() {
   scoped_ptr<Contains::Params> params(Contains::Params::Create(*args_));
 
   scoped_refptr<PermissionSet> permissions =
      helpers::UnpackPermissionSet(params->permissions, &error_);
   if (!permissions.get())
     return false;
 
  results_ = Contains::Results::Create(
      GetExtension()->GetActivePermissions()->Contains(*permissions));
  return true;
}
