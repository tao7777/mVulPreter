  void ProcessBackingStore(HeapObjectHeader* header) {
     EXPECT_TRUE(header->IsValid());
     EXPECT_TRUE(header->IsMarked());
     header->Unmark();
    GCInfoTable::Get()
        .GCInfoFromIndex(header->GcInfoIndex())
        ->trace_(this, header->Payload());
   }
