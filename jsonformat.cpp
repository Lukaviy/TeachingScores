#include "jsonformat.h"

#include <QJsonValue>

ts::Data ts::formats::JsonFormat::importData(const std::vector<std::byte>& data)
{
    QByteArray::fromRawData(data.data(), data.size());
}
