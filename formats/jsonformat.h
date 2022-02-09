#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include "dataformats.h"
#include "datamodel.h"

namespace ts::formats {
    class JsonFormat : public DataExporter, public DataImporter
    {
    public:
        [[nodiscard]] tl::expected<ts::VerifiedData, std::string> importData(const QByteArray& data) const noexcept override;
        [[nodiscard]] QByteArray exportData(const ts::ComputedDataModel& data) const noexcept override;
    };
}

#endif // JSONFORMAT_H
