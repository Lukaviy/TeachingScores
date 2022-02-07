#include "Data.h"

ts::VerifiedData::VerifiedData(Data &&data) : m_data(std::move(data)) {}

ts::Data &&ts::VerifiedData::data() &&noexcept
{
    return std::move(m_data);
}

const ts::Data &ts::VerifiedData::data() const &noexcept
{
    return m_data;
}

tl::expected<ts::VerifiedData, std::string> ts::VerifiedData::initializeWithDefaults(std::vector<Subject>&& subjects, std::vector<Article>&& articles)
{
    if (subjects.empty()) {
        throw std::exception("There must be at least one subject");
    }

    std::map<Article::Id, Subject::Id> firstAppearance;
    std::map<Article::Id, std::set<Subject::Id>> appearance;

    if (auto error = checkDuplicates(subjects, articles); !error) {
        return tl::unexpected(std::move(error).error());
    }

    for (const auto& article : articles) {
        firstAppearance.insert_or_assign(article.id, subjects.front().id);
        appearance.insert_or_assign(article.id, std::set<Subject::Id>{ subjects.front().id });
    }

    return ts::VerifiedData(ts::Data{
        .subjects = std::move(subjects),
        .articles = std::move(articles),
        .firstAppearance = std::move(firstAppearance),
        .appearance = std::move(appearance)
    });
}

tl::expected<ts::VerifiedData, std::string> ts::VerifiedData::verify(Data &&data)
{
    auto verifyRes = checkDuplicates(data.subjects, data.articles);

    if (!verifyRes) {
        return tl::unexpected(std::move(verifyRes).error());
    }

    const auto [articleIds, subjectIds] = std::move(verifyRes).value();

    for (const auto& [articleId, subjectId] : data.firstAppearance) {
        if (!articleIds.count(articleId)) {
            return tl::unexpected("article with id " + std::to_string(unsigned(articleId)) + " is not found");
        }
        if (!subjectIds.count(subjectId)) {
            return tl::unexpected("subject with id " + std::to_string(unsigned(subjectId)) + " is not found");
        }
    }

    for (const auto& [articleId, linkedSubjectIds] : data.appearance) {
        if (!articleIds.count(articleId)) {
            return tl::unexpected("article with id " + std::to_string(unsigned(articleId)) + " is not found");
        }

        for (const auto& linkedSubjectId : linkedSubjectIds) {
            if (!subjectIds.count(linkedSubjectId)) {
                return tl::unexpected("subject with id " + std::to_string(unsigned(linkedSubjectId)) + " is not found");
            }
        }
    }

    for (const auto& article : data.articles) {
        if (!data.appearance.count(article.id)) {
            return tl::unexpected("appearance is not found for article with id " + std::to_string(unsigned(article.id)));
        }
        if (!data.firstAppearance.count(article.id)) {
            return tl::unexpected("first appearance is not found for article with id " + std::to_string(unsigned(article.id)));
        }
    }

    return ts::VerifiedData(std::move(data));
}

ts::VerifiedData ts::VerifiedData::unverifiedFromRawData(Data&& data)
{
    return ts::VerifiedData(std::move(data));
}

tl::expected<std::tuple<std::set<ts::Article::Id>, std::set<ts::Subject::Id>>, std::string> ts::VerifiedData::checkDuplicates(const std::vector<Subject> &subjects, const std::vector<Article> &articles)
{
    std::set<Subject::Id> subjectIds;

    for (const auto& subject : subjects) {
        if (subjectIds.count(subject.id)) {
            return tl::unexpected<std::string>("there are duplicate subject ids: " + std::to_string(unsigned(subject.id)));
        }
        subjectIds.insert(subject.id);
    }

    std::set<Article::Id> articleIds;

    for (const auto& article : articles) {
        if (articleIds.count(article.id)) {
            return tl::unexpected<std::string>("there are duplicate article ids: " + std::to_string(unsigned(article.id)));
        }
        articleIds.insert(article.id);
    }

    return std::tuple(std::move(articleIds), std::move(subjectIds));
}
