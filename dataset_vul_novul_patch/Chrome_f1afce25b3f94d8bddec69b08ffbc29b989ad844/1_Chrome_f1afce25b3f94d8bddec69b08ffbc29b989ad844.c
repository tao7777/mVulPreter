  void TestAppInstancesHelper(const std::string& app_name) {
    LOG(INFO) << "Start of test.";

    extensions::ProcessMap* process_map =
        extensions::ProcessMap::Get(browser()->profile());

    ASSERT_TRUE(LoadExtension(
        test_data_dir_.AppendASCII(app_name)));
    const Extension* extension = GetSingleLoadedExtension();

    GURL base_url = GetTestBaseURL(app_name);

    ui_test_utils::NavigateToURLWithDisposition(
        browser(), base_url.Resolve("path1/empty.html"),
        WindowOpenDisposition::NEW_FOREGROUND_TAB,
        ui_test_utils::BROWSER_TEST_WAIT_FOR_NAVIGATION);
    LOG(INFO) << "Nav 1.";
    EXPECT_TRUE(process_map->Contains(
        browser()->tab_strip_model()->GetWebContentsAt(1)->
            GetRenderProcessHost()->GetID()));
    EXPECT_FALSE(browser()->tab_strip_model()->GetWebContentsAt(1)->GetWebUI());

    content::WindowedNotificationObserver tab_added_observer(
        chrome::NOTIFICATION_TAB_ADDED,
        content::NotificationService::AllSources());
    chrome::NewTab(browser());
    tab_added_observer.Wait();
    LOG(INFO) << "New tab.";
    ui_test_utils::NavigateToURL(browser(),
                                 base_url.Resolve("path2/empty.html"));
    LOG(INFO) << "Nav 2.";
    EXPECT_TRUE(process_map->Contains(
        browser()->tab_strip_model()->GetWebContentsAt(2)->
            GetRenderProcessHost()->GetID()));
    EXPECT_FALSE(browser()->tab_strip_model()->GetWebContentsAt(2)->GetWebUI());

    ASSERT_EQ(3, browser()->tab_strip_model()->count());
    WebContents* tab1 = browser()->tab_strip_model()->GetWebContentsAt(1);
    WebContents* tab2 = browser()->tab_strip_model()->GetWebContentsAt(2);
    EXPECT_NE(tab1->GetRenderProcessHost(), tab2->GetRenderProcessHost());

     ASSERT_EQ(1u, chrome::GetBrowserCount(browser()->profile()));
    OpenWindow(tab1, base_url.Resolve("path1/empty.html"), true, NULL);
     LOG(INFO) << "WindowOpenHelper 1.";
    OpenWindow(tab2, base_url.Resolve("path2/empty.html"), true, NULL);
     LOG(INFO) << "End of test.";
     UnloadExtension(extension->id());
   }
