    StateBase* writeFileList(v8::Handle<v8::Value> value, StateBase* next)
    {
        FileList* fileList = V8FileList::toNative(value.As<v8::Object>());
        if (!fileList)
            return 0;
        unsigned length = fileList->length();
        Vector<int> blobIndices;
        for (unsigned i = 0; i < length; ++i) {
            int blobIndex = -1;
             const File* file = fileList->item(i);
             if (file->hasBeenClosed())
                 return handleError(DataCloneError, "A File object has been closed, and could therefore not be cloned.", next);
            m_blobDataHandles.set(file->uuid(), file->blobDataHandle());
             if (appendFileInfo(file, &blobIndex)) {
                 ASSERT(!i || blobIndex > 0);
                 ASSERT(blobIndex >= 0);
                blobIndices.append(blobIndex);
            }
        }
        if (!blobIndices.isEmpty())
            m_writer.writeFileListIndex(blobIndices);
        else
            m_writer.writeFileList(*fileList);
        return 0;
    }
