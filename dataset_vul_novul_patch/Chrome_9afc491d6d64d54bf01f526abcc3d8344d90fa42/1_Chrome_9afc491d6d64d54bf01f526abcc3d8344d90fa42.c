GURL CreateResource(const std::string& content, const std::string& file_ext) {
   base::FilePath path;
   EXPECT_TRUE(base::CreateTemporaryFile(&path));
   EXPECT_EQ(static_cast<int>(content.size()),
             base::WriteFile(path, content.c_str(), content.size()));
  base::FilePath path_with_extension;
  path_with_extension = path.AddExtension(FILE_PATH_LITERAL(file_ext));
  EXPECT_TRUE(base::Move(path, path_with_extension));
  return net::FilePathToFileURL(path_with_extension);
 }
