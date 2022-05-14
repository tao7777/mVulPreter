GURL CreateResource(const std::string& content, const std::string& file_ext) {
GURL CreateResource(const std::string& content) {
   base::FilePath path;
   EXPECT_TRUE(base::CreateTemporaryFile(&path));
   EXPECT_EQ(static_cast<int>(content.size()),
             base::WriteFile(path, content.c_str(), content.size()));
  return GURL("file:///" + path.AsUTF8Unsafe());
 }
