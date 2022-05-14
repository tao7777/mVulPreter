  ExtensionScriptAndCaptureVisibleTest()
      : http_url("http://www.google.com"),
        http_url_with_path("http://www.google.com/index.html"),
        https_url("https://www.google.com"),
        example_com("https://example.com"),
        test_example_com("https://test.example.com"),
        sample_example_com("https://sample.example.com"),
        file_url("file:///foo/bar"),
        favicon_url("chrome://favicon/http://www.google.com"),
        extension_url("chrome-extension://" +
                       crx_file::id_util::GenerateIdForPath(
                           base::FilePath(FILE_PATH_LITERAL("foo")))),
         settings_url("chrome://settings"),
        about_url("about:flags") {
     urls_.insert(http_url);
     urls_.insert(http_url_with_path);
     urls_.insert(https_url);
    urls_.insert(example_com);
    urls_.insert(test_example_com);
    urls_.insert(sample_example_com);
    urls_.insert(file_url);
     urls_.insert(favicon_url);
     urls_.insert(extension_url);
     urls_.insert(settings_url);
    urls_.insert(about_url);
     PermissionsData::SetPolicyDelegate(NULL);
   }
