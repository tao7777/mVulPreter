 Node::InsertionNotificationRequest HTMLStyleElement::InsertedInto(
     ContainerNode* insertion_point) {
   HTMLElement::InsertedInto(insertion_point);
  if (isConnected()) {
    if (StyleElement::ProcessStyleSheet(GetDocument(), *this) ==
        StyleElement::kProcessingFatalError) {
      NotifyLoadedSheetAndAllCriticalSubresources(
          kErrorOccurredLoadingSubresource);
    }
  }
  return kInsertionDone;
 }
