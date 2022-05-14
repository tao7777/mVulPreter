 PrintingContext::Result PrintingContext::AskUserForSettings(
    HWND view,
     int max_pages,
     bool has_selection) {
   DCHECK(!in_print_job_);
   dialog_box_dismissed_ = false;

  HWND window;
  if (!view || !IsWindow(view)) {
    // TODO(maruel):  bug 1214347 Get the right browser window instead.
    window = GetDesktopWindow();
  } else {
    window = GetAncestor(view, GA_ROOTOWNER);
  }
  DCHECK(window);

  PRINTDLGEX dialog_options = { sizeof(PRINTDLGEX) };
  dialog_options.hwndOwner = window;
  dialog_options.Flags = PD_RETURNDC | PD_USEDEVMODECOPIESANDCOLLATE  |
                         PD_NOCURRENTPAGE | PD_HIDEPRINTTOFILE;
  if (!has_selection)
    dialog_options.Flags |= PD_NOSELECTION;

  PRINTPAGERANGE ranges[32];
  dialog_options.nStartPage = START_PAGE_GENERAL;
  if (max_pages) {
    memset(ranges, 0, sizeof(ranges));
    ranges[0].nFromPage = 1;
    ranges[0].nToPage = max_pages;
    dialog_options.nPageRanges = 1;
    dialog_options.nMaxPageRanges = arraysize(ranges);
    dialog_options.nMinPage = 1;
    dialog_options.nMaxPage = max_pages;
    dialog_options.lpPageRanges = ranges;
  } else {
    dialog_options.Flags |= PD_NOPAGENUMS;
  }

  {
    if (PrintDlgEx(&dialog_options) != S_OK) {
      ResetSettings();
      return FAILED;
    }
  }
  return ParseDialogResultEx(dialog_options);
}
