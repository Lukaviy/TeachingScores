#ifndef CSVFORMAT_H
#define CSVFORMAT_H

#include "datamodel.h"
#include "dataformats.h"

namespace ts::formats {
    class CsvFormat : public ts::formats::DataExporter
    {
    public:
        QByteArray exportData(const ts::ComputedDataModel &data) const noexcept override;
    };
}

#endif // CSVFORMAT_H
