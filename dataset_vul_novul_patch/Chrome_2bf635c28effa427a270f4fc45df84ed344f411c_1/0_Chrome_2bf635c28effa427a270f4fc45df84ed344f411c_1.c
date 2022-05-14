void SVGStyleElement::DidNotifySubtreeInsertionsToDocument() {
  if (isConnected()) {
    if (StyleElement::ProcessStyleSheet(GetDocument(), *this) ==
        StyleElement::kProcessingFatalError) {
      NotifyLoadedSheetAndAllCriticalSubresources(
          kErrorOccurredLoadingSubresource);
    }
  }
  return kInsertionDone;
 }
