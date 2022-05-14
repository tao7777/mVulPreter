 int main()
 {
     gdImagePtr im;
     char *buffer;
     size_t size;
 
    size = read_test_file(&buffer, "heap_overflow.tga");
     im = gdImageCreateFromTgaPtr(size, (void *) buffer);
     gdTestAssert(im == NULL);
     free(buffer);
    return gdNumFailures();
 }
