void FolderHeaderView::ContentsChanged(views::Textfield* sender,
                                       const base::string16& new_contents) {
   if (!folder_item_)
     return;
 
  folder_name_view_->Update();
   folder_item_->RemoveObserver(this);
  // Enforce the maximum folder name length in UI.
  std::string name = base::UTF16ToUTF8(
      folder_name_view_->text().substr(0, kMaxFolderNameChars));
  if (name != folder_item_->name())
    delegate_->SetItemName(folder_item_, name);
   folder_item_->AddObserver(this);
 
   Layout();
}
