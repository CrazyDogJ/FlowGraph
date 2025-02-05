#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLOWEXTRA_API FFlowExtraModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
