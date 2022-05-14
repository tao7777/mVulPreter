PassRefPtr<SharedBuffer> readFile(const char* fileName)
const char decodersTestingDir[] = "Source/platform/image-decoders/testing";
const char layoutTestResourcesDir[] = "LayoutTests/fast/images/resources";
const char webTestsDataDir[] = "Source/web/tests/data";

PassRefPtr<SharedBuffer> readFile(const char* dir, const char* fileName)
 {
     String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
    filePath.append("/");
    filePath.append(dir);
    filePath.append("/");
     filePath.append(fileName);
 
     return Platform::current()->unitTestSupport()->readFromFile(filePath);
}
