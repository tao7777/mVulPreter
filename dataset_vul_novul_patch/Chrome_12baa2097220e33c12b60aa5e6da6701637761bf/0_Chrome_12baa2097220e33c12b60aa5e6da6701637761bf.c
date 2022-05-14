 void BookmarksIOFunction::ShowSelectFileDialog(
     ui::SelectFileDialog::Type type,
     const base::FilePath& default_path) {
  if (!dispatcher())
    return;  // Extension was unloaded.

   AddRef();

  WebContents* web_contents = dispatcher()->delegate()->
      GetAssociatedWebContents();

  select_file_dialog_ = ui::SelectFileDialog::Create(
      this, new ChromeSelectFilePolicy(web_contents));
  ui::SelectFileDialog::FileTypeInfo file_type_info;
  file_type_info.extensions.resize(1);
  file_type_info.extensions[0].push_back(FILE_PATH_LITERAL("html"));
  if (type == ui::SelectFileDialog::SELECT_OPEN_FILE)
    file_type_info.support_drive = true;
  select_file_dialog_->SelectFile(type,
                                  string16(),
                                  default_path,
                                  &file_type_info,
                                  0,
                                  FILE_PATH_LITERAL(""),
                                  NULL,
                                  NULL);
}
