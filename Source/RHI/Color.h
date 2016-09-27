// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "unreal_defination.h"

/**
 * A linear, 32-bit/component floating point RGBA color.
 */
struct FLinearColor
{
	float	R,
		G,
		B,
		A;
	FLinearColor()
		: R(0), G(0), B(0), A(0)
	{}
	FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}

	// Common colors.	
	static  const FLinearColor White;
	static  const FLinearColor Gray;
	static  const FLinearColor Black;
	static  const FLinearColor Transparent;
	static  const FLinearColor Red;
	static  const FLinearColor Green;
	static  const FLinearColor Blue;
	static  const FLinearColor Yellow;
};

struct FColor
{
public:

	union { struct { uint8 B, G, R, A; }; uint32 AlignmentDummy; };

	FColor()
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = G = B = A = 0;
	}
	FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
	{
		// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
		R = InR;
		G = InG;
		B = InB;
		A = InA;

	}
	static  const FColor White;
	static  const FColor Black;
	static  const FColor Transparent;
	static  const FColor Red;
	static  const FColor Green;
	static  const FColor Blue;
	static  const FColor Yellow;
	static  const FColor Cyan;
	static  const FColor Magenta;

};
