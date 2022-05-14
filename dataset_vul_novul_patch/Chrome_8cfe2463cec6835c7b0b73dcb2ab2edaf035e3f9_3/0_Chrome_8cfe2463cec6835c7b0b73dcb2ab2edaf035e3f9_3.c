void AppControllerImpl::GetArcAndroidId(
void AppControllerService::GetArcAndroidId(
     mojom::AppController::GetArcAndroidIdCallback callback) {
   arc::GetAndroidId(base::BindOnce(
       [](mojom::AppController::GetArcAndroidIdCallback callback, bool success,
         int64_t android_id) {
        std::move(callback).Run(success, base::NumberToString(android_id));
      },
       std::move(callback)));
 }
