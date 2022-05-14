bool GIFImageReader::parseData(size_t dataPosition, size_t len, GIFImageDecoder::GIFParseQuery query)
{
    if (!len) {
        return true;
    }

    if (len < m_bytesToConsume)
        return true;

    while (len >= m_bytesToConsume) {
        const size_t currentComponentPosition = dataPosition;
        const unsigned char* currentComponent = data(dataPosition);

        dataPosition += m_bytesToConsume;
        len -= m_bytesToConsume;

        switch (m_state) {
        case GIFLZW:
            ASSERT(!m_frames.isEmpty());
            m_frames.last()->addLzwBlock(currentComponentPosition, m_bytesToConsume);
            GETN(1, GIFSubBlock);
            break;

        case GIFLZWStart: {
            ASSERT(!m_frames.isEmpty());
            m_frames.last()->setDataSize(*currentComponent);
            GETN(1, GIFSubBlock);
            break;
        }

        case GIFType: {
            if (!strncmp((char*)currentComponent, "GIF89a", 6))
                m_version = 89;
            else if (!strncmp((char*)currentComponent, "GIF87a", 6))
                m_version = 87;
            else
                return false;
            GETN(7, GIFGlobalHeader);
            break;
         }
 
         case GIFGlobalHeader: {
             m_screenWidth = GETINT16(currentComponent);
             m_screenHeight = GETINT16(currentComponent + 2);
 
            if (m_client && !m_client->setSize(m_screenWidth, m_screenHeight))
                return false;
             const size_t globalColorMapColors = 2 << (currentComponent[4] & 0x07);
 
             if ((currentComponent[4] & 0x80) && globalColorMapColors > 0) { /* global map */
                m_globalColorMap.setTablePositionAndSize(dataPosition, globalColorMapColors);
                GETN(BYTES_PER_COLORMAP_ENTRY * globalColorMapColors, GIFGlobalColormap);
                break;
            }

            GETN(1, GIFImageStart);
            break;
        }

        case GIFGlobalColormap: {
            m_globalColorMap.setDefined();
            GETN(1, GIFImageStart);
            break;
        }

        case GIFImageStart: {
            if (*currentComponent == '!') { // extension.
                GETN(2, GIFExtension);
                break;
            }

            if (*currentComponent == ',') { // image separator.
                GETN(9, GIFImageHeader);
                break;
            }

            GETN(0, GIFDone);
            break;
        }

        case GIFExtension: {
            size_t bytesInBlock = currentComponent[1];
            GIFState exceptionState = GIFSkipBlock;

            switch (*currentComponent) {
            case 0xf9:
                exceptionState = GIFControlExtension;
                bytesInBlock = std::max(bytesInBlock, static_cast<size_t>(4));
                break;

            case 0x01:
                break;

            case 0xff:
                exceptionState = GIFApplicationExtension;
                break;

            case 0xfe:
                exceptionState = GIFConsumeComment;
                break;
            }

            if (bytesInBlock)
                GETN(bytesInBlock, exceptionState);
            else
                GETN(1, GIFImageStart);
            break;
        }

        case GIFConsumeBlock: {
            if (!*currentComponent)
                GETN(1, GIFImageStart);
            else
                GETN(*currentComponent, GIFSkipBlock);
            break;
        }

        case GIFSkipBlock: {
            GETN(1, GIFConsumeBlock);
            break;
        }

        case GIFControlExtension: {
            addFrameIfNecessary();
            GIFFrameContext* currentFrame = m_frames.last().get();
            if (*currentComponent & 0x1)
                currentFrame->setTransparentPixel(currentComponent[3]);


            int disposalMethod = ((*currentComponent) >> 2) & 0x7;
            if (disposalMethod < 4) {
                currentFrame->setDisposalMethod(static_cast<blink::ImageFrame::DisposalMethod>(disposalMethod));
            } else if (disposalMethod == 4) {
                currentFrame->setDisposalMethod(blink::ImageFrame::DisposeOverwritePrevious);
            }
            currentFrame->setDelayTime(GETINT16(currentComponent + 1) * 10);
            GETN(1, GIFConsumeBlock);
            break;
        }

        case GIFCommentExtension: {
            if (*currentComponent)
                GETN(*currentComponent, GIFConsumeComment);
            else
                GETN(1, GIFImageStart);
            break;
        }

        case GIFConsumeComment: {
            GETN(1, GIFCommentExtension);
            break;
        }

        case GIFApplicationExtension: {
            if (m_bytesToConsume == 11
                && (!strncmp((char*)currentComponent, "NETSCAPE2.0", 11) || !strncmp((char*)currentComponent, "ANIMEXTS1.0", 11)))
                GETN(1, GIFNetscapeExtensionBlock);
            else
                GETN(1, GIFConsumeBlock);
            break;
        }

        case GIFNetscapeExtensionBlock: {
            if (*currentComponent)
                GETN(std::max(3, static_cast<int>(*currentComponent)), GIFConsumeNetscapeExtension);
            else
                GETN(1, GIFImageStart);
            break;
        }

        case GIFConsumeNetscapeExtension: {
            int netscapeExtension = currentComponent[0] & 7;

            if (netscapeExtension == 1) {
                m_loopCount = GETINT16(currentComponent + 1);

                if (!m_loopCount)
                    m_loopCount = blink::cAnimationLoopInfinite;

                GETN(1, GIFNetscapeExtensionBlock);
            } else if (netscapeExtension == 2) {

                GETN(1, GIFNetscapeExtensionBlock);
            } else {
                return false;
            }
            break;
        }

        case GIFImageHeader: {
            unsigned height, width, xOffset, yOffset;

            /* Get image offsets, with respect to the screen origin */
            xOffset = GETINT16(currentComponent);
            yOffset = GETINT16(currentComponent + 2);

            /* Get image width and height. */
             width  = GETINT16(currentComponent + 4);
             height = GETINT16(currentComponent + 6);
 
            /* Work around broken GIF files where the logical screen
             * size has weird width or height.  We assume that GIF87a
             * files don't contain animations.
             */
            if (currentFrameIsFirstFrame()
                && ((m_screenHeight < height) || (m_screenWidth < width) || (m_version == 87))) {
                m_screenHeight = height;
                m_screenWidth = width;
                xOffset = 0;
                yOffset = 0;
                if (m_client && !m_client->setSize(m_screenWidth, m_screenHeight))
                    return false;
             }
 
            if (!height || !width) {
                height = m_screenHeight;
                width = m_screenWidth;
                if (!height || !width)
                    return false;
            }
 
             if (query == GIFImageDecoder::GIFSizeQuery) {
                setRemainingBytes(len + 9);
                GETN(9, GIFImageHeader);
                return true;
            }

            addFrameIfNecessary();
             GIFFrameContext* currentFrame = m_frames.last().get();
 
             currentFrame->setHeaderDefined();
             currentFrame->setRect(xOffset, yOffset, width, height);
            m_screenWidth = std::max(m_screenWidth, width);
            m_screenHeight = std::max(m_screenHeight, height);
             currentFrame->setInterlaced(currentComponent[8] & 0x40);
 
            currentFrame->setProgressiveDisplay(currentFrameIsFirstFrame());

            const bool isLocalColormapDefined = currentComponent[8] & 0x80;
            if (isLocalColormapDefined) {
                const size_t numColors = 2 << (currentComponent[8] & 0x7);
                currentFrame->localColorMap().setTablePositionAndSize(dataPosition, numColors);
                GETN(BYTES_PER_COLORMAP_ENTRY * numColors, GIFImageColormap);
                break;
            }

            GETN(1, GIFLZWStart);
            break;
        }

        case GIFImageColormap: {
            ASSERT(!m_frames.isEmpty());
            m_frames.last()->localColorMap().setDefined();
            GETN(1, GIFLZWStart);
            break;
        }

        case GIFSubBlock: {
            const size_t bytesInBlock = *currentComponent;
            if (bytesInBlock)
                GETN(bytesInBlock, GIFLZW);
            else {
                ASSERT(!m_frames.isEmpty());
                m_frames.last()->setComplete();
                GETN(1, GIFImageStart);
            }
            break;
        }

        case GIFDone: {
            m_parseCompleted = true;
            return true;
        }

        default:
            return false;
            break;
        }
    }

    setRemainingBytes(len);
    return true;
}
