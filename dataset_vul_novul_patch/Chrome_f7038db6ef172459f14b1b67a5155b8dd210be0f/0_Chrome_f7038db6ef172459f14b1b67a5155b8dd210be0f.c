    bool decode(const SharedBuffer& data, bool onlySize)
    {
        m_decodingSizeOnly = onlySize;

        unsigned newByteCount = data.size() - m_bufferLength;
        unsigned readOffset = m_bufferLength - m_info.src->bytes_in_buffer;

        m_info.src->bytes_in_buffer += newByteCount;
        m_info.src->next_input_byte = (JOCTET*)(data.data()) + readOffset;

        if (m_bytesToSkip)
            skipBytes(m_bytesToSkip);

        m_bufferLength = data.size();

        if (setjmp(m_err.setjmp_buffer))
            return m_decoder->setFailed();

        switch (m_state) {
        case JPEG_HEADER:
            if (jpeg_read_header(&m_info, true) == JPEG_SUSPENDED)
                return false; // I/O suspension.

            switch (m_info.jpeg_color_space) {
            case JCS_GRAYSCALE:
            case JCS_RGB:
            case JCS_YCbCr:
                m_info.out_color_space = rgbOutputColorSpace();
#if defined(TURBO_JPEG_RGB_SWIZZLE)
                if (m_info.saw_JFIF_marker)
                    break;
                if (m_info.saw_Adobe_marker && !m_info.Adobe_transform)
                    m_info.out_color_space = JCS_RGB;
#endif
                break;
            case JCS_CMYK:
            case JCS_YCCK:
                m_info.out_color_space = JCS_CMYK;
                break;
            default:
                return m_decoder->setFailed();
            }

            m_state = JPEG_START_DECOMPRESS;

            if (!m_decoder->setSize(m_info.image_width, m_info.image_height))
                return false;

            m_decoder->setOrientation(readImageOrientation(info()));

#if ENABLE(IMAGE_DECODER_DOWN_SAMPLING) && defined(TURBO_JPEG_RGB_SWIZZLE)
            if (m_decoder->willDownSample() && turboSwizzled(m_info.out_color_space))
                m_info.out_color_space = JCS_RGB;
#endif

#if USE(QCMSLIB)
            if (!m_decoder->ignoresGammaAndColorProfile()) {
                ColorProfile colorProfile = readColorProfile(info());
                createColorTransform(colorProfile, colorSpaceHasAlpha(m_info.out_color_space));
#if defined(TURBO_JPEG_RGB_SWIZZLE)
                if (m_transform && m_info.out_color_space == JCS_EXT_BGRA)
                    m_info.out_color_space = JCS_EXT_RGBA;
#endif
            }
#endif
            m_info.buffered_image = jpeg_has_multiple_scans(&m_info);

            jpeg_calc_output_dimensions(&m_info);

            m_samples = (*m_info.mem->alloc_sarray)((j_common_ptr) &m_info, JPOOL_IMAGE, m_info.output_width * 4, 1);

            if (m_decodingSizeOnly) {
                m_bufferLength -= m_info.src->bytes_in_buffer;
                m_info.src->bytes_in_buffer = 0;
                return true;
            }

        case JPEG_START_DECOMPRESS:
            m_info.dct_method = dctMethod();
            m_info.dither_mode = ditherMode();
            m_info.do_fancy_upsampling = doFancyUpsampling();
            m_info.enable_2pass_quant = false;
            m_info.do_block_smoothing = true;

            if (!jpeg_start_decompress(&m_info))
                return false; // I/O suspension.

            m_state = (m_info.buffered_image) ? JPEG_DECOMPRESS_PROGRESSIVE : JPEG_DECOMPRESS_SEQUENTIAL;

        case JPEG_DECOMPRESS_SEQUENTIAL:
            if (m_state == JPEG_DECOMPRESS_SEQUENTIAL) {

                if (!m_decoder->outputScanlines())
                    return false; // I/O suspension.

                ASSERT(m_info.output_scanline == m_info.output_height);
                m_state = JPEG_DONE;
            }

        case JPEG_DECOMPRESS_PROGRESSIVE:
            if (m_state == JPEG_DECOMPRESS_PROGRESSIVE) {
                int status;
                do {
                    status = jpeg_consume_input(&m_info);
                } while ((status != JPEG_SUSPENDED) && (status != JPEG_REACHED_EOI));

                for (;;) {
                    if (!m_info.output_scanline) {
                        int scan = m_info.input_scan_number;

                        if (!m_info.output_scan_number && (scan > 1) && (status != JPEG_REACHED_EOI))
                            --scan;

                        if (!jpeg_start_output(&m_info, scan))
                            return false; // I/O suspension.
                    }

                     if (m_info.output_scanline == 0xffffff)
                         m_info.output_scanline = 0;
 
                    // If outputScanlines() fails, it deletes |this|. Therefore,
                    // copy the decoder pointer and use it to check for failure
                    // to avoid member access in the failure case.
                    JPEGImageDecoder* decoder = m_decoder;
                    if (!decoder->outputScanlines()) {
                        if (decoder->failed()) // Careful; |this| is deleted.
                            return false;
                         if (!m_info.output_scanline)
                            m_info.output_scanline = 0xffffff;
                        return false; // I/O suspension.
                    }

                    if (m_info.output_scanline == m_info.output_height) {
                        if (!jpeg_finish_output(&m_info))
                            return false; // I/O suspension.

                        if (jpeg_input_complete(&m_info) && (m_info.input_scan_number == m_info.output_scan_number))
                            break;

                        m_info.output_scanline = 0;
                    }
                }

                m_state = JPEG_DONE;
            }

        case JPEG_DONE:
            return jpeg_finish_decompress(&m_info);

        case JPEG_ERROR:
            return m_decoder->setFailed();
        }

        return true;
    }
