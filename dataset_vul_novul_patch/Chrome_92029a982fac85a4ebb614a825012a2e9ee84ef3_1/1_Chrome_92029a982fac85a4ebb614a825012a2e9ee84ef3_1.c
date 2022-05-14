void FolderHeaderView::Update() {
  if (!folder_item_)
     return;
 
   folder_name_view_->SetVisible(folder_name_visible_);
  if (folder_name_visible_)
     folder_name_view_->SetText(base::UTF8ToUTF16(folder_item_->name()));
 
   Layout();
 }
