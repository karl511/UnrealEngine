// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

/**
 * Interface for providing different Transports for the Network File System ( client )
 */

#pragma once 
#include "NetworkFilePrivatePCH.h"

class ITransport 
{

public:
	ITransport (){};
	virtual ~ITransport(){};

	virtual bool Initialize(const TCHAR*) = 0; 
	virtual bool SendPayloadAndReceiveResponse(TArray<uint8>& In, TArray<uint8>& Out) = 0; 

};