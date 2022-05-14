 void ExtensionBrowserTest::OpenWindow(content::WebContents* contents,
                                       const GURL& url,
                                       bool newtab_process_should_equal_opener,
                                       content::WebContents** newtab_result) {
   content::WebContentsAddedObserver tab_added_observer;
   ASSERT_TRUE(content::ExecuteScript(contents,
                                      "window.open('" + url.spec() + "');"));
   content::WebContents* newtab = tab_added_observer.GetWebContents();
   ASSERT_TRUE(newtab);
   WaitForLoadStop(newtab);
  EXPECT_EQ(url, newtab->GetLastCommittedURL());
   if (newtab_process_should_equal_opener) {
     EXPECT_EQ(contents->GetMainFrame()->GetSiteInstance(),
               newtab->GetMainFrame()->GetSiteInstance());
  } else {
    EXPECT_NE(contents->GetMainFrame()->GetSiteInstance(),
              newtab->GetMainFrame()->GetSiteInstance());
  }

  if (newtab_result)
    *newtab_result = newtab;
}
