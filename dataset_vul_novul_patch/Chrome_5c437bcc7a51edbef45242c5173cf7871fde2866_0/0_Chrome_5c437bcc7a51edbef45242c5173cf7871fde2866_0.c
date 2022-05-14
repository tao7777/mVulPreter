void ExtensionOptionsGuest::DidNavigateMainFrame(
    const content::LoadCommittedDetails& details,
    const content::FrameNavigateParams& params) {
  if (attached()) {
    auto guest_zoom_controller =
        ui_zoom::ZoomController::FromWebContents(web_contents());
    guest_zoom_controller->SetZoomMode(
         ui_zoom::ZoomController::ZOOM_MODE_ISOLATED);
     SetGuestZoomLevelToMatchEmbedder();
 
    if (!url::IsSameOriginWith(params.url, options_page_)) {
       bad_message::ReceivedBadMessage(web_contents()->GetRenderProcessHost(),
                                       bad_message::EOG_BAD_ORIGIN);
     }
  }
}
