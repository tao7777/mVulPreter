     virtual void commitCompleteOnCCThread(CCLayerTreeHostImpl*)
     {
        EXPECT_EQ(0, m_numDraws);
         m_numCommits++;
     }
