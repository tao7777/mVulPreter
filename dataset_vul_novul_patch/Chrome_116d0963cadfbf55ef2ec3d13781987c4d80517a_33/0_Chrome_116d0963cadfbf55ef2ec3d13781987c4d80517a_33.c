 void ChromeMockRenderThread::OnScriptedPrint(
     const PrintHostMsg_ScriptedPrint_Params& params,
     PrintMsg_PrintPages_Params* settings) {
  if (print_dialog_user_response_) {
     printer_->ScriptedPrint(params.cookie,
                             params.expected_pages_count,
                             params.has_selection,
                            settings);
  }
}
