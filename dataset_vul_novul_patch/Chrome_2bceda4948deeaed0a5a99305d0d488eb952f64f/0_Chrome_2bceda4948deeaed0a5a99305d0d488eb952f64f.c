 ScriptPromise BluetoothRemoteGATTCharacteristic::getDescriptorsImpl(
     ScriptState* scriptState,
     mojom::blink::WebBluetoothGATTQueryQuantity quantity,
    const String& descriptorsUUID) {
   if (!getGatt()->connected()) {
     return ScriptPromise::rejectWithDOMException(
         scriptState,
        BluetoothRemoteGATTUtils::CreateDOMException(
            BluetoothRemoteGATTUtils::ExceptionType::kGATTServerNotConnected));
  }

  if (!getGatt()->device()->isValidCharacteristic(
          m_characteristic->instance_id)) {
    return ScriptPromise::rejectWithDOMException(
        scriptState,
        BluetoothRemoteGATTUtils::CreateDOMException(
            BluetoothRemoteGATTUtils::ExceptionType::kInvalidCharacteristic));
  }

  ScriptPromiseResolver* resolver = ScriptPromiseResolver::create(scriptState);
  ScriptPromise promise = resolver->promise();
   getGatt()->AddToActiveAlgorithms(resolver);
 
   mojom::blink::WebBluetoothService* service = m_device->bluetooth()->service();
   service->RemoteCharacteristicGetDescriptors(
      m_characteristic->instance_id, quantity, descriptorsUUID,
       convertToBaseCallback(
           WTF::bind(&BluetoothRemoteGATTCharacteristic::GetDescriptorsCallback,
                     wrapPersistent(this), m_characteristic->instance_id,
                    quantity, wrapPersistent(resolver))));

  return promise;
}
