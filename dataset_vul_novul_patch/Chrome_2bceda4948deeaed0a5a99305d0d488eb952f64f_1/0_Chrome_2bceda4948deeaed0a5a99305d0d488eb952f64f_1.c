ScriptPromise BluetoothRemoteGATTService::getCharacteristicsImpl(
    ScriptState* scriptState,
    mojom::blink::WebBluetoothGATTQueryQuantity quantity,
    const String& characteristicsUUID) {
  if (!device()->gatt()->connected()) {
    return ScriptPromise::rejectWithDOMException(
        scriptState,
        DOMException::create(NetworkError, kGATTServerNotConnected));
  }

  if (!device()->isValidService(m_service->instance_id)) {
    return ScriptPromise::rejectWithDOMException(
        scriptState, DOMException::create(InvalidStateError, kInvalidService));
  }

  ScriptPromiseResolver* resolver = ScriptPromiseResolver::create(scriptState);
  ScriptPromise promise = resolver->promise();
   device()->gatt()->AddToActiveAlgorithms(resolver);
 
   mojom::blink::WebBluetoothService* service = m_device->bluetooth()->service();
   service->RemoteServiceGetCharacteristics(
      m_service->instance_id, quantity, characteristicsUUID,
       convertToBaseCallback(
           WTF::bind(&BluetoothRemoteGATTService::GetCharacteristicsCallback,
                     wrapPersistent(this), m_service->instance_id, quantity,
                    wrapPersistent(resolver))));

  return promise;
}
