#ifndef AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_RECENTACTIVITYSERVICE_HPP
#define AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_RECENTACTIVITYSERVICE_HPP

#include "../../Utilities/CommonTypes.hpp"
#include "../../Utilities/Interface/IRecentActionService.hpp"
#include "../../Utilities/Status/LoadFunctions.hpp"

#include <optional>
#include <unordered_map>

namespace automationtest::playtestbook::services {

class RecentActivityService : public utilities::interface::IRecentActionService {
public:
    static RecentActivityService& Instance();

    [[nodiscard]] const utilities::Bitmap* RecentMost() const noexcept override;
    [[nodiscard]] const utilities::Bitmap* RecentMost1() const noexcept override;
    [[nodiscard]] const utilities::Bitmap* RecentMost2() const noexcept override;
    [[nodiscard]] utilities::Point LatestClick() const noexcept override;

    void Clear() override;
    void PushBitmap(const utilities::Bitmap& bitmap) override;
    void PushLocation(utilities::Point point) override;

    void SaveTransaction(const std::string& transaction_name);
    [[nodiscard]] std::any GetSavedTransactionValue(const std::string& transaction_name, const std::string& value_type) const;
    void RegisterBindings(utilities::status::LoadFunctions& load_functions);

private:
    RecentActivityService() = default;

    struct SavedTransaction {
        utilities::Bitmap bitmap {};
        utilities::Point point {};
    };

    std::vector<utilities::Bitmap> bitmaps_ {};
    utilities::Point latest_click_ {};
    std::unordered_map<std::string, SavedTransaction, utilities::status::CaseInsensitiveHash, utilities::status::CaseInsensitiveEqual> saved_transactions_ {};
};

} // namespace automationtest::playtestbook::services

#endif // AUTOMATIOTEST_PLAYTESTBOOK_SERVICES_RECENTACTIVITYSERVICE_HPP
