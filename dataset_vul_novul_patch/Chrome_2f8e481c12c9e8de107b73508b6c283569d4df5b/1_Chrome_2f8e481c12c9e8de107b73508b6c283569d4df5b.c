Node::InsertionNotificationRequest HTMLLinkElement::InsertedInto(
    ContainerNode& insertion_point) {
  HTMLElement::InsertedInto(insertion_point);
  LogAddElementIfIsolatedWorldAndInDocument("link", relAttr, hrefAttr);
   if (!insertion_point.isConnected())
     return kInsertionDone;
   DCHECK(isConnected());
   if (!ShouldLoadLink() && IsInShadowTree()) {
     String message = "HTML element <link> is ignored in shadow tree.";
     GetDocument().AddConsoleMessage(ConsoleMessage::Create(
         kJSMessageSource, kWarningMessageLevel, message));
     return kInsertionDone;
   }
 
  GetDocument().GetStyleEngine().AddStyleSheetCandidateNode(*this);
   Process();
 
   if (link_)
    link_->OwnerInserted();

  return kInsertionDone;
}
