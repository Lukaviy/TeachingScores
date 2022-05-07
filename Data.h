#ifndef DATA_H
#define DATA_H

#include <map>
#include <set>
#include <string>
#include <QUuid>
#include "KeyId.h"

#include "libs/expected/include/tl/expected.hpp"

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
        std::map<Article::Id, std::set<Subject::Id>> appearance;
    };

    struct VerifiedData {
        Data&& data() && noexcept;
        const Data& data() const & noexcept;

        static tl::expected<VerifiedData, std::string> initializeWithDefaults(std::vector<Subject>&& subjects, std::vector<Article>&& articles);
        static tl::expected<VerifiedData, std::string> verify(Data&& data);

        static VerifiedData unverifiedFromRawData(Data&& data);
    private:
        static tl::expected<std::tuple<std::set<Article::Id>, std::set<Subject::Id>>, std::string> checkDuplicates(const std::vector<Subject>& subjects, const std::vector<Article>& articles);
        VerifiedData(Data&& data);

        Data m_data;
    };

}

#endif // DATA_H
