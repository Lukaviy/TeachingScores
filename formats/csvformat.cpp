#include "csvformat.h"

#include <QStringList>
#include <QStringBuilder>

QByteArray ts::formats::CsvFormat::exportData(const ts::ComputedDataModel &data) const noexcept
{
    QStringList res;

    {
        QStringList line;
        line << "Article Names";

        for (const auto& subject : data.getSubjects()) {
            line << QString::fromStdString(subject.name);
        }

        line << "L" << "C" << "h";

        res << std::move(line).join(',');

        line.clear();
    }

    for (const auto& article : data.getArticles()) {
        QStringList line;

        line << QString::fromStdString(article.name);

        for (const auto& subject : data.getSubjects()) {
            QString cell;
            if (data.isArticleAppearedAt(article.id, subject.id)) {
                cell += "🔴";
            }

            if (data.isArticleFirstAppearedAt(article.id, subject.id)) {
                cell += "✅";
            }

            line << std::move(cell);
        }

        const auto computedData = data.getComputedDataForArticle(article.id);

        line << QString::number(computedData.l) << QString::number(computedData.c) << QString::number(computedData.h);

        res << std::move(line).join(',');
    }

    return res.join('\n').toUtf8();
}
