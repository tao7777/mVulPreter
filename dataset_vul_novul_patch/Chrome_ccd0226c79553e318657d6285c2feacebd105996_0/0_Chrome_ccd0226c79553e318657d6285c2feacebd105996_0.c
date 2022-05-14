 PrintDialogGtk::~PrintDialogGtk() {
  AutoLock lock(DialogLock());
  DCHECK_EQ(this, g_print_dialog);
  g_print_dialog = NULL;
 }
