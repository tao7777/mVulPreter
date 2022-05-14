ExtensionFunction::ResponseAction TabsCaptureVisibleTabFunction::Run() {
  using api::extension_types::ImageDetails;

  EXTENSION_FUNCTION_VALIDATE(args_);

  int context_id = extension_misc::kCurrentWindowId;
  args_->GetInteger(0, &context_id);

  std::unique_ptr<ImageDetails> image_details;
  if (args_->GetSize() > 1) {
    base::Value* spec = NULL;
    EXTENSION_FUNCTION_VALIDATE(args_->Get(1, &spec) && spec);
    image_details = ImageDetails::FromValue(*spec);
  }
 
   std::string error;
   WebContents* contents = GetWebContentsForID(context_id, &error);
  if (!contents)
    return RespondNow(Error(error));
 
   const CaptureResult capture_result = CaptureAsync(
       contents, image_details.get(),
      base::BindOnce(&TabsCaptureVisibleTabFunction::CopyFromSurfaceComplete,
                     this));
  if (capture_result == OK) {
    return did_respond() ? AlreadyResponded() : RespondLater();
  }

  return RespondNow(Error(CaptureResultToErrorMessage(capture_result)));
}
