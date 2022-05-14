FileReaderLoader::FileReaderLoader(ReadType read_type,
                                   FileReaderLoaderClient* client)
    : read_type_(read_type),
      client_(client),
       handle_watcher_(FROM_HERE, mojo::SimpleWatcher::ArmingPolicy::AUTOMATIC),
