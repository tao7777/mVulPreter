void Track::Info::Clear()
{
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
