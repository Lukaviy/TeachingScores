#include "jsonformat.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

template <typename T>
tl::expected<std::vector<T>, std::string> parseVector(const QJsonArray& array) {
    std::vector<T> res;

    for (const auto& elementJson : array) {
        if (!elementJson.isObject()) {
            return tl::unexpected<std::string>("elements must have object type");
        }

        const auto elementObject = elementJson.toObject();

        if (!elementObject.contains("id")) {
            return tl::unexpected<std::string>("elements must have \'id\' field");
        }

        const auto idJson = elementObject["id"];

        if (!idJson.isDouble()) {
            return tl::unexpected<std::string>("\'id\' field must have integer type");
        }

        auto id = typename T::Id(idJson.toInt());

        if (!elementObject.contains("name")) {
            return tl::unexpected<std::string>("elements must have \'name\' field");
        }

        const auto nameJson = elementObject["name"];

        if (!nameJson.isString()) {
            return tl::unexpected<std::string>("\'name\' field in elements must have string type");
        }

        auto name = nameJson.toString().toStdString();

        res.push_back(T{ .id = std::move(id), .name = std::move(name) });
    }

    return res;
}


tl::expected<ts::VerifiedData, std::string> ts::formats::JsonFormat::importData(const QByteArray& data) const noexcept
{
    QJsonParseError error;
    auto json = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return tl::unexpected(error.errorString().toStdString());
    }

    if (!json.isObject()) {
        return tl::unexpected<std::string>("Root element must have object type");
    }

    auto root = json.object();

    if (!root.contains("subjects")) {
        return tl::unexpected<std::string>("Root object must contains \'subjects\' field");
    }

    std::vector<ts::Subject> subjects;
    {
        auto subjectsField = root["subjects"];

        if (!subjectsField.isArray()) {
            return tl::unexpected<std::string>("\'subjects\' field must have array type");
        }

        const auto subjectsArray = subjectsField.toArray();

        auto subjectsRes = parseVector<ts::Subject>(subjectsArray);

        if (!subjectsRes) {
            return tl::unexpected("Error at parse field 'subjects': " + subjectsRes.error());
        }

        subjects = std::move(subjectsRes).value();
    }

    std::vector<ts::Article> articles;
    {
        auto articlesField = root["articles"];

        if (!articlesField.isArray()) {
            return tl::unexpected<std::string>("\'articles\' field must have array type");
        }

        const auto articlesArray = articlesField.toArray();

        auto articlesRes = parseVector<ts::Article>(articlesArray);

        if (!articlesRes) {
            return tl::unexpected("Error at parse field 'articles': " + articlesRes.error());
        }

        articles = std::move(articlesRes).value();
    }

    std::map<Article::Id, std::set<Subject::Id>> appearance;
    {
        auto appearanceField = root["appearance"];

        if (!appearanceField.isObject()) {
            return tl::unexpected<std::string>("\'appearance\' field must have object type");
        }

        const auto appearanceObjet = appearanceField.toObject();

        for (auto iter = appearanceObjet.begin(); iter != appearanceObjet.end(); ++iter) {
            bool ok = false;
            auto articleIdInt = iter.key().toUInt(&ok);
            if (!ok) {
                return tl::unexpected<std::string>("\'articleId\' at 'appearance' field has bad format: " + iter.key().toStdString());
            }

            auto articleId = Article::Id(articleIdInt);

            if (appearance.count(articleId)) {
                return tl::unexpected<std::string>("duplicate article ids at 'appearance' list " + std::to_string(unsigned(articleId)));
            }

            if (!iter.value().isArray()) {
                return tl::unexpected<std::string>("field values at 'appearance' object must have array type");
            }

            const auto subjectIdListJson = iter.value().toArray();

            std::set<Subject::Id> subjectIdsSet;
            for (const auto& subjectIdJson : subjectIdListJson) {
                if (!subjectIdJson.isDouble()) {
                    return tl::unexpected<std::string>("field values at 'appearance' object must have array type");
                }

                auto subjectId = Subject::Id(subjectIdJson.toInt());

                if (subjectIdsSet.contains(subjectId)) {
                    return tl::unexpected<std::string>("duplicate subject ids at 'appearance' list " + std::to_string(unsigned(subjectId)));
                }

                subjectIdsSet.insert(subjectId);
            }

            appearance.insert_or_assign(articleId, std::move(subjectIdsSet));
        }
    }

    return VerifiedData::verify(Data{
                                    .subjects = std::move(subjects),
                                    .articles = std::move(articles),
                                    .appearance = std::move(appearance)
                                });
}

QByteArray ts::formats::JsonFormat::exportData(const ts::ComputedDataModel &data_model) const noexcept
{
    const auto data = data_model.getData();

    QJsonArray subjectsListJson;

    for (const auto& subject : data.data().subjects) {
        subjectsListJson.append(QJsonObject{
                                { "id", int(unsigned(subject.id)) },
                                { "name", QString::fromStdString(subject.name) }
                            });
    }

    QJsonArray articlesListJson;
    for (const auto& article : data.data().articles) {
        articlesListJson.append(QJsonObject{
                                { "id", int(unsigned(article.id)) },
                                { "name", QString::fromStdString(article.name) }
                            });
    }

    QJsonObject appearanceJson;
    for (const auto& [articleId, subjectIds] : data.data().appearance) {
        QJsonArray subjectIdsJson;

        for (const auto& subjectId : subjectIds) {
            subjectIdsJson.append(int(unsigned(subjectId)));
        }

        appearanceJson[QString::number(unsigned(articleId))] = std::move(subjectIdsJson);
    }

    return QJsonDocument(QJsonObject{
                             {"subjects", std::move(subjectsListJson)},
                             {"articles", std::move(articlesListJson)},
                             {"appearance", std::move(appearanceJson)}
                         }).toJson();
}
