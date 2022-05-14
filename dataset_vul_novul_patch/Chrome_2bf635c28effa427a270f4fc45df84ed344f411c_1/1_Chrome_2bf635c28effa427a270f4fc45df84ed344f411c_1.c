void SVGStyleElement::DidNotifySubtreeInsertionsToDocument() {
  if (StyleElement::ProcessStyleSheet(GetDocument(), *this) ==
      StyleElement::kProcessingFatalError)
    NotifyLoadedSheetAndAllCriticalSubresources(
        kErrorOccurredLoadingSubresource);
 }
