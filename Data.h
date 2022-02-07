#ifndef DATA_H
#define DATA_H

#include <map>
#include <set>
#include <string>
#include <QUuid>
#include "KeyId.h"

namespace ts {
    struct Subject {
        using Id = KeyId<Subject>;
        Id id;
        std::string name;
    };

    struct Article {
        using Id = KeyId<Article>;
        Id id;
        std::string name;
    };

    struct Data {
        std::vector<Subject> subjects;
        std::vector<Article> articles;
        std::map<Article::Id, Subject::Id> firstAppearance;
        std::map<Article::Id, std::set<Subject::Id>> appearance;
    };

    Data initializeWithDefaults(std::vector<Subject>&& subjects, std::vector<Article>&& articles);
}

#endif // DATA_H
