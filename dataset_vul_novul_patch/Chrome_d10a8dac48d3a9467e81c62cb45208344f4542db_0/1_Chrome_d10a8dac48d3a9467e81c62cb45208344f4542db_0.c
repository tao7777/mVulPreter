    StateBase* writeFile(v8::Handle<v8::Value> value, StateBase* next)
    {
        File* file = V8File::toNative(value.As<v8::Object>());
        if (!file)
            return 0;
         if (file->hasBeenClosed())
             return handleError(DataCloneError, "A File object has been closed, and could therefore not be cloned.", next);
         int blobIndex = -1;
        m_blobDataHandles.add(file->uuid(), file->blobDataHandle());
         if (appendFileInfo(file, &blobIndex)) {
             ASSERT(blobIndex >= 0);
             m_writer.writeFileIndex(blobIndex);
        } else {
            m_writer.writeFile(*file);
        }
        return 0;
    }
