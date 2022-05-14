void RenderViewImpl::OnSwapOut(const ViewMsg_SwapOut_Params& params) {
  OnStop();

  if (!is_swapped_out_) {

    SyncNavigationState();

    webview()->dispatchUnloadEvent();

    SetSwappedOut(true);

    // to chrome::kSwappedOutURL.  If that happens to be to the page we had been
    GURL swappedOutURL(chrome::kSwappedOutURL);
    WebURLRequest request(swappedOutURL);
     webview()->mainFrame()->loadRequest(request);
   }
 
  Send(new ViewHostMsg_SwapOut_ACK(routing_id_, params));
}
