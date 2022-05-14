void Browser::FindInPage(bool find_next, bool forward_direction) {
  ShowFindBar();
  if (find_next) {
    string16 find_text;
#if defined(OS_MACOSX)
     find_text = GetFindPboardText();
 #endif
     GetSelectedTabContentsWrapper()->
        GetFindManager()->StartFinding(find_text,
                                       forward_direction,
                                       false);  // Not case sensitive.
   }
 }
