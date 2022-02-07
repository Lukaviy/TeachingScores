#include "Data.h"

ts::Data ts::initializeWithDefaults(std::vector<Subject>&& subjects, std::vector<Article>&& articles)
{
    if (subjects.empty()) {
        throw std::exception("There must be at least one subject");
    }

    std::map<Article::Id, Subject::Id> firstAppearance;
    std::map<Article::Id, std::set<Subject::Id>> appearance;

    for (const auto& article : articles) {
        firstAppearance.insert_or_assign(article.id, subjects.front().id);
        appearance.insert_or_assign(article.id, std::set<Subject::Id>{ subjects.front().id });
    }

    return ts::Data{
        .subjects = std::move(subjects),
        .articles = std::move(articles),
        .firstAppearance = std::move(firstAppearance),
        .appearance = std::move(appearance)
    };
}
