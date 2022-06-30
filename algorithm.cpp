#include "algorithm.h"
#include <ranges>

using namespace ts;
using namespace ts::algorithm;

ComputedData ts::algorithm::computeOuterLinks(const std::vector<Subject>& subjects, const std::map<Article::Id, Subject::Id> &firstAppearance, const std::map<Article::Id, std::set<Subject::Id>> &appearance, Article::Id articleId)
{
    const auto& articleAppearance = appearance.at(articleId);

    auto i_max = 0u;

    for (auto i = 0u; i < subjects.size(); i++) {
        if (articleAppearance.contains(subjects[i].id)) {
            i_max = i + 1;
        }
    }

    const auto firstAppearanceSubjectId = firstAppearance.at(articleId);

    const auto t_m = std::distance(subjects.begin(), std::ranges::find(subjects, firstAppearanceSubjectId, [](const auto& v) {return v.id;})) + 1;

    const auto t_p = std::min(std::distance(subjects.begin(), std::ranges::find_first_of(subjects, articleAppearance, {}, [](const auto& v) {return v.id;})) + 1, t_m);

    std::vector<int> a_l(subjects.size());

    {
        const auto p_2 = 2;
        const auto p_1 = 1;

        for (auto i = 0u; i < subjects.size(); i++) {
            if (articleAppearance.count(subjects[i].id) == 0) {
                continue;
            }

            if (t_p == t_m && t_m <= i + 1) {
                a_l[i] = p_2 * (i + 1 - t_p + 1);
            }
            if (t_p < t_m && t_m <= i + 1) {
                a_l[i] = p_1 * (t_m - t_p) + p_2 * (i + 1 - t_m + 1);
            }
            if (t_p <= i + 1 && i + 1 < t_m) {
                a_l[i] = p_1 * (i + 1 - t_p + 1);
            }
        }
    }

    std::vector<int> a_l_t(subjects.size());

    {
        const auto r = [&](unsigned i, unsigned j) {
            auto res = 0u;

            for (auto k = i; k < j; k++) {
                if (articleAppearance.count(subjects[k].id) == 0) {
                    res++;
                }
            }

            return res;
        };

        for (auto i = 0u; i < subjects.size(); i++) {
            if (articleAppearance.count(subjects[i].id) == 0) {
                continue;
            }

            if (t_p == t_m && t_m <= i + 1) {
                a_l_t[i] = a_l[i] - r(t_p, i + 1);
            }
            if (t_p < t_m && t_m <= i + 1) {
                a_l_t[i] = a_l[i] - r(t_p, t_m) - r(t_m, i + 1);
            }
            if (t_p <= i + 1 && i + 1 < t_m) {
                a_l_t[i] = a_l[i] - r(t_p, i + 1);
            }
        }
    }

    const auto l = float(i_max - t_p + 1) / subjects.size();

    auto c = 0.f;
    {
        for (auto i = 0u; i < subjects.size(); i++) {
            if (a_l_t[i] != 0 && a_l[i] != 0) {
                c += float(a_l_t[i]) / float(a_l[i]);
            }
        }

        c /= subjects.size();
    }

    const auto h = l * c;

    return ComputedData { .l = l, .c = c, .h = h };
}
