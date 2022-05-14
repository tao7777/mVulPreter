 void HTMLImportsController::Dispose() {
  // TODO(tkent): We copy loaders_ before iteration to avoid crashes.
  // This copy should be unnecessary. loaders_ is not modified during
  // the iteration.  Also, null-check for |loader| should be
  // unnecessary.  crbug.com/843151.
  LoaderList list;
  list.swap(loaders_);
  for (const auto& loader : list) {
    if (loader)
      loader->Dispose();
  }
 
   if (root_) {
     root_->Dispose();
    root_.Clear();
  }
}
