long Track::Create(
Track::Info::Info()
    : uid(0),
      defaultDuration(0),
      codecDelay(0),
      seekPreRoll(0),
      nameAsUTF8(NULL),
      language(NULL),
      codecId(NULL),
      codecNameAsUTF8(NULL),
      codecPrivate(NULL),
      codecPrivateSize(0),
      lacing(false) {}
 
Track::Info::~Info() { Clear(); }
 
void Track::Info::Clear() {
  delete[] nameAsUTF8;
  nameAsUTF8 = NULL;
 
  delete[] language;
  language = NULL;
 
  delete[] codecId;
  codecId = NULL;
 
  delete[] codecPrivate;
  codecPrivate = NULL;
  codecPrivateSize = 0;

  delete[] codecNameAsUTF8;
  codecNameAsUTF8 = NULL;
 }
