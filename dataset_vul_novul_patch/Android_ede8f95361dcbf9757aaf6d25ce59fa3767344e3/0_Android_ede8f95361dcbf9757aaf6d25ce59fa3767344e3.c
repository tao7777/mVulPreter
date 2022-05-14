long FrameSequenceState_gif::drawFrame(int frameNr,
 Color8888* outputPtr, int outputPixelStride, int previousFrameNr) {

 GifFileType* gif = mFrameSequence.getGif();
 if (!gif) {
        ALOGD("Cannot drawFrame, mGif is NULL");
 return -1;
 }

#if GIF_DEBUG
    ALOGD("      drawFrame on %p nr %d on addr %p, previous frame nr %d",
 this, frameNr, outputPtr, previousFrameNr);
#endif

 const int height = mFrameSequence.getHeight();
 const int width = mFrameSequence.getWidth();

 GraphicsControlBlock gcb;

 int start = max(previousFrameNr + 1, 0);

 for (int i = max(start - 1, 0); i < frameNr; i++) {
 int neededPreservedFrame = mFrameSequence.getRestoringFrame(i);
 if (neededPreservedFrame >= 0 && (mPreserveBufferFrame != neededPreservedFrame)) {
#if GIF_DEBUG
            ALOGD("frame %d needs frame %d preserved, but %d is currently, so drawing from scratch",
                    i, neededPreservedFrame, mPreserveBufferFrame);
#endif
            start = 0;
 }
 }

 for (int i = start; i <= frameNr; i++) {
 DGifSavedExtensionToGCB(gif, i, &gcb);
 const SavedImage& frame = gif->SavedImages[i];

#if GIF_DEBUG
 bool frameOpaque = gcb.TransparentColor == NO_TRANSPARENT_COLOR;
        ALOGD("producing frame %d, drawing frame %d (opaque %d, disp %d, del %d)",
                frameNr, i, frameOpaque, gcb.DisposalMode, gcb.DelayTime);
#endif
 if (i == 0) {
 Color8888 bgColor = mFrameSequence.getBackgroundColor();
 for (int y = 0; y < height; y++) {
 for (int x = 0; x < width; x++) {
                    outputPtr[y * outputPixelStride + x] = bgColor;
 }
 }
 } else {
 GraphicsControlBlock prevGcb;
 DGifSavedExtensionToGCB(gif, i - 1, &prevGcb);
 const SavedImage& prevFrame = gif->SavedImages[i - 1];
 bool prevFrameDisposed = willBeCleared(prevGcb);

 bool newFrameOpaque = gcb.TransparentColor == NO_TRANSPARENT_COLOR;
 bool prevFrameCompletelyCovered = newFrameOpaque
 && checkIfCover(frame.ImageDesc, prevFrame.ImageDesc);

 if (prevFrameDisposed && !prevFrameCompletelyCovered) {
 switch (prevGcb.DisposalMode) {
 case DISPOSE_BACKGROUND: {
 Color8888* dst = outputPtr + prevFrame.ImageDesc.Left +
                            prevFrame.ImageDesc.Top * outputPixelStride;

 GifWord copyWidth, copyHeight;
                    getCopySize(prevFrame.ImageDesc, width, height, copyWidth, copyHeight);
 for (; copyHeight > 0; copyHeight--) {
                        setLineColor(dst, TRANSPARENT, copyWidth);
                        dst += outputPixelStride;
 }
 } break;
 case DISPOSE_PREVIOUS: {
                    restorePreserveBuffer(outputPtr, outputPixelStride);
 } break;
 }
 }

 if (mFrameSequence.getPreservedFrame(i - 1)) {
                savePreserveBuffer(outputPtr, outputPixelStride, i - 1);
 }
 }

 bool willBeCleared = gcb.DisposalMode == DISPOSE_BACKGROUND
 || gcb.DisposalMode == DISPOSE_PREVIOUS;
 if (i == frameNr || !willBeCleared) {
 const ColorMapObject* cmap = gif->SColorMap;
 if (frame.ImageDesc.ColorMap) {

                 cmap = frame.ImageDesc.ColorMap;
             }
 
            // If a cmap is missing, the frame can't be decoded, so we skip it.
            if (cmap) {
                const unsigned char* src = (unsigned char*)frame.RasterBits;
                Color8888* dst = outputPtr + frame.ImageDesc.Left +
                        frame.ImageDesc.Top * outputPixelStride;
                GifWord copyWidth, copyHeight;
                getCopySize(frame.ImageDesc, width, height, copyWidth, copyHeight);
                for (; copyHeight > 0; copyHeight--) {
                    copyLine(dst, src, cmap, gcb.TransparentColor, copyWidth);
                    src += frame.ImageDesc.Width;
                    dst += outputPixelStride;
                }
             }
         }
     }

 const int maxFrame = gif->ImageCount;
 const int lastFrame = (frameNr + maxFrame - 1) % maxFrame;
 DGifSavedExtensionToGCB(gif, lastFrame, &gcb);
 return getDelayMs(gcb);
}
