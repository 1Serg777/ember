#include "Event/EventRegistry.h"

namespace ember {

	void EventCallbackStorageBase::ClearAll() {
		ClearEventCallbacks();
		ClearEventCallbackData();
	}

	void EventRegistry::Update() {
		NotifyAllCallbacks();
		TidyCallbackData();
	}
	void EventRegistry::NotifyAllCallbacks() {
		for (auto& storage : eventCallbackStorage) {
			if (storage)
				storage->NotifyAll();
		}
	}
	void EventRegistry::TidyCallbackData() {
		for (auto& storage : eventCallbackStorage) {
			if (storage)
				storage->ClearEventCallbackData();
		}
	}
}