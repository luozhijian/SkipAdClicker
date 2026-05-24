#include "RecentActivityService.hpp"

#include <stdexcept>

namespace automationtest::playtestbook::services {

namespace {

std::string AnyToString(const std::any& value)
{
    if (!value.has_value()) {
        return {};
    }
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    }
    if (value.type() == typeid(const char*)) {
        return std::any_cast<const char*>(value);
    }
    throw std::runtime_error("Expected string parameter");
}

} // namespace

RecentActivityService& RecentActivityService::Instance()
{
    static RecentActivityService instance;
    return instance;
}

const utilities::Bitmap* RecentActivityService::RecentMost() const noexcept
{
    return bitmaps_.empty() ? nullptr : &bitmaps_.back();
}

const utilities::Bitmap* RecentActivityService::RecentMost1() const noexcept
{
    return bitmaps_.size() < 2 ? nullptr : &bitmaps_[bitmaps_.size() - 2];
}

const utilities::Bitmap* RecentActivityService::RecentMost2() const noexcept
{
    return bitmaps_.size() < 3 ? nullptr : &bitmaps_[bitmaps_.size() - 3];
}

utilities::Point RecentActivityService::LatestClick() const noexcept
{
    return latest_click_;
}

void RecentActivityService::Clear()
{
    bitmaps_.clear();
}

void RecentActivityService::PushBitmap(const utilities::Bitmap& bitmap)
{
    bitmaps_.push_back(bitmap);
    if (bitmaps_.size() > 3) {
        bitmaps_.erase(bitmaps_.begin(), bitmaps_.end() - 3);
    }
}

void RecentActivityService::PushLocation(utilities::Point point)
{
    latest_click_ = point;
}

void RecentActivityService::SaveTransaction(const std::string& transaction_name)
{
    const auto* recent = RecentMost();
    saved_transactions_[transaction_name] = SavedTransaction {recent ? *recent : utilities::Bitmap {}, latest_click_};
}

std::any RecentActivityService::GetSavedTransactionValue(const std::string& transaction_name, const std::string& value_type) const
{
    const auto it = saved_transactions_.find(transaction_name);
    if (it == saved_transactions_.end()) {
        throw std::runtime_error("Cannot find saved transaction " + transaction_name);
    }
    if (utilities::status::CaseInsensitiveEqual {}(value_type, "point")) {
        return it->second.point;
    }
    return it->second.bitmap;
}

void RecentActivityService::RegisterBindings(utilities::status::LoadFunctions& load_functions)
{
    using RegisteredMethod = utilities::status::LoadFunctions::RegisteredMethod;
    using RegisteredParameter = utilities::status::LoadFunctions::RegisteredParameter;
    using RegisteredProperty = utilities::status::LoadFunctions::RegisteredProperty;

    load_functions.RegisterMethod("RecentActivitySaveTransaction", RegisteredMethod {
        .declaring_type = "RecentActivityService",
        .invoke = [](const std::vector<std::any>& arguments) -> std::any {
            RecentActivityService::Instance().SaveTransaction(AnyToString(arguments.at(0)));
            return {};
        },
        .parameters = {
            RegisteredParameter {.name = "transactionPointName", .converter = [](const std::string& value) -> std::any {
                return AnyToString(value);
            }},
        },
    });

    load_functions.RegisterMethod("RecentActivitySaveTransaction", RegisteredMethod {
        .declaring_type = "RecentActivityService",
        .invoke = [](const std::vector<std::any>& arguments) -> std::any {
            return RecentActivityService::Instance().GetSavedTransactionValue(
                AnyToString(arguments.at(0)),
                AnyToString(arguments.at(1)));
        },
        .parameters = {
            RegisteredParameter {.name = "transactionName", .converter = [](const std::string& value) -> std::any {
                return AnyToString(value);
            }},
            RegisteredParameter {.name = "valueType", .converter = [](const std::string& value) -> std::any {
                return AnyToString(value);
            }},
        },
    });

    load_functions.RegisterMethod("GetLatestClick", RegisteredMethod {
        .declaring_type = "RecentActivityService",
        .invoke = [](const std::vector<std::any>&) -> std::any {
            return RecentActivityService::Instance().LatestClick();
        },
    });

    load_functions.RegisterProperty("RecentClickLocation", RegisteredProperty {
        .declaring_type = "RecentActivityService",
        .getter = []() -> std::any {
            return RecentActivityService::Instance().LatestClick();
        },
    });
    load_functions.RegisterProperty("BitmapRecentMost", RegisteredProperty {
        .declaring_type = "RecentActivityService",
        .getter = []() -> std::any {
            const auto* bitmap = RecentActivityService::Instance().RecentMost();
            return bitmap ? std::any(*bitmap) : std::any {};
        },
    });
    load_functions.RegisterProperty("BitmapRecentMost_1", RegisteredProperty {
        .declaring_type = "RecentActivityService",
        .getter = []() -> std::any {
            const auto* bitmap = RecentActivityService::Instance().RecentMost1();
            return bitmap ? std::any(*bitmap) : std::any {};
        },
    });
    load_functions.RegisterProperty("BitmapRecentMost_2", RegisteredProperty {
        .declaring_type = "RecentActivityService",
        .getter = []() -> std::any {
            const auto* bitmap = RecentActivityService::Instance().RecentMost2();
            return bitmap ? std::any(*bitmap) : std::any {};
        },
    });
}

} // namespace automationtest::playtestbook::services
