void SyncTest::TriggerSetSyncTabs() {
  ASSERT_TRUE(ServerSupportsErrorTriggering());
  std::string path = "chromiumsync/synctabs";
  ui_test_utils::NavigateToURL(browser(), sync_server_.GetURL(path));
  ASSERT_EQ("Sync Tabs",
            UTF16ToASCII(browser()->GetSelectedWebContents()->GetTitle()));
}
