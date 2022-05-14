 void PrintDialogGtk::CreateDialogImpl(const FilePath& path) {
  // Only show one print dialog at once. This is to prevent a page from
  // locking up the system with
  //
  //   while(true){print();}
  AutoLock lock(DialogLock());
  if (g_print_dialog)
    return;

  g_print_dialog = new PrintDialogGtk(path);
 }
