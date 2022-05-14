bool ClipboardUtil::GetWebCustomData(
    IDataObject* data_object,
    std::map<base::string16, base::string16>* custom_data) {
  DCHECK(data_object && custom_data);

  if (!HasData(data_object, Clipboard::GetWebCustomDataFormatType()))
    return false;

   STGMEDIUM store;
   if (GetData(data_object, Clipboard::GetWebCustomDataFormatType(), &store)) {
     {
      base::win::ScopedHGlobal<char> data(store.hGlobal);
       ReadCustomDataIntoMap(data.get(), data.Size(), custom_data);
     }
     ReleaseStgMedium(&store);
    return true;
  }
  return false;
}
