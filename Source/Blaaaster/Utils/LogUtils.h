// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class LogUtils
{
public:
	LogUtils();
	~LogUtils();
	static void Log(int32 key, float time, FColor clr, const FString& msg);
};
