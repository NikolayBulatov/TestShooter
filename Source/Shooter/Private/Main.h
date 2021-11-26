// Copyright 2021 Nikolay Bulatov. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
// #include "Debug.h"

#pragma warning(disable:4458)

//------------------------------------------------------------------------------
// Collision
//------------------------------------------------------------------------------


/** when you modify this, please note that this information can be saved with instances
* also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list **/

#define ECC_Projectile				ECC_GameTraceChannel1
#define Projectile_ProfileName		TEXT("Projectile")

