CreateDataReductionProxyChromeIOData(
    Profile* profile,
    const scoped_refptr<base::SingleThreadTaskRunner>& io_task_runner,
    const scoped_refptr<base::SingleThreadTaskRunner>& ui_task_runner) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
   DCHECK(profile);
   DCHECK(profile->GetPrefs());
 
  bool enabled = data_reduction_proxy::DataReductionProxySettings::
      IsDataSaverEnabledByUser(profile->GetPrefs());
 
   std::unique_ptr<data_reduction_proxy::DataReductionProxyIOData>
       data_reduction_proxy_io_data(
          new data_reduction_proxy::DataReductionProxyIOData(
              DataReductionProxyChromeSettings::GetClient(),
              profile->GetPrefs(), content::GetNetworkConnectionTracker(),
              io_task_runner, ui_task_runner, enabled, GetUserAgent(),
              version_info::GetChannelString(chrome::GetChannel())));

  data_reduction_proxy_io_data->set_lofi_decider(
      std::make_unique<data_reduction_proxy::ContentLoFiDecider>());
  data_reduction_proxy_io_data->set_resource_type_provider(
      std::make_unique<data_reduction_proxy::ContentResourceTypeProvider>());
  data_reduction_proxy_io_data->set_lofi_ui_service(
      std::make_unique<data_reduction_proxy::ContentLoFiUIService>(
          ui_task_runner, base::Bind(&OnLoFiResponseReceivedOnUI)));

  return data_reduction_proxy_io_data;
}
