#pragma once

#include "CoreMinimal.h"
#include "AIGraphTypes.h"

#include "Modules/ModuleManager.h"

#include "IKatsuraQuestTreeEditor.h"

class FKatsuraQuestEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Creates an instance of Quest Tree editor.  Only virtual so that it can be called across the DLL boundary. */
    virtual TSharedRef<IKatsuraQuestTreeEditor> CreateQuestTreeEditor(const EToolkitMode::Type Mode,
        const TSharedPtr<IToolkitHost>& InitToolkitHost, UObject* Object);


	TSharedPtr<FGraphNodeClassHelper> GetClassCache() const { return ClassCache; }

	/** Quest Tree app identifier string */
	static const FName QuestTreeEditorAppIdentifier;

private:
	TSharedPtr<FGraphNodeClassHelper> ClassCache;
};
