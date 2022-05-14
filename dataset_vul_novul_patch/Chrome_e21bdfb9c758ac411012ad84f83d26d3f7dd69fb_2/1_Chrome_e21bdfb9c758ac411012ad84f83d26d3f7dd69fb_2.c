 bool PermissionsRequestFunction::RunImpl() {
   if (!user_gesture() && !ignore_user_gesture_for_tests) {
     error_ = kUserGestureRequiredError;
     return false;
  }

   scoped_ptr<Request::Params> params(Request::Params::Create(*args_));
   EXTENSION_FUNCTION_VALIDATE(params.get());
 
   requested_permissions_ =
      helpers::UnpackPermissionSet(params->permissions, &error_);
   if (!requested_permissions_.get())
     return false;
 
  extensions::ExtensionPrefs* prefs =
      profile()->GetExtensionService()->extension_prefs();
   APIPermissionSet apis = requested_permissions_->apis();
   for (APIPermissionSet::const_iterator i = apis.begin();
       i != apis.end(); ++i) {
    if (!i->info()->supports_optional()) {
      error_ = ErrorUtils::FormatErrorMessage(
          kNotWhitelistedError, i->name());
      return false;
    }
  }

  scoped_refptr<extensions::PermissionSet>
      manifest_required_requested_permissions =
          PermissionSet::ExcludeNotInManifestPermissions(
              requested_permissions_.get());

  if (!GetExtension()->optional_permission_set()->Contains(
          *manifest_required_requested_permissions)) {
    error_ = kNotInOptionalPermissionsError;
    results_ = Request::Results::Create(false);
    return false;
  }

  scoped_refptr<const PermissionSet> granted =
      prefs->GetGrantedPermissions(GetExtension()->id());
  if (granted.get() && granted->Contains(*requested_permissions_)) {
    PermissionsUpdater perms_updater(profile());
    perms_updater.AddPermissions(GetExtension(), requested_permissions_.get());
    results_ = Request::Results::Create(true);
    SendResponse(true);
    return true;
  }

  requested_permissions_ = PermissionSet::CreateDifference(
      requested_permissions_.get(), granted.get());

  AddRef();  // Balanced in InstallUIProceed() / InstallUIAbort().

  bool has_no_warnings = requested_permissions_->GetWarningMessages(
      GetExtension()->GetType()).empty();
  if (auto_confirm_for_tests == PROCEED || has_no_warnings) {
    InstallUIProceed();
  } else if (auto_confirm_for_tests == ABORT) {
    InstallUIAbort(true);
  } else {
    CHECK_EQ(DO_NOT_SKIP, auto_confirm_for_tests);
    install_ui_.reset(new ExtensionInstallPrompt(GetAssociatedWebContents()));
    install_ui_->ConfirmPermissions(
        this, GetExtension(), requested_permissions_.get());
  }

  return true;
}
