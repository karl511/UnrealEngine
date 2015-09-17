// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UdpMessagingTestTypes.generated.h"


USTRUCT()
struct FUdpMockMessage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 Foo;

	FUdpMockMessage()
		: Foo(123)
	{ }
};


class FUdpMockMessageContext
	: public IMessageContext
{
public:

	FUdpMockMessageContext(FUdpMockMessage* InMessage)
		: Expiration(FDateTime::MaxValue())
		, Message(InMessage)
		, Scope(EMessageScope::Network)
		, SenderThread(ENamedThreads::AnyThread)
		, TimeSent(FDateTime(2015, 9, 17, 10, 59, 23, 666))
		, TypeInfo(FUdpMockMessage::StaticStruct())
	{
		FMessageAddress::Parse(TEXT("11111111-22222222-33333333-44444444"), Sender);
	}

public:

	// IMessageContext interface

	virtual const TMap<FName, FString>& GetAnnotations() const override { return Annotations; }
	virtual TSharedPtr<IMessageAttachment, ESPMode::ThreadSafe> GetAttachment() const override { return Attachment; }
	virtual const FDateTime& GetExpiration() const override { return Expiration; }
	virtual const void* GetMessage() const override { return Message; }
	virtual const TWeakObjectPtr<UScriptStruct>& GetMessageTypeInfo() const override { return TypeInfo; }
	virtual TSharedPtr<IMessageContext, ESPMode::ThreadSafe> GetOriginalContext() const override { return OriginalContext; }
	virtual const TArray<FMessageAddress>& GetRecipients() const override { return Recipients; }
	virtual EMessageScope GetScope() const override { return Scope; }
	virtual const FMessageAddress& GetSender() const override { return Sender; }
	virtual ENamedThreads::Type GetSenderThread() const override { return SenderThread; }
	virtual const FDateTime& GetTimeForwarded() const override { return TimeSent; }
	virtual const FDateTime& GetTimeSent() const override { return TimeSent; }

private:

	TMap<FName, FString> Annotations;
	IMessageAttachmentPtr Attachment;
	FDateTime Expiration;
	void* Message;
	IMessageContextPtr OriginalContext;
	TArray<FMessageAddress> Recipients;
	EMessageScope Scope;
	FMessageAddress Sender;
	ENamedThreads::Type SenderThread;
	FDateTime TimeSent;
	TWeakObjectPtr<UScriptStruct> TypeInfo;
};
