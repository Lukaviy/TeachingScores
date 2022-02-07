#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include "dataformats.h"

namespace ts::formats {
    class JsonFormat : public DataExporter, public DataImporter
    {
    public:
        [[nodiscard]] tl::expected<ts::VerifiedData, std::string> importData(const QByteArray& data) noexcept override;
        [[nodiscard]] QByteArray exportData(const ts::VerifiedData& data) noexcept override;
    };
}

#endif // JSONFORMAT_H
