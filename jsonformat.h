#ifndef JSONFORMAT_H
#define JSONFORMAT_H

#include "dataformats.h"

namespace ts::formats {
    class JsonFormat : public DataExporter, public DataImporter
    {
    public:
        ts::Data importData(const QByteArray& data) override;
        QByteArray exportData(const ts::Data& data) override;
    };
}

#endif // JSONFORMAT_H
