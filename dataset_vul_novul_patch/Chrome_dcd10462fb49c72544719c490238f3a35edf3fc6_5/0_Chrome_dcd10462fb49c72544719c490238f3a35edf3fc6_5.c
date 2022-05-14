 DomDistillerViewerSource::DomDistillerViewerSource(
     DomDistillerServiceInterface* dom_distiller_service,
     const std::string& scheme,
    scoped_ptr<DistillerUIHandle> ui_handle)
     : scheme_(scheme),
       dom_distiller_service_(dom_distiller_service),
      distiller_ui_handle_(ui_handle.Pass()) {
 }
