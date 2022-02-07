#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Data.h"

namespace ts::algorithm {
    struct ComputedData {
        float l = 0;
        float c = 0;
        float h = 0;
    };

    ComputedData computeOuterLinks(const std::vector<Subject>& subjects, const std::map<Article::Id, Subject::Id>& firstAppearance, const std::map<Article::Id, std::set<Subject::Id>>& appearance, Article::Id articleId);
}

#endif // ALGORITHM_H
