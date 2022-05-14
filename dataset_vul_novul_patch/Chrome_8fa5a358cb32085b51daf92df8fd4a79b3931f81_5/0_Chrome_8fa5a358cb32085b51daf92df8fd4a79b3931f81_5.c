 void PrintWebViewHelper::OnPrintingDone(bool success) {
  CHECK_LE(ipc_nesting_level_, 1);
   notify_browser_of_print_failure_ = false;
   if (!success)
     LOG(ERROR) << "Failure in OnPrintingDone";
  DidFinishPrinting(success ? OK : FAIL_PRINT);
}
