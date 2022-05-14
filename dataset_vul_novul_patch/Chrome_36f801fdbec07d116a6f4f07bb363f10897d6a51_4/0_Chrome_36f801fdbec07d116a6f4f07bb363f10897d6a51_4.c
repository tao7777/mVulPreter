void DOMWindow::focus(LocalDOMWindow* incumbent_window) {
  if (!GetFrame())
    return;

  Page* page = GetFrame()->GetPage();
  if (!page)
    return;

  DCHECK(incumbent_window);
  ExecutionContext* incumbent_execution_context =
      incumbent_window->GetExecutionContext();

  bool allow_focus = incumbent_execution_context->IsWindowInteractionAllowed();
  if (allow_focus) {
    incumbent_execution_context->ConsumeWindowInteraction();
  } else {
    DCHECK(IsMainThread());
    allow_focus =
        opener() && (opener() != this) &&
        (ToDocument(incumbent_execution_context)->domWindow() == opener());
  }
 
   if (GetFrame()->IsMainFrame() && allow_focus)
    page->GetChromeClient().Focus(incumbent_window->GetFrame());
 
   page->GetFocusController().FocusDocumentView(GetFrame(),
                                                true /* notifyEmbedder */);
}
