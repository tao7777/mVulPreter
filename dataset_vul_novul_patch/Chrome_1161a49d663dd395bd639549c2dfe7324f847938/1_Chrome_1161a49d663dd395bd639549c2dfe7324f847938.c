bool BookmarkNodeData::Read(const ui::OSExchangeData& data) {
  elements.clear();

  profile_path_.clear();

  if (data.HasCustomFormat(GetBookmarkCustomFormat())) {
    Pickle drag_data_pickle;
    if (data.GetPickledData(GetBookmarkCustomFormat(), &drag_data_pickle)) {
      if (!ReadFromPickle(&drag_data_pickle))
        return false;
    }
  } else {
     Element element;
     GURL url;
     base::string16 title;
    if (data.GetURLAndTitle(&url, &title))
       ReadFromTuple(url, title);
   }
 
  return is_valid();
}
