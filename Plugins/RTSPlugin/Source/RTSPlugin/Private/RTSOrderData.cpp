// Craig Duthie 2023

// This Include
#include "RTSOrderData.h"

// Local Includes
#include "RTSOrder.h"

FRTSOrderData::FRTSOrderData()
{
    OrderType = nullptr;
    bUseLocation = false;
    Location = FVector::ZeroVector;
    Target = nullptr;
    Index = -1;
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType)
    : OrderType(InOrderType)
    , bUseLocation(false)
    , Location(FVector::ZeroVector)
    , Target(nullptr)
    , Index(-1)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, AActor* InTarget)
    : OrderType(InOrderType)
    , bUseLocation(false)
    , Location(FVector::ZeroVector)
    , Target(InTarget)
    , Index(-1)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, FVector InLocation)
    : OrderType(InOrderType)
    , bUseLocation(true)
    , Location(InLocation)
    , Target(nullptr)
    , Index(-1)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, AActor* InTarget, FVector InLocation)
    : OrderType(InOrderType)
    , bUseLocation(true)
    , Location(InLocation)
    , Target(InTarget)
    , Index(-1)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex)
    : OrderType(InOrderType)
    , bUseLocation(false)
    , Location(FVector::ZeroVector)
    , Target(nullptr)
    , Index(InIndex)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, AActor* InTarget)
    : OrderType(InOrderType)
    , bUseLocation(false)
    , Location(FVector::ZeroVector)
    , Target(InTarget)
    , Index(InIndex)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, FVector InLocation)
    : OrderType(InOrderType)
    , bUseLocation(true)
    , Location(InLocation)
    , Target(nullptr)
    , Index(InIndex)
{
}

FRTSOrderData::FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, AActor* InTarget, FVector InLocation)
    : OrderType(InOrderType)
    , bUseLocation(true)
    , Location(InLocation)
    , Target(InTarget)
    , Index(InIndex)
{

}

FString FRTSOrderData::ToString() const
{
    FString s;

    if (OrderType != nullptr)
    {
        if (!OrderType.IsValid())
        {
            OrderType.LoadSynchronous();
        }

        s += OrderType->GetName();
    }
    else
    {
        s += TEXT("Unknown Order");
    }

    s += TEXT(" (");

    if (Index >= 0)
    {
        s += TEXT("Index: ");
        s += FString::FromInt(Index);
    }

    if (Target != nullptr)
    {
        if (Index >= 0)
        {
            s += TEXT(", Target: ");
        }

        else
        {
            s += TEXT("Target: ");
        }

        s += Target->GetName();
    }

    if (bUseLocation)
    {
        if (Target != nullptr || Index >= 0)
        {
            s += TEXT(", Location: ");
        }

        else
        {
            s += TEXT("Location: ");
        }

        s += Location.ToString();
    }

    s += TEXT(")");

    return s;
}

bool FRTSOrderData::operator==(const FRTSOrderData& Other) const
{
    bool bEqual =
        OrderType == Other.OrderType && bUseLocation == Other.bUseLocation && Target == Other.Target && Index == Other.Index;

    ERTSOrderTargetType TargetType = ERTSOrderTargetType::NONE;
    if (OrderType.IsValid())
    {
        TargetType = OrderType->GetDefaultObject<URTSOrder>()->GetTargetType(nullptr, Index);
    }

    if (TargetType == ERTSOrderTargetType::LOCATION || TargetType == ERTSOrderTargetType::DIRECTION)
    {
        bEqual = bEqual && Location.Equals(Other.Location);
    }

    return bEqual;
}

bool FRTSOrderData::operator!=(const FRTSOrderData& Other) const
{
    return !(*this == Other);
}
