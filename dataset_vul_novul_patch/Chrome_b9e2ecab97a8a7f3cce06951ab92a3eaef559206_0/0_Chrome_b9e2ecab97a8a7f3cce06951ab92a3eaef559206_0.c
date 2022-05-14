void NavigationController::RendererDidNavigateNewSubframe(
    const ViewHostMsg_FrameNavigate_Params& params) {
  if (PageTransition::StripQualifier(params.transition) ==
      PageTransition::AUTO_SUBFRAME) {
     return;
   }
 
  DCHECK(GetLastCommittedEntry()) << "ClassifyNavigation should guarantee "
                                  << "that a last committed entry exists.";
  NavigationEntry* new_entry = new NavigationEntry(*GetLastCommittedEntry());
  new_entry->set_page_id(params.page_id);
  InsertOrReplaceEntry(new_entry, false);
}
