int main(int argc, char* argv[])
{
    char* name = argv[0];


    bool applyColorCorrection = false;

#if USE(QCMSLIB)
    if (argc >= 2 && strcmp(argv[1], "--color-correct") == 0)
        applyColorCorrection = (--argc, ++argv, true);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [--color-correct] file [iterations] [packetSize]\n", name);
        exit(1);
    }
#else
    if (argc < 2) {
        fprintf(stderr, "Usage: %s file [iterations] [packetSize]\n", name);
        exit(1);
    }
#endif


    size_t iterations = 1;
    if (argc >= 3) {
        char* end = 0;
        iterations = strtol(argv[2], &end, 10);
        if (*end != '\0' || !iterations) {
            fprintf(stderr, "Second argument should be number of iterations. "
                "The default is 1. You supplied %s\n", argv[2]);
            exit(1);
        }
    }

    size_t packetSize = 0;
    if (argc >= 4) {
        char* end = 0;
        packetSize = strtol(argv[3], &end, 10);
        if (*end != '\0') {
            fprintf(stderr, "Third argument should be packet size. Default is "
                "0, meaning to decode the entire image in one packet. You "
                "supplied %s\n", argv[3]);
            exit(1);
        }
    }


    class WebPlatform : public blink::Platform {
    public:
        const unsigned char* getTraceCategoryEnabledFlag(const char*) override
        {
            return reinterpret_cast<const unsigned char *>("nope-none-nada");
        }
 
         void cryptographicallyRandomValues(unsigned char*, size_t) override
         {
            RELEASE_ASSERT_NOT_REACHED();
         }
 
         void screenColorProfile(WebVector<char>* profile) override
        {
            getScreenColorProfile(profile); // Returns a whacked color profile.
        }
    };

    blink::initializeWithoutV8(new WebPlatform());


#if USE(QCMSLIB)
    ImageDecoder::qcmsOutputDeviceProfile(); // Initialize screen colorProfile.
#endif


    RefPtr<SharedBuffer> data = readFile(argv[1]);
    if (!data.get() || !data->size()) {
        fprintf(stderr, "Error reading image data from [%s]\n", argv[1]);
        exit(2);
    }

    data->data();


    double totalTime = 0.0;

    for (size_t i = 0; i < iterations; ++i) {
        double startTime = getCurrentTime();
        bool decoded = decodeImageData(data.get(), applyColorCorrection, packetSize);
        double elapsedTime = getCurrentTime() - startTime;
        totalTime += elapsedTime;
        if (!decoded) {
            fprintf(stderr, "Image decode failed [%s]\n", argv[1]);
            exit(3);
        }
    }


    double averageTime = totalTime / static_cast<double>(iterations);
    printf("%f %f\n", totalTime, averageTime);
    return 0;
}
