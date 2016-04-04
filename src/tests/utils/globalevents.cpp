/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#define BOOST_TEST_MODULE globalevents

#include <boost/test/unit_test.hpp>

#include "../testingstuff/silent_logger.h"

struct TestInitializer {
	TestInitializer() {
		InitWxLogger();
	}
	~TestInitializer() {
	}
};

BOOST_GLOBAL_FIXTURE(TestInitializer);

#include "utils/globalevents.h"

class FakeEvtHandler: public wxEvtHandler {
public:
	FakeEvtHandler() :
			CallCount { 0 } {
	}

	virtual void QueueEvent(wxEvent *event) override
	{
		CallCount++;
	}

	void DummyTarget(wxCommandEvent&) {
		//DUMMY
	}

public:
	int CallCount;
};

BOOST_AUTO_TEST_CASE(TestIfCallsProperHandler) {
	GlobalEventManager* gem = GlobalEventManager::Instance();

	FakeEvtHandler DownloadStartedHandler;
	FakeEvtHandler DownloadInProgressHandler;
	FakeEvtHandler DownloadInProgressHandler2;
	FakeEvtHandler DownloadCompletedHandler;

	//Prepare
	gem->Subscribe(&DownloadStartedHandler, gem->OnDownloadStarted,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));
	gem->Subscribe(&DownloadInProgressHandler, gem->OnDownloadProgress,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));
	gem->Subscribe(&DownloadInProgressHandler2, gem->OnDownloadProgress,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));
	gem->Subscribe(&DownloadCompletedHandler, gem->OnDownloadComplete,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));

	//Make test
	gem->Send(gem->OnDownloadProgress);

	//Check results
	BOOST_CHECK_EQUAL(DownloadStartedHandler.CallCount, 0);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler.CallCount, 1);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler2.CallCount, 1);
	BOOST_CHECK_EQUAL(DownloadCompletedHandler.CallCount, 0);

	//Clean up
	gem->UnSubscribeAll(&DownloadStartedHandler);
	gem->UnSubscribeAll(&DownloadInProgressHandler);
	gem->UnSubscribeAll(&DownloadInProgressHandler2);
	gem->UnSubscribeAll(&DownloadCompletedHandler);

	gem->Release();
}

BOOST_AUTO_TEST_CASE(TestIfDisabledAfterOnQuitEvent) {
	GlobalEventManager* gem = GlobalEventManager::Instance();

	FakeEvtHandler DownloadStartedHandler;

	gem->Subscribe(&DownloadStartedHandler, gem->OnDownloadStarted,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));

	//Make first test
	gem->Send(gem->OnDownloadStarted);
	//Check
	BOOST_CHECK_EQUAL(DownloadStartedHandler.CallCount, 1);
	//Make second part of the test
	gem->Send(gem->OnQuit);
	gem->Send(gem->OnDownloadStarted);
	//Check
	BOOST_CHECK_EQUAL(DownloadStartedHandler.CallCount, 1); // <- no events processed after OnQuit event

	gem->UnSubscribeAll(&DownloadStartedHandler);

	gem->Release();
}

BOOST_AUTO_TEST_CASE(TestIfUnsubscribedProperly) {
	GlobalEventManager* gem = GlobalEventManager::Instance();

	FakeEvtHandler DownloadStartedHandler;
	FakeEvtHandler DownloadInProgressHandler;
	FakeEvtHandler DownloadInProgressHandler2;
	FakeEvtHandler DownloadCompletedHandler;

	//Prepare
	gem->Subscribe(&DownloadStartedHandler, gem->OnDownloadStarted,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));
	gem->Subscribe(&DownloadInProgressHandler, gem->OnDownloadProgress,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));
	gem->Subscribe(&DownloadInProgressHandler2, gem->OnDownloadProgress,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));
	gem->Subscribe(&DownloadCompletedHandler, gem->OnDownloadComplete,
			wxObjectEventFunction(&FakeEvtHandler::DummyTarget));

	//Make test
	gem->Send(gem->OnDownloadProgress);

	//Check results
	BOOST_CHECK_EQUAL(DownloadStartedHandler.CallCount, 0);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler.CallCount, 1);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler2.CallCount, 1);
	BOOST_CHECK_EQUAL(DownloadCompletedHandler.CallCount, 0);

	//Make second part of the test
	gem->UnSubscribe(&DownloadInProgressHandler, gem->OnDownloadProgress);
	gem->Send(gem->OnDownloadProgress);

	//Check results
	BOOST_CHECK_EQUAL(DownloadStartedHandler.CallCount, 0);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler.CallCount, 1);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler2.CallCount, 2);
	BOOST_CHECK_EQUAL(DownloadCompletedHandler.CallCount, 0);

	//Clean up
	gem->UnSubscribeAll(&DownloadStartedHandler);
	gem->UnSubscribeAll(&DownloadInProgressHandler2);
	gem->UnSubscribeAll(&DownloadCompletedHandler);

	//Final test
	gem->Send(gem->OnDownloadProgress);
	BOOST_CHECK_EQUAL(DownloadInProgressHandler2.CallCount, 2);

	gem->Release();
}
