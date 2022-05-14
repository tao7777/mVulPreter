static void DecrementUntilZero(int* count) {
  {
    base::RunLoop run_loop;
    cloud_print_proxy->GetCloudPrintProxyInfo(
        base::BindOnce([](base::OnceClosure done, bool, const std::string&,
                          const std::string&) { std::move(done).Run(); },
                       run_loop.QuitClosure()));
    run_loop.Run();
  }
 }
