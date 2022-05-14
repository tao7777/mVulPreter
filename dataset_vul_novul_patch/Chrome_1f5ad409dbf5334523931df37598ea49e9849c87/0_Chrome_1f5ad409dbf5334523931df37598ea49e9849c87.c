  void TestProcessOverflow() {
    int tab_count = 1;
    int host_count = 1;
    WebContents* tab1 = NULL;
    WebContents* tab2 = NULL;
    content::RenderProcessHost* rph1 = NULL;
    content::RenderProcessHost* rph2 = NULL;
    content::RenderProcessHost* rph3 = NULL;

    const extensions::Extension* extension =
        LoadExtension(test_data_dir_.AppendASCII("options_page"));

    GURL omnibox(chrome::kChromeUIOmniboxURL);
    ui_test_utils::NavigateToURL(browser(), omnibox);
    EXPECT_EQ(tab_count, browser()->tab_strip_model()->count());
    tab1 = browser()->tab_strip_model()->GetWebContentsAt(tab_count - 1);
    rph1 = tab1->GetMainFrame()->GetProcess();
    EXPECT_EQ(omnibox, tab1->GetURL());
    EXPECT_EQ(host_count, RenderProcessHostCount());

    GURL page1("data:text/html,hello world1");

    ui_test_utils::WindowedTabAddedNotificationObserver observer1(
        content::NotificationService::AllSources());
    ::ShowSingletonTab(browser(), page1);
    observer1.Wait();

    tab_count++;
    host_count++;
    EXPECT_EQ(tab_count, browser()->tab_strip_model()->count());
    tab1 = browser()->tab_strip_model()->GetWebContentsAt(tab_count - 1);
    rph2 = tab1->GetMainFrame()->GetProcess();
    EXPECT_EQ(tab1->GetURL(), page1);
    EXPECT_EQ(host_count, RenderProcessHostCount());
    EXPECT_NE(rph1, rph2);

    GURL page2("data:text/html,hello world2");
    ui_test_utils::WindowedTabAddedNotificationObserver observer2(
        content::NotificationService::AllSources());
    ::ShowSingletonTab(browser(), page2);
    observer2.Wait();
    tab_count++;
    if (content::AreAllSitesIsolatedForTesting())
      host_count++;
    EXPECT_EQ(tab_count, browser()->tab_strip_model()->count());
    tab2 = browser()->tab_strip_model()->GetWebContentsAt(tab_count - 1);
    EXPECT_EQ(tab2->GetURL(), page2);
    EXPECT_EQ(host_count, RenderProcessHostCount());
    if (content::AreAllSitesIsolatedForTesting())
      EXPECT_NE(tab2->GetMainFrame()->GetProcess(), rph2);
     else
       EXPECT_EQ(tab2->GetMainFrame()->GetProcess(), rph2);
 
    // Create another WebUI tab.  Each WebUI tab should get a separate process
    // because of origin locking.
    GURL history(chrome::kChromeUIHistoryURL);
    ui_test_utils::WindowedTabAddedNotificationObserver observer3(
        content::NotificationService::AllSources());
     ::ShowSingletonTab(browser(), history);
     observer3.Wait();
     tab_count++;
    host_count++;
     EXPECT_EQ(tab_count, browser()->tab_strip_model()->count());
     tab2 = browser()->tab_strip_model()->GetWebContentsAt(tab_count - 1);
     EXPECT_EQ(tab2->GetURL(), GURL(history));
     EXPECT_EQ(host_count, RenderProcessHostCount());
    EXPECT_NE(tab2->GetMainFrame()->GetProcess(), rph1);
 
     GURL extension_url("chrome-extension://" + extension->id());
    ui_test_utils::WindowedTabAddedNotificationObserver observer4(
        content::NotificationService::AllSources());
    ::ShowSingletonTab(browser(), extension_url);

    observer4.Wait();
    tab_count++;
    host_count++;
    EXPECT_EQ(tab_count, browser()->tab_strip_model()->count());
    tab1 = browser()->tab_strip_model()->GetWebContentsAt(tab_count - 1);
    rph3 = tab1->GetMainFrame()->GetProcess();
    EXPECT_EQ(tab1->GetURL(), extension_url);
    EXPECT_EQ(host_count, RenderProcessHostCount());
    EXPECT_NE(rph1, rph3);
    EXPECT_NE(rph2, rph3);
  }
