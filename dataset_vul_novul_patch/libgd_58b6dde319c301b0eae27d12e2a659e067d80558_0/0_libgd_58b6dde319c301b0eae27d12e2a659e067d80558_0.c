 int main()
{
    check_file("heap_overflow_1.tga");
    check_file("heap_overflow_2.tga");

    return gdNumFailures();
}


static void check_file(char *basename)
 {
     gdImagePtr im;
     char *buffer;
     size_t size;
 
    size = read_test_file(&buffer, basename);
     im = gdImageCreateFromTgaPtr(size, (void *) buffer);
     gdTestAssert(im == NULL);
     free(buffer);
 }
