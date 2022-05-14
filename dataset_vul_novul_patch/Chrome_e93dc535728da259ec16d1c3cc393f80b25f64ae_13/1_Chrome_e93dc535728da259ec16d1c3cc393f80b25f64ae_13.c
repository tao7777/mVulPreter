bool OSExchangeDataProviderWin::GetPickledData(
    const OSExchangeData::CustomFormat& format,
    Pickle* data) const {
  DCHECK(data);
  bool success = false;
  STGMEDIUM medium;
   FORMATETC format_etc = format.ToFormatEtc();
   if (SUCCEEDED(source_object_->GetData(&format_etc, &medium))) {
     if (medium.tymed & TYMED_HGLOBAL) {
      base::win::ScopedHGlobal<char> c_data(medium.hGlobal);
       DCHECK_GT(c_data.Size(), 0u);
       *data = Pickle(c_data.get(), static_cast<int>(c_data.Size()));
       success = true;
    }
    ReleaseStgMedium(&medium);
  }
  return success;
}
