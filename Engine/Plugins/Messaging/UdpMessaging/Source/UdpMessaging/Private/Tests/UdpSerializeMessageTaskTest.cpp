// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UdpMessagingPrivatePCH.h"
#include "AutomationTest.h"
#include "UdpMessagingTestTypes.h"



IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUdpSerializeMessageTaskTest, "System.Core.Messaging.Transports.Udp.UdpSerializedMessage", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)


namespace UdpSerializeMessageTaskTest
{
	const int64 NumMessages = 100000;
	const FTimespan MaxWaitTime(0, 0, 5);

	int64 CompletedMessages = 0;
	int64 FailedMessages = 0;
	FUdpSerializedMessagePtr ReferenceMessage;
}


void HandleSerializedMessageStateChanged(FUdpSerializedMessageRef SerializedMessage)
{
	using namespace UdpSerializeMessageTaskTest;

	FPlatformAtomics::InterlockedIncrement(&CompletedMessages);

	const TArray<uint8>& ReferenceDataArray = ReferenceMessage->GetDataArray();
	const TArray<uint8>& SerializedDataArray = SerializedMessage->GetDataArray();

	if (ReferenceDataArray.Num() != SerializedDataArray.Num())
	{
		FPlatformAtomics::InterlockedIncrement(&FailedMessages);
	}
	else
	{
		const void* ReferenceData = ReferenceDataArray.GetData();
		const void* SerializedData = SerializedDataArray.GetData();

		if (FMemory::Memcmp(ReferenceData, SerializedData, ReferenceDataArray.Num()) != 0)
		{
			FPlatformAtomics::InterlockedIncrement(&FailedMessages);
		}
	}
}


bool FUdpSerializeMessageTaskTest::RunTest(const FString& Parameters)
{
	using namespace UdpSerializeMessageTaskTest;

	CompletedMessages = 0;
	FailedMessages = 0;
	ReferenceMessage = MakeShareable(new FUdpSerializedMessage());

	FUdpMockMessage* Message = new FUdpMockMessage();
	IMessageContextRef Context = MakeShareable(new FUdpMockMessageContext(Message));

	// synchronous reference serialization
	FUdpSerializeMessageTask ReferenceTask(Context, ReferenceMessage.ToSharedRef());
	{
		ReferenceTask.DoTask(FTaskGraphInterface::Get().GetCurrentThreadIfKnown(), FGraphEventRef());
	}

	// stress test
	for (int32 TestIndex = 0; TestIndex < NumMessages; ++TestIndex)
	{
		FUdpSerializedMessageRef SerializedMessage = MakeShareable(new FUdpSerializedMessage());
		{
			SerializedMessage->OnStateChanged().BindStatic(&HandleSerializedMessageStateChanged, SerializedMessage);
		}

		TGraphTask<FUdpSerializeMessageTask>::CreateTask().ConstructAndDispatchWhenReady(Context, SerializedMessage);
	}

	FDateTime StartTime = FDateTime::UtcNow();

	while ((CompletedMessages < NumMessages) && ((FDateTime::UtcNow() - StartTime) < MaxWaitTime))
	{
		FPlatformProcess::Sleep(0.0f);
	}

	TestEqual(TEXT("The number of completed messages must equal the total number of messages"), CompletedMessages, NumMessages);
	TestEqual(TEXT("There must be no failed messages"), FailedMessages, (int64)0);

	// clean up
	FMemory::Free(Message);
	ReferenceMessage.Reset();

	return ((CompletedMessages == NumMessages) && (FailedMessages == 0));
}
