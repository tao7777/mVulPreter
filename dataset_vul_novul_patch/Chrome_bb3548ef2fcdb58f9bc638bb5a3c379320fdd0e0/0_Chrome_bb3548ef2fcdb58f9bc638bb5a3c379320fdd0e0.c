void HistoryController::UpdateForCommit(RenderFrameImpl* frame,
                                        const WebHistoryItem& item,
                                        WebHistoryCommitType commit_type,
                                        bool navigation_within_page) {
  switch (commit_type) {
     case blink::WebBackForwardCommit:
       if (!provisional_entry_)
         return;
      // Commit the provisional entry, but only if this back/forward item
      // matches it.  Otherwise it could be a commit from an earlier attempt to
      // go back/forward, and we should leave the provisional entry in place.
      if (HistoryEntry::HistoryNode* node =
              provisional_entry_->GetHistoryNodeForFrame(frame)) {
        if (node->item().itemSequenceNumber() == item.itemSequenceNumber())
          current_entry_.reset(provisional_entry_.release());
      }
       if (HistoryEntry::HistoryNode* node =
               current_entry_->GetHistoryNodeForFrame(frame)) {
         node->set_item(item);
      }
      break;
    case blink::WebStandardCommit:
      CreateNewBackForwardItem(frame, item, navigation_within_page);
      break;
    case blink::WebInitialCommitInChildFrame:
      UpdateForInitialLoadInChildFrame(frame, item);
      break;
    case blink::WebHistoryInertCommit:
      if (current_entry_) {
        if (HistoryEntry::HistoryNode* node =
                current_entry_->GetHistoryNodeForFrame(frame)) {
          if (!navigation_within_page)
            node->RemoveChildren();
          node->set_item(item);
        }
      }
      break;
    default:
      NOTREACHED() << "Invalid commit type: " << commit_type;
  }
}
