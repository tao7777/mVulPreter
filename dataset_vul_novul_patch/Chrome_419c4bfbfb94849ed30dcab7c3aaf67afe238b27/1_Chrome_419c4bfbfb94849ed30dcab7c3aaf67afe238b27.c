 void ImageBitmapFactories::ImageBitmapLoader::DidFinishLoading() {
   DOMArrayBuffer* array_buffer = loader_->ArrayBufferResult();
   if (!array_buffer) {
     RejectPromise(kAllocationFailureImageBitmapRejectionReason);
     return;
  }
  ScheduleAsyncImageBitmapDecoding(array_buffer);
}
