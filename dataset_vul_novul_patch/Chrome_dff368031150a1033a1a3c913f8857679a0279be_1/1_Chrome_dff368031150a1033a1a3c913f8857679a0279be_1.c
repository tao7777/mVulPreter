void Microtask::performCheckpoint()
 {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
     V8PerIsolateData* isolateData = V8PerIsolateData::from(isolate);
     ASSERT(isolateData);
     if (isolateData->recursionLevel() || isolateData->performingMicrotaskCheckpoint() || isolateData->destructionPending() || ScriptForbiddenScope::isScriptForbidden())
        return;
    isolateData->setPerformingMicrotaskCheckpoint(true);
    {
        V8RecursionScope recursionScope(isolate);
        isolate->RunMicrotasks();
    }
    isolateData->setPerformingMicrotaskCheckpoint(false);
}
