 void ExtensionBrowserTest::OpenWindow(content::WebContents* contents,
                                       const GURL& url,
                                       bool newtab_process_should_equal_opener,
                                      bool should_succeed,
                                       content::WebContents** newtab_result) {
   content::WebContentsAddedObserver tab_added_observer;
   ASSERT_TRUE(content::ExecuteScript(contents,
                                      "window.open('" + url.spec() + "');"));
   content::WebContents* newtab = tab_added_observer.GetWebContents();
   ASSERT_TRUE(newtab);
   WaitForLoadStop(newtab);

  if (should_succeed) {
    EXPECT_EQ(url, newtab->GetLastCommittedURL());
    EXPECT_EQ(content::PAGE_TYPE_NORMAL,
              newtab->GetController().GetLastCommittedEntry()->GetPageType());
  } else {
    // "Failure" comes in two forms: redirecting to about:blank or showing an
    // error page. At least one should be true.
    EXPECT_TRUE(
        newtab->GetLastCommittedURL() == GURL(url::kAboutBlankURL) ||
        newtab->GetController().GetLastCommittedEntry()->GetPageType() ==
            content::PAGE_TYPE_ERROR);
  }

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
