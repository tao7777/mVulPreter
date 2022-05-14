void BrowserActionsContainer::BrowserActionRemoved(Extension* extension) {
  if (popup_ && popup_->host()->extension() == extension)
    HidePopup();
  int visible_actions = 0;
  for (size_t i = 0; i < browser_action_views_.size(); ++i) {
    if (browser_action_views_[i]->IsVisible())
      ++visible_actions;
  }
  for (std::vector<BrowserActionView*>::iterator iter =
       browser_action_views_.begin(); iter != browser_action_views_.end();
       ++iter) {
    if ((*iter)->button()->extension() == extension) {
      RemoveChildView(*iter);
      delete *iter;
      browser_action_views_.erase(iter);
      animation_target_size_ =
          ClampToNearestIconCount(IconCountToWidth(visible_actions));
      resize_animation_->Reset();
      resize_animation_->SetTweenType(SlideAnimation::EASE_OUT);
      resize_animation_->Show();
      return;
    }
  }
}
