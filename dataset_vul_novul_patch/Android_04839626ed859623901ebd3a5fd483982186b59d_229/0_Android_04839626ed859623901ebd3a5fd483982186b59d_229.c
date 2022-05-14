mkvparser::IMkvReader::~IMkvReader()
#ifdef _MSC_VER
//// Disable MSVC warnings that suggest making code non-portable.
#pragma warning(disable : 4996)
#endif

mkvparser::IMkvReader::~IMkvReader() {}

void mkvparser::GetVersion(int& major, int& minor, int& build, int& revision) {
  major = 1;
  minor = 0;
  build = 0;
  revision = 28;
 }
