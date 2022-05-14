   std::unique_ptr<WebContents> CreateWebContents() {
     std::unique_ptr<WebContents> web_contents = CreateTestWebContents();
    ResourceCoordinatorTabHelper::CreateForWebContents(web_contents.get());
     content::WebContentsTester::For(web_contents.get())
         ->NavigateAndCommit(GURL("https://www.example.com"));

    base::RepeatingClosure run_loop_cb = base::BindRepeating(
       &base::TestMockTimeTaskRunner::RunUntilIdle, task_runner_);

    testing::WaitForLocalDBEntryToBeInitialized(web_contents.get(),
                                                run_loop_cb);
    testing::ExpireLocalDBObservationWindows(web_contents.get());
     return web_contents;
   }
