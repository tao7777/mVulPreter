 InProcessBrowserTest::InProcessBrowserTest()
     : browser_(NULL),
       exit_when_last_browser_closes_(true),
       multi_desktop_test_(false)
 #if defined(OS_MACOSX)
       , autorelease_pool_(NULL)
#endif  // OS_MACOSX
    {
#if defined(OS_MACOSX)
  base::FilePath chrome_path;
  CHECK(PathService::Get(base::FILE_EXE, &chrome_path));
  chrome_path = chrome_path.DirName();
  chrome_path = chrome_path.Append(chrome::kBrowserProcessExecutablePath);
  CHECK(PathService::Override(base::FILE_EXE, chrome_path));
#endif  // defined(OS_MACOSX)

  CreateTestServer(base::FilePath(FILE_PATH_LITERAL("chrome/test/data")));
  base::FilePath src_dir;
  CHECK(PathService::Get(base::DIR_SOURCE_ROOT, &src_dir));
  base::FilePath test_data_dir = src_dir.AppendASCII("chrome/test/data");
  embedded_test_server()->ServeFilesFromDirectory(test_data_dir);

  CHECK(PathService::Override(chrome::DIR_TEST_DATA, test_data_dir));
}
