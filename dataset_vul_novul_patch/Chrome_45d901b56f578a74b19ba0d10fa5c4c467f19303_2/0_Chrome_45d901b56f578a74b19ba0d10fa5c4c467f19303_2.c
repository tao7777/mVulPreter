 void TabStrip::ChangeTabGroup(int model_index,
                               base::Optional<int> old_group,
                               base::Optional<int> new_group) {
  tab_at(model_index)->SetGroup(new_group);
   if (new_group.has_value() && !group_headers_[new_group.value()]) {
    auto header = std::make_unique<TabGroupHeader>(this, new_group.value());
     header->set_owned_by_client();
     AddChildView(header.get());
     group_headers_[new_group.value()] = std::move(header);
  }
  if (old_group.has_value() &&
      controller_->ListTabsInGroup(old_group.value()).size() == 0) {
    group_headers_.erase(old_group.value());
  }
  UpdateIdealBounds();
  AnimateToIdealBounds();
}
