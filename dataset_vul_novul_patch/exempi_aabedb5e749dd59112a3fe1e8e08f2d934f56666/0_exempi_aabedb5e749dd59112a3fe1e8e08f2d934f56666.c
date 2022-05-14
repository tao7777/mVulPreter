VP8XChunk::VP8XChunk(Container* parent)
  : Chunk(parent, kChunk_VP8X)
{
    this->needsRewrite = true;
    this->size = 10;
    this->data.resize(this->size);
     this->data.assign(this->size, 0);
     XMP_Uns8* bitstream =
         (XMP_Uns8*)parent->chunks[WEBP_CHUNK_IMAGE][0]->data.data();
    // See bug https://bugs.freedesktop.org/show_bug.cgi?id=105247
    // bitstream could be NULL.
    XMP_Uns32 width = bitstream ? ((bitstream[7] << 8) | bitstream[6]) & 0x3fff : 0;
    XMP_Uns32 height = bitstream ? ((bitstream[9] << 8) | bitstream[8]) & 0x3fff : 0;
     this->width(width);
     this->height(height);
     parent->vp8x = this;
VP8XChunk::VP8XChunk(Container* parent, WEBP_MetaHandler* handler)
  : Chunk(parent, handler)
{
    this->size = 10;
    this->needsRewrite = true;
    parent->vp8x = this;
}

XMP_Uns32 VP8XChunk::width()
{
    return GetLE24(&this->data[4]) + 1;
}
void VP8XChunk::width(XMP_Uns32 val)
{
    PutLE24(&this->data[4], val > 0 ? val - 1 : 0);
}
XMP_Uns32 VP8XChunk::height()
{
    return GetLE24(&this->data[7]) + 1;
}
void VP8XChunk::height(XMP_Uns32 val)
{
    PutLE24(&this->data[7], val > 0 ? val - 1 : 0);
}
bool VP8XChunk::xmp()
{
    XMP_Uns32 flags = GetLE32(&this->data[0]);
    return (bool)((flags >> XMP_FLAG_BIT) & 1);
}
void VP8XChunk::xmp(bool hasXMP)
{
    XMP_Uns32 flags = GetLE32(&this->data[0]);
    flags ^= (-hasXMP ^ flags) & (1 << XMP_FLAG_BIT);
    PutLE32(&this->data[0], flags);
}

Container::Container(WEBP_MetaHandler* handler) : Chunk(NULL, handler)
{
    this->needsRewrite = false;

    XMP_IO* file = handler->parent->ioRef;

    file->Seek(12, kXMP_SeekFromStart);

    XMP_Int64 size = handler->initialFileSize;

    XMP_Uns32 peek = 0;

    while (file->Offset() < size) {
        peek = XIO::PeekUns32_LE(file);
        switch (peek) {
        case kChunk_XMP_:
            this->addChunk(new XMPChunk(this, handler));
            break;
        case kChunk_VP8X:
            this->addChunk(new VP8XChunk(this, handler));
            break;
        default:
            this->addChunk(new Chunk(this, handler));
            break;
        }
    }

    if (this->chunks[WEBP_CHUNK_IMAGE].size() == 0) {
        XMP_Throw("File has no image bitstream", kXMPErr_BadFileFormat);
    }

    if (this->chunks[WEBP_CHUNK_VP8X].size() == 0) {
        this->needsRewrite = true;
        this->addChunk(new VP8XChunk(this));
    }

    if (this->chunks[WEBP_CHUNK_XMP].size() == 0) {
        XMPChunk* xmpChunk = new XMPChunk(this);
        this->addChunk(xmpChunk);
        handler->xmpChunk = xmpChunk;
        this->vp8x->xmp(true);
    }
}

Chunk* Container::getExifChunk()
{
    if (this->chunks[WEBP::WEBP_CHUNK_EXIF].size() == 0) {
        return NULL;
    }
    return this->chunks[WEBP::WEBP_CHUNK_EXIF][0];
}

void Container::addChunk(Chunk* chunk)
{
    ChunkId idx;

    try {
        idx = chunkMap.at(chunk->tag);
    }
    catch (const std::out_of_range& e) {
        idx = WEBP_CHUNK_UNKNOWN;
    }
    this->chunks[idx].push_back(chunk);
}

void Container::write(WEBP_MetaHandler* handler)
{
    XMP_IO* file = handler->parent->ioRef;
    file->Rewind();
    XIO::WriteUns32_LE(file, this->tag);
    XIO::WriteUns32_LE(file, (XMP_Uns32) this->size);
    XIO::WriteUns32_LE(file, kChunk_WEBP);

    size_t i, j;
    std::vector<Chunk*> chunkVect;
    for (i = 0; i < WEBP_CHUNK_NIL; i++) {
        chunkVect = this->chunks[i];
        for (j = 0; j < chunkVect.size(); j++) {
            chunkVect.at(j)->write(handler);
        }
    }
    XMP_Int64 lastOffset = file->Offset();
    this->size = lastOffset - 8;
    file->Seek(this->pos + 4, kXMP_SeekFromStart);
    XIO::WriteUns32_LE(file, (XMP_Uns32) this->size);
    file->Seek(lastOffset, kXMP_SeekFromStart);
    if (lastOffset < handler->initialFileSize) {
        file->Truncate(lastOffset);
    }
}

Container::~Container()
{
    Chunk* chunk;
    size_t i;
    std::vector<Chunk*> chunkVect;
    for (i = 0; i < WEBP_CHUNK_NIL; i++) {
        chunkVect = this->chunks[i];
        while (!chunkVect.empty()) {
            chunk = chunkVect.back();
            delete chunk;
            chunkVect.pop_back();
        }
    }
}
}
