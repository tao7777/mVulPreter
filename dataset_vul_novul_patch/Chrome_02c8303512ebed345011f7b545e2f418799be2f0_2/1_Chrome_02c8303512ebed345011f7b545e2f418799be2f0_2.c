 PassRefPtrWillBeRawPtr<File> DOMFileSystemSync::createFile(const FileEntrySync* fileEntry, ExceptionState& exceptionState)
 {
     KURL fileSystemURL = createFileSystemURL(fileEntry);
    RefPtrWillBeRawPtr<CreateFileHelper::CreateFileResult> result(CreateFileHelper::CreateFileResult::create());
     fileSystem()->createSnapshotFileAndReadMetadata(fileSystemURL, CreateFileHelper::create(result, fileEntry->name(), fileSystemURL, type()));
     if (result->m_failed) {
         exceptionState.throwDOMException(result->m_code, "Could not create '" + fileEntry->name() + "'.");
        return nullptr;
    }
    return result->m_file.get();
}
