HRESULT StreamOnHGlobalToString(IStream* stream, std::string* out) {
  DCHECK(stream);
  DCHECK(out);
  HGLOBAL hdata = NULL;
   HRESULT hr = GetHGlobalFromStream(stream, &hdata);
   if (SUCCEEDED(hr)) {
     DCHECK(hdata);
    base::win::ScopedHGlobal<char> locked_data(hdata);
     out->assign(locked_data.release(), locked_data.Size());
   }
   return hr;
}
