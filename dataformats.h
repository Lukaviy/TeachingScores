#ifndef DATAFORMATS_H
#define DATAFORMATS_H

#include "Data.h"

namespace ts::formats {
    class DataExporter {
    public:
        virtual ~DataExporter() = 0;
        virtual QByteArray exportData(const ts::Data& data) = 0;
    };

    class DataImporter {
    public:
        virtual ~DataImporter() = 0;
        virtual ts::Data importData(const QByteArray& data) = 0;
    };
}

#endif // DATAFORMATS_H
