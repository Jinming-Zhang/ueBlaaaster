// Fill out your copyright notice in the Description page of Project Settings.


#include "LogUtils.h"

LogUtils::LogUtils()
{
}

LogUtils::~LogUtils()
{
}

void LogUtils::Log(int32 key, float time, FColor clr, const FString& msg)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			key,
			time,
			clr,
			msg
		);
	}
}
