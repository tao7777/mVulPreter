   SessionHistoryTest() : UITest() {
    FilePath path(test_data_directory_);
    path = path.AppendASCII("session_history");
    url_prefix_ = UTF8ToWide(net::FilePathToFileURL(path).spec());
   }
