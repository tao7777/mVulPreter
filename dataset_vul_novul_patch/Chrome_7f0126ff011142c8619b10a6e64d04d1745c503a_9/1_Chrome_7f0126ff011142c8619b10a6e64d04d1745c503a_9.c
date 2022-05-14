    void printResources()
    {
        printf("Printing resources (%zu resources in total)\n", m_resources.size());
        for (size_t i = 0; i < m_resources.size(); ++i) {
            printf("%zu. '%s', '%s'\n", i, m_resources[i].url.string().utf8().data(),
                m_resources[i].mimeType.utf8().data());
        }
    }
