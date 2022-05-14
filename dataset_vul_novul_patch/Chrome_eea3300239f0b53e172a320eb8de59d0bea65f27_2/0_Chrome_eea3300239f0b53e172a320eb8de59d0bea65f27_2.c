 void DevToolsUIBindings::RecordEnumeratedHistogram(const std::string& name,
                                                    int sample,
                                                    int boundary_value) {
  if (!frontend_host_)
    return;
   if (!(boundary_value >= 0 && boundary_value <= 100 && sample >= 0 &&
         sample < boundary_value)) {
    frontend_host_->BadMessageRecieved();
    return;
  }
  if (name == kDevToolsActionTakenHistogram)
    UMA_HISTOGRAM_ENUMERATION(name, sample, boundary_value);
  else if (name == kDevToolsPanelShownHistogram)
    UMA_HISTOGRAM_ENUMERATION(name, sample, boundary_value);
  else
    frontend_host_->BadMessageRecieved();
}
