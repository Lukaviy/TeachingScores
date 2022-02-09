#ifndef DATAFORMATS_H
#define DATAFORMATS_H

#include "datamodel.h"
#include "libs/expected/include/tl/expected.hpp"

namespace ts::formats {
    class DataExporter {
    public:
        virtual ~DataExporter() = default;
        [[nodiscard]] virtual QByteArray exportData(const ts::ComputedDataModel& data) const noexcept = 0;
    };

    class DataImporter {
    public:
        virtual ~DataImporter() = default;
        [[nodiscard]] virtual tl::expected<ts::VerifiedData, std::string> importData(const QByteArray& data) const noexcept = 0;
    };
}

#endif // DATAFORMATS_H
