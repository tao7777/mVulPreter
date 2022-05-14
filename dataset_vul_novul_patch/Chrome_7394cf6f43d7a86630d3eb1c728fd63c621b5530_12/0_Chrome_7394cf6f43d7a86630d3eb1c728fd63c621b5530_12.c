  void OpenTwoTabs(const GURL& first_url, const GURL& second_url) {
    content::WindowedNotificationObserver load1(
        content::NOTIFICATION_LOAD_COMPLETED_MAIN_FRAME,
        content::NotificationService::AllSources());
     OpenURLParams open1(first_url, content::Referrer(),
                         WindowOpenDisposition::CURRENT_TAB,
                         ui::PAGE_TRANSITION_TYPED, false);
    content::WebContents* web_contents = browser()->OpenURL(open1);
     load1.Wait();
    if (URLShouldBeStoredInLocalDatabase(first_url))
      testing::ExpireLocalDBObservationWindows(web_contents);
 
     content::WindowedNotificationObserver load2(
         content::NOTIFICATION_LOAD_COMPLETED_MAIN_FRAME,
         content::NotificationService::AllSources());
     OpenURLParams open2(second_url, content::Referrer(),
                         WindowOpenDisposition::NEW_BACKGROUND_TAB,
                         ui::PAGE_TRANSITION_TYPED, false);
    web_contents = browser()->OpenURL(open2);
     load2.Wait();
    // Expire all the observation windows to prevent the discarding and freezing
    // interventions to fail because of a lack of observations.
    if (URLShouldBeStoredInLocalDatabase(second_url))
      testing::ExpireLocalDBObservationWindows(web_contents);
 
     ASSERT_EQ(2, tsm()->count());
   }
