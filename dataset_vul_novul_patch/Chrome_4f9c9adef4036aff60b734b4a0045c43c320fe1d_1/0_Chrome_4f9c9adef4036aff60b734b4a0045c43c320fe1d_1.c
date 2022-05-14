void testRandomFrameDecode(const char* gifFile)
void testRandomFrameDecode(const char* dir, const char* gifFile)
 {
     SCOPED_TRACE(gifFile);
 
    RefPtr<SharedBuffer> fullData = readFile(dir, gifFile);
     ASSERT_TRUE(fullData.get());
     Vector<unsigned> baselineHashes;
     createDecodingBaseline(fullData.get(), &baselineHashes);
    size_t frameCount = baselineHashes.size();

    OwnPtr<GIFImageDecoder> decoder = createDecoder();
    decoder->setData(fullData.get(), true);
    const size_t skippingStep = 5;
    for (size_t i = 0; i < skippingStep; ++i) {
        for (size_t j = i; j < frameCount; j += skippingStep) {
            SCOPED_TRACE(testing::Message() << "Random i:" << i << " j:" << j);
            ImageFrame* frame = decoder->frameBufferAtIndex(j);
            EXPECT_EQ(baselineHashes[j], hashSkBitmap(frame->getSkBitmap()));
        }
    }

    decoder = createDecoder();
    decoder->setData(fullData.get(), true);
    for (size_t i = frameCount; i; --i) {
        SCOPED_TRACE(testing::Message() << "Reverse i:" << i);
        ImageFrame* frame = decoder->frameBufferAtIndex(i - 1);
        EXPECT_EQ(baselineHashes[i - 1], hashSkBitmap(frame->getSkBitmap()));
     }
 }
