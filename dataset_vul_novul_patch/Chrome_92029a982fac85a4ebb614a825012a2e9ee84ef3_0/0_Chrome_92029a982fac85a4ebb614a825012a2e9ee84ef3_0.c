void FolderHeaderView::SetFolderItem(AppListFolderItem* folder_item) {
  if (folder_item_)
    folder_item_->RemoveObserver(this);

  folder_item_ = folder_item;
  if (!folder_item_)
     return;
   folder_item_->AddObserver(this);
 
  folder_name_view_->SetEnabled(folder_item_->folder_type() !=
                                 AppListFolderItem::FOLDER_TYPE_OEM);

   Update();
 }
