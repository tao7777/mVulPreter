void RemoteFrame::Navigate(const FrameLoadRequest& passed_request) {
  FrameLoadRequest frame_request(passed_request);

  FrameLoader::SetReferrerForFrameRequest(frame_request);
   FrameLoader::UpgradeInsecureRequest(frame_request.GetResourceRequest(),
                                       frame_request.OriginDocument());
 
   Client()->Navigate(frame_request.GetResourceRequest(),
                      frame_request.ReplacesCurrentItem());
 }
