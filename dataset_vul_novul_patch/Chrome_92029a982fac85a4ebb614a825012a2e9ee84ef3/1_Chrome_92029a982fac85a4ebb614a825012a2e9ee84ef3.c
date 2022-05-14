void FolderHeaderView::ContentsChanged(views::Textfield* sender,
                                       const base::string16& new_contents) {
   if (!folder_item_)
     return;
 
   folder_item_->RemoveObserver(this);
  std::string name = base::UTF16ToUTF8(folder_name_view_->text());
  delegate_->SetItemName(folder_item_, name);
   folder_item_->AddObserver(this);
 
   Layout();
}
