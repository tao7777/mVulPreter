  std::unique_ptr<SiteCharacteristicsDataReader> GetReaderForOrigin(
      Profile* profile,
      const url::Origin& origin) {
    SiteCharacteristicsDataStore* data_store =
        LocalSiteCharacteristicsDataStoreFactory::GetForProfile(profile);
    EXPECT_TRUE(data_store);
     std::unique_ptr<SiteCharacteristicsDataReader> reader =
         data_store->GetReaderForOrigin(origin);
 
    internal::LocalSiteCharacteristicsDataImpl* impl =
         static_cast<LocalSiteCharacteristicsDataReader*>(reader.get())
            ->impl_for_testing()
            .get();
     while (!impl->site_characteristics_for_testing().IsInitialized())
       base::RunLoop().RunUntilIdle();
     return reader;
  }
