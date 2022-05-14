 void ImageBitmapFactories::ImageBitmapLoader::RejectPromise(
    ImageBitmapRejectionReason reason) {
  switch (reason) {
    case kUndecodableImageBitmapRejectionReason:
      resolver_->Reject(
          DOMException::Create(DOMExceptionCode::kInvalidStateError,
                               "The source image could not be decoded."));
      break;
    case kAllocationFailureImageBitmapRejectionReason:
      resolver_->Reject(
          DOMException::Create(DOMExceptionCode::kInvalidStateError,
                               "The ImageBitmap could not be allocated."));
      break;
     default:
       NOTREACHED();
   }
  loader_.reset();
   factory_->DidFinishLoading(this);
 }
