PassRefPtr<SharedBuffer> readFile(const char* fileName)
 {
     String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
     filePath.append(fileName);
 
     return Platform::current()->unitTestSupport()->readFromFile(filePath);
}
