  void CreateFile(const FilePath& file_path) {
    const std::string kFoo = "foo";
    ASSERT_TRUE(file_util::WriteFile(file_path, kFoo.data(), kFoo.size()))
        << ": " << file_path.value();
  }
