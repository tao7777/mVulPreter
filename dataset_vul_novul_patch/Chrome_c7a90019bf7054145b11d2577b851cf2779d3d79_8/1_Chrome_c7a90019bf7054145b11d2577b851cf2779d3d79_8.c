void PrintWebViewHelper::Print(WebKit::WebFrame* frame, WebKit::WebNode* node) {
  if (print_web_view_)
    return;
 
   scoped_ptr<PrepareFrameAndViewForPrint> prepare;
  if (!InitPrintSettingsAndPrepareFrame(frame, node, &prepare))
     return;  // Failed to init print page settings.
 
   int expected_page_count = 0;
   bool use_browser_overlays = true;

  expected_page_count = prepare->GetExpectedPageCount();
  if (expected_page_count)
    use_browser_overlays = prepare->ShouldUseBrowserOverlays();

  prepare.reset();

  if (!expected_page_count) {
    DidFinishPrinting(OK);  // Release resources and fail silently.
    return;
  }

  if (!GetPrintSettingsFromUser(frame, expected_page_count,
                                use_browser_overlays)) {
    DidFinishPrinting(OK);  // Release resources and fail silently.
    return;
  }

  if (!RenderPagesForPrint(frame, node, NULL)) {
    LOG(ERROR) << "RenderPagesForPrint failed";
    DidFinishPrinting(FAIL_PRINT);
  }
  ResetScriptedPrintCount();
}
