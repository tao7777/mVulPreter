void testRandomDecodeAfterClearFrameBufferCache(const char* gifFile)
void testRandomDecodeAfterClearFrameBufferCache(const char* dir, const char* gifFile)
 {
     SCOPED_TRACE(gifFile);
 
    RefPtr<SharedBuffer> data = readFile(dir, gifFile);
     ASSERT_TRUE(data.get());
     Vector<unsigned> baselineHashes;
     createDecodingBaseline(data.get(), &baselineHashes);
    size_t frameCount = baselineHashes.size();

    OwnPtr<GIFImageDecoder> decoder = createDecoder();
    decoder->setData(data.get(), true);
    for (size_t clearExceptFrame = 0; clearExceptFrame < frameCount; ++clearExceptFrame) {
        decoder->clearCacheExceptFrame(clearExceptFrame);
        const size_t skippingStep = 5;
        for (size_t i = 0; i < skippingStep; ++i) {
            for (size_t j = 0; j < frameCount; j += skippingStep) {
                SCOPED_TRACE(testing::Message() << "Random i:" << i << " j:" << j);
                ImageFrame* frame = decoder->frameBufferAtIndex(j);
                EXPECT_EQ(baselineHashes[j], hashSkBitmap(frame->getSkBitmap()));
            }
        }
    }
}
