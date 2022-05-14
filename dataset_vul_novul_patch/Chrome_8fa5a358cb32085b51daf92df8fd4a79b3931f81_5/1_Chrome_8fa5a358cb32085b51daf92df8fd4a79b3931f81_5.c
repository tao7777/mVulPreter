 void PrintWebViewHelper::OnPrintingDone(bool success) {
   notify_browser_of_print_failure_ = false;
   if (!success)
     LOG(ERROR) << "Failure in OnPrintingDone";
  DidFinishPrinting(success ? OK : FAIL_PRINT);
}
