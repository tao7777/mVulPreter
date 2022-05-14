void BrowserActionsContainer::BrowserActionAdded(Extension* extension,
                                                 int index) {
#if defined(DEBUG)
  for (size_t i = 0; i < browser_action_views_.size(); ++i) {
    DCHECK(browser_action_views_[i]->button()->extension() != extension) <<
           "Asked to add a browser action view for an extension that already "
           "exists.";
  }
#endif
  size_t visible_actions = 0;
  for (size_t i = 0; i < browser_action_views_.size(); ++i) {
    if (browser_action_views_[i]->IsVisible())
      ++visible_actions;
  }
  BrowserActionView* view = new BrowserActionView(extension, this);
  browser_action_views_.push_back(view);
  AddChildView(index, view);
  if (visible_actions < browser_action_views_.size() - 1) {
    OnBrowserActionVisibilityChanged();
  } else {
    animation_target_size_ = IconCountToWidth(visible_actions + 1);
    suppress_chevron_ = !chevron_->IsVisible();
    resize_animation_->Reset();
    resize_animation_->SetTweenType(SlideAnimation::NONE);
    resize_animation_->Show();
  }
}
