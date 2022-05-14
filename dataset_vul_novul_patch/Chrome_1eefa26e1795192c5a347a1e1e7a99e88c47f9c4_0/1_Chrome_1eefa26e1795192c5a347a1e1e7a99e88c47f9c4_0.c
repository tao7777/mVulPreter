  void RunTestsInFile(const std::string& filename,
                      const std::string& pdf_filename) {
    extensions::ResultCatcher catcher;

    GURL url(embedded_test_server()->GetURL("/pdf/" + pdf_filename));

    ASSERT_TRUE(LoadPdf(url));
    content::WebContents* contents =
        browser()->tab_strip_model()->GetActiveWebContents();
    content::BrowserPluginGuestManager* guest_manager =
        contents->GetBrowserContext()->GetGuestManager();
    content::WebContents* guest_contents =
        guest_manager->GetFullPageGuest(contents);
     ASSERT_TRUE(guest_contents);
 
     base::FilePath test_data_dir;
    PathService::Get(chrome::DIR_TEST_DATA, &test_data_dir);
    test_data_dir = test_data_dir.Append(FILE_PATH_LITERAL("pdf"));
    base::FilePath test_util_path = test_data_dir.AppendASCII("test_util.js");
    std::string test_util_js;
    ASSERT_TRUE(base::ReadFileToString(test_util_path, &test_util_js));

    base::FilePath test_file_path = test_data_dir.AppendASCII(filename);
    std::string test_js;
    ASSERT_TRUE(base::ReadFileToString(test_file_path, &test_js));

    test_util_js.append(test_js);
    ASSERT_TRUE(content::ExecuteScript(guest_contents, test_util_js));

    if (!catcher.GetNextResult())
      FAIL() << catcher.message();
  }
