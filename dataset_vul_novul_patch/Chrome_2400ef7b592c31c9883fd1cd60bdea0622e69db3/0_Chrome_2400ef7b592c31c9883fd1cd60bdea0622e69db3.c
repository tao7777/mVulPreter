void SerializedScriptValue::transferArrayBuffers(v8::Isolate* isolate, const ArrayBufferArray& arrayBuffers, ExceptionState& exceptionState)
{
    if (!arrayBuffers.size())
        return;

    for (size_t i = 0; i < arrayBuffers.size(); ++i) {
        if (arrayBuffers[i]->isNeutered()) {
            exceptionState.throwDOMException(DataCloneError, "ArrayBuffer at index " + String::number(i) + " is already neutered.");
            return;
        }
    }

    OwnPtr<ArrayBufferContentsArray> contents = adoptPtr(new ArrayBufferContentsArray(arrayBuffers.size()));

    HeapHashSet<Member<DOMArrayBufferBase>> visited;
    for (size_t i = 0; i < arrayBuffers.size(); ++i) {
        if (visited.contains(arrayBuffers[i]))
            continue;
        visited.add(arrayBuffers[i]);

        if (arrayBuffers[i]->isShared()) {
            bool result = arrayBuffers[i]->shareContentsWith(contents->at(i));
            if (!result) {
                exceptionState.throwDOMException(DataCloneError, "SharedArrayBuffer at index " + String::number(i) + " could not be transferred.");
                return;
            }
        } else {
            Vector<v8::Local<v8::ArrayBuffer>, 4> bufferHandles;
            v8::HandleScope handleScope(isolate);
            acculumateArrayBuffersForAllWorlds(isolate, static_cast<DOMArrayBuffer*>(arrayBuffers[i].get()), bufferHandles);
            bool isNeuterable = true;
            for (size_t j = 0; j < bufferHandles.size(); ++j)
                isNeuterable &= bufferHandles[j]->IsNeuterable();
 
             DOMArrayBufferBase* toTransfer = arrayBuffers[i];
             if (!isNeuterable)
                toTransfer = DOMArrayBuffer::create(arrayBuffers[i]->buffer()->data(), arrayBuffers[i]->buffer()->byteLength());
             bool result = toTransfer->transfer(contents->at(i));
             if (!result) {
                 exceptionState.throwDOMException(DataCloneError, "ArrayBuffer at index " + String::number(i) + " could not be transferred.");
                return;
            }

            if (isNeuterable)
                for (size_t j = 0; j < bufferHandles.size(); ++j)
                    bufferHandles[j]->Neuter();
        }

    }
    m_arrayBufferContentsArray = contents.release();
}
