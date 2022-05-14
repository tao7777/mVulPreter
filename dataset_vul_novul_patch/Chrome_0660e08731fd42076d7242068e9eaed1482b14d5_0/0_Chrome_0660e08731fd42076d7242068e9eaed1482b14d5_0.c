bool PageCaptureSaveAsMHTMLFunction::RunAsync() {
  params_ = SaveAsMHTML::Params::Create(*args_);
  EXTENSION_FUNCTION_VALIDATE(params_.get());

  AddRef();  // Balanced in ReturnFailure/ReturnSuccess()

#if defined(OS_CHROMEOS)
  if (profiles::ArePublicSessionRestrictionsEnabled()) {
    WebContents* web_contents = GetWebContents();
    if (!web_contents) {
      ReturnFailure(kTabClosedError);
      return true;
    }
    auto callback =
        base::Bind(&PageCaptureSaveAsMHTMLFunction::ResolvePermissionRequest,
                   base::Unretained(this));
    permission_helper::HandlePermissionRequest(
        *extension(), {APIPermission::kPageCapture}, web_contents, callback,
        permission_helper::PromptFactory());
    return true;
   }
 #endif
 
  if (!CanCaptureCurrentPage()) {
    return false;
  }
   base::PostTaskWithTraits(
       FROM_HERE, kCreateTemporaryFileTaskTraits,
       base::BindOnce(&PageCaptureSaveAsMHTMLFunction::CreateTemporaryFile,
                      this));
   return true;
 }
