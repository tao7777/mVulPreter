  void ImageFetched(const ContentSuggestion::ID& id,
                    const GURL& url,
                    const base::string16& title,
                    const base::string16& text,
                    base::Time timeout_at,
                    const gfx::Image& image) {
    if (!ShouldNotifyInState(app_status_listener_.GetState())) {
      return;  // Became foreground while we were fetching the image; forget it.
    }
     DVLOG(1) << "Fetched " << image.Size().width() << "x"
              << image.Size().height() << " image for " << url.spec();
    ConsumeQuota(profile_->GetPrefs());
     if (ContentSuggestionsNotificationHelper::SendNotification(
             id, url, title, text, CropSquare(image), timeout_at)) {
       RecordContentSuggestionsNotificationImpression(
          id.category().IsKnownCategory(KnownCategories::ARTICLES)
              ? CONTENT_SUGGESTIONS_ARTICLE
              : CONTENT_SUGGESTIONS_NONARTICLE);
    }
  }
