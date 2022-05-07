#include "datamodel.h"

#include <algorithm>
#include <ranges>
#include <QColor>
#include <QBrush>

using namespace ts;

ComputedDataModel ComputedDataModel::compute(VerifiedData &&verified_data)
{
    auto data = std::move(verified_data).data();
    std::map<Article::Id, algorithm::ComputedData> computedData;
    for (const auto& article : data.articles) {
        auto computedDataForArticle = ts::algorithm::computeOuterLinks(data.subjects, data.firstAppearance, data.appearance, article.id);
        computedData.insert_or_assign(article.id, std::move(computedDataForArticle));
    }

    auto lastArticleId = data.articles.empty() ? Article::Id{0} : data.articles.front().id;
    for (const auto& article: data.articles) {
        lastArticleId = std::max(article.id, lastArticleId);
    }

    auto lastSubjectId = data.subjects.front().id;
    for (const auto& subject: data.subjects) {
        lastSubjectId = std::max(subject.id, lastSubjectId);
    }

    const auto C_nu = computeC_nu(computedData);

    return ComputedDataModel(std::move(data), std::move(computedData), C_nu, lastArticleId, lastSubjectId);
}

class ThereMustBeAtLeastOneSubject : public std::exception {};

void ComputedDataModel::setAppearance(Subject::Id subjectId, Article::Id articleId, bool appearance)
{
    auto& articleAppearance = m_data.appearance[articleId];

    if (appearance) {
        articleAppearance.insert(subjectId);
    } else {
        if (articleAppearance.size() == 1 && articleAppearance.count(subjectId) == 1) {
            throw ThereMustBeAtLeastOneSubject{};
        }
        articleAppearance.erase(subjectId);
    }

    const auto oldC = m_computedData.at(articleId).c;
    const auto newComputedData = algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId);

    m_computedData.insert_or_assign(articleId, newComputedData);

    m_C_nu = recomputeC_nu(m_C_nu.value(), oldC, newComputedData.c, int(m_computedData.size()));
}

void ComputedDataModel::setFirstAppearance(Subject::Id subjectId, Article::Id articleId)
{
    m_data.firstAppearance.insert_or_assign(articleId, subjectId);

    const auto oldC = m_computedData.at(articleId).c;

    const auto newComputedData = algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId);

    m_computedData.insert_or_assign(articleId, newComputedData);

    m_C_nu = recomputeC_nu(m_C_nu.value(), oldC, newComputedData.c, int(m_computedData.size()));
}

Subject::Id ComputedDataModel::addSubject(std::string&& name)
{
    auto subjectId = Subject::Id(++m_lastSubjectId);

    m_data.subjects.emplace_back(Subject { .id = subjectId, .name = std::move(name) });

    for (auto& [articleId, data] : m_computedData) {
        data = ts::algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId);
    }

    m_C_nu = computeC_nu(m_computedData);

    return subjectId;
}

Article::Id ComputedDataModel::addArticle(std::string&& name)
{
    auto articleId = Article::Id(++m_lastArticleId);

    m_data.articles.emplace_back(Article { .id = articleId, .name = std::move(name) });

    m_data.appearance.insert_or_assign(articleId, std::set<Subject::Id> { m_data.subjects.front().id });
    m_data.firstAppearance.insert_or_assign(articleId, m_data.subjects.front().id);

    m_computedData.insert_or_assign(articleId, algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId));

    m_C_nu = computeC_nu(m_computedData);

    return articleId;
}

void ComputedDataModel::renameArticle(int index, std::string &&name)
{
    m_data.articles.at(index).name = std::move(name);
}

void ComputedDataModel::renameSubject(int index, std::string &&name)
{
    m_data.subjects.at(index).name = std::move(name);
}

const std::vector<Subject> &ComputedDataModel::getSubjects() const noexcept
{
    return m_data.subjects;
}

const std::vector<Article> &ComputedDataModel::getArticles() const noexcept
{
    return m_data.articles;
}

void ComputedDataModel::setSubjects(std::vector<Subject>&& subjects)
{
    if (subjects.empty()) {
        throw std::exception("There must be atleast one subject");
    }

    std::set<Subject::Id> subjectIds;

    for (const auto& [id, _] : subjects) {
        if (subjectIds.contains(id)) {
            std::exception("There are diplicated subject ids");
        }
        subjectIds.insert(id);
    }

    m_data.subjects = std::move(subjects);

    for (auto& [articleId, m] : m_data.appearance) {
        for (auto iter = m.begin(); iter != m.end();) {
            if (!subjectIds.contains(*iter)) {
                iter = m.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    for (auto iter = m_data.firstAppearance.begin(); iter != m_data.firstAppearance.end();) {
        if (!subjectIds.contains(iter->second)) {
            iter = m_data.firstAppearance.erase(iter);
        } else {
            ++iter;
        }
    }
}

void ComputedDataModel::removeArticle(Article::Id articleId)
{
    auto iter = std::ranges::remove(m_data.articles, articleId, &Article::id);
    m_data.articles.erase(iter.begin(), iter.end());

    m_data.appearance.erase(articleId);
    m_data.firstAppearance.erase(articleId);

    m_computedData.erase(articleId);
}

bool ComputedDataModel::isArticleAppearedAt(Article::Id articleId, Subject::Id subjectId) const
{
    auto articleColumn = m_data.appearance.find(articleId);

    if (articleColumn == m_data.appearance.end()) {
        return false;
    }

    return articleColumn->second.contains(subjectId);
}

bool ComputedDataModel::isArticleFirstAppearedAt(Article::Id articleId, Subject::Id subjectId) const
{
    auto articleColumn = m_data.firstAppearance.find(articleId);

    if (articleColumn == m_data.firstAppearance.end()) {
        throw std::exception("There are no such article");
    }

    return articleColumn->second == subjectId;
}

const algorithm::ComputedData& ComputedDataModel::getComputedDataForArticle(Article::Id id) const
{
    auto article = m_computedData.find(id);

    if (article == m_computedData.end()) {
        throw std::exception("There are no such article");
    }

    return article->second;
}

void ComputedDataModel::toggleSubjectAppearance(Article::Id id)
{
    if (m_data.appearance[id].size() == 1) {
        std::set<Subject::Id> subjectsSet;
        for (const auto& subject : m_data.subjects) {
            subjectsSet.insert(subject.id);
        }
        m_data.appearance[id] = subjectsSet;
    } else {
        m_data.appearance[id].clear();
        m_data.appearance[id].insert(m_data.subjects.front().id);
    }

    m_computedData[id] = getComputedDataForArticle(id);
}

std::optional<float> ComputedDataModel::getC_nu() const noexcept
{
    return m_C_nu;
}

void ComputedDataModel::sort()
{
    std::vector<std::pair<Article, float>> articlesWithC;

    articlesWithC.reserve(m_data.articles.size());

    for (const auto& articleId : m_data.articles) {
        articlesWithC.push_back({articleId, m_computedData.at(articleId.id).h});
    }

    std::ranges::sort(articlesWithC, std::greater{}, [](const auto& t) { return t.second; });

    for (auto i = 0u; i < articlesWithC.size(); i++) {
        m_data.articles[i] = std::move(articlesWithC[i].first);
    }
}

VerifiedData ComputedDataModel::getData() const noexcept
{
    return ts::VerifiedData::unverifiedFromRawData(ts::Data(m_data));
}

ComputedDataModel::ComputedDataModel(Data&& data, std::map<Article::Id, algorithm::ComputedData>&& computedData, std::optional<float> C_nu, Article::Id lastArticleId, Subject::Id lastSubjectId)
    : m_data(std::move(data)), m_computedData(std::move(computedData)), m_C_nu(C_nu), m_lastArticleId(lastArticleId), m_lastSubjectId(lastSubjectId)
{

}

std::optional<float> ComputedDataModel::computeC_nu(const std::map<Article::Id, algorithm::ComputedData>& computedData)
{
    if (computedData.empty()) {
        return std::nullopt;
    }

    float c_sum = 0;

    for (const auto& [_, data] : computedData) {
        c_sum += data.c;
    }

    return c_sum / computedData.size();
}

float ComputedDataModel::recomputeC_nu(float C_nu, float oldC, float newC, int size)
{
    return C_nu - (oldC - newC) / size;
}

DataModel::DataModel(ts::ComputedDataModel&& dataModel) : m_dataModel(std::move(dataModel))
{

}

QModelIndex DataModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex DataModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int DataModel::rowCount(const QModelIndex &parent) const
{
    return static_cast<int>(m_dataModel.getArticles().size());
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(m_dataModel.getSubjects().size() + 4);
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    const auto& article = m_dataModel.getArticles()[index.row()];

    if (role == Qt::ItemDataRole::DisplayRole) {
        if (index.column() == 0) {
            return QString::fromStdString(article.name);
        }

        const auto getComputedData = [&]() {
            return m_dataModel.getComputedDataForArticle(article.id);
        };

        const auto colCount = columnCount(QModelIndex());

        if (index.column() == colCount - 3) {
            return QString::number(getComputedData().l, 'f', 2);
        }
        if (index.column() == colCount - 2) {
            return QString::number(getComputedData().c, 'f', 2);
        }
        if (index.column() == colCount - 1) {
            return QString::number(getComputedData().h, 'f', 2);
        }
    }

    if (role == Qt::ItemDataRole::EditRole) {
        if (index.column() == 0) {
            return QString::fromStdString(article.name);
        }
    }

    if (auto subjectIndex = getSubjectIndex(index.column())) {
        const auto& subject = m_dataModel.getSubjects().at(subjectIndex.value());

        if (role == Qt::DisplayRole) {
            return m_dataModel.isArticleAppearedAt(article.id, subject.id) ? QString::fromWCharArray(L"🔴") : QVariant();
        }
        if (role == Qt::BackgroundRole) {
            return m_dataModel.isArticleFirstAppearedAt(article.id, subject.id) ? QBrush(QColor(Qt::gray)) : QVariant();
        }
        if (role == Qt::TextAlignmentRole) {
            return Qt::AlignCenter;
        }
    }

    return QVariant();
}

QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::ItemDataRole::DisplayRole && orientation == Qt::Orientation::Horizontal) {
        if (section == 0) {
            return "Article Name";
        }

        const auto colCount = columnCount(QModelIndex());

        if (section == colCount - 3) {
            return "L";
        }
        if (section == colCount - 2) {
            return "C";
        }
        if (section == colCount - 1) {
            return "h";
        }

        const auto& subject = m_dataModel.getSubjects().at(section - subjectsStart);

        return QString::fromStdString(subject.name) + "\n" + QString::number(section);
    }

    if (role == Qt::ItemDataRole::DisplayRole && orientation == Qt::Orientation::Vertical) {
        return section + 1;
    }

    return QVariant();
}

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    if (index.column() < subjectsStart) {
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    }

    return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

bool DataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (index.column() == 0) {
            m_dataModel.renameArticle(index.row(), value.toString().toStdString());

            emit dataChanged(index, index, QList<int> {role});

            return true;
        }
    }

    if (auto subjectsIndex = getSubjectIndex(index.column())) {
        const auto& article = m_dataModel.getArticles().at(index.row());
        const auto& subject = m_dataModel.getSubjects().at(subjectsIndex.value());


        if (role == Qt::CheckStateRole) {
            try {
                m_dataModel.setAppearance(subject.id, article.id, value == Qt::Checked);
            } catch (const ThereMustBeAtLeastOneSubject&) {
                return false;
            }

            emit dataChanged(index, index);
            emit dataChanged(createIndex(index.row(), getSubjectsColumnIndexEnd() - 1), createIndex(index.row(), columnCount(QModelIndex()) - 1));

            emit C_nu_changed(m_dataModel.getC_nu());

            return true;
        }

        if (role == Qt::EditRole && value.toBool()) {
            m_dataModel.setFirstAppearance(subject.id, article.id);

            emit dataChanged(createIndex(index.row(), subjectsStart), createIndex(index.row(), columnCount(QModelIndex()) - 1));

            emit C_nu_changed(m_dataModel.getC_nu());

            return true;
        }
    }

    return false;
}

bool DataModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (role == Qt::EditRole && orientation == Qt::Horizontal) {
        if (section >= subjectsStart && section < getSubjectsColumnIndexEnd()) {
            m_dataModel.renameSubject(section - subjectsStart, value.toString().toStdString());

            emit headerDataChanged(orientation, section, section);

            return true;
        }
    }

    return false;
}

void DataModel::addSubject(std::string&& name)
{
    const auto subjectsEndColumnIndex = getSubjectsColumnIndexEnd();
    beginInsertColumns(QModelIndex(), subjectsEndColumnIndex, subjectsEndColumnIndex);
    m_dataModel.addSubject(std::move(name));
    endInsertColumns();
}

void DataModel::addArticle(std::string&& name)
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    m_dataModel.addArticle(std::move(name));
    endInsertRows();
}

void DataModel::sort()
{
    m_dataModel.sort();

    emit dataChanged(createIndex(0, 0), createIndex(rowCount(QModelIndex()) - 1, columnCount(QModelIndex()) - 1));
}

void DataModel::toggleWholeRow(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    m_dataModel.toggleSubjectAppearance(m_dataModel.getArticles()[index.row()].id);

    emit dataChanged(createIndex(index.row(), 0), createIndex(index.row(), columnCount(QModelIndex()) - 1));
}

std::optional<int> DataModel::getSubjectIndex(int column) const
{
    if (column >= subjectsStart && column < getSubjectsColumnIndexEnd()) {
        return column - subjectsStart;
    }

    return std::nullopt;
}

const ts::ComputedDataModel& DataModel::getData() const
{
    return m_dataModel;
}

const std::vector<ts::Subject>& DataModel::getSubjects() const
{
    return m_dataModel.getSubjects();
}

void DataModel::setSubjects(std::vector<ts::Subject> &&subjects)
{
    m_dataModel.setSubjects(std::move(subjects));

    emit dataChanged(createIndex(0, 0), createIndex(rowCount(QModelIndex()) - 1, columnCount(QModelIndex()) - 1));
    emit headerDataChanged(Qt::Horizontal, 0, columnCount(QModelIndex()) - 1);
}

void DataModel::toggleAppearance(const QModelIndex &index)
{
    const auto& article = m_dataModel.getArticles().at(index.row());

    if (auto subjectIndex = getSubjectIndex(index.column())) {
        const auto& subject = m_dataModel.getSubjects().at(subjectIndex.value());

        setData(index, m_dataModel.isArticleAppearedAt(article.id, subject.id) ? Qt::Unchecked : Qt::Checked, Qt::CheckStateRole);
    }
}

void DataModel::removeArticle(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_dataModel.removeArticle(m_dataModel.getArticles().at(row).id);
    endRemoveRows();
}

int DataModel::getSubjectsColumnIndexEnd() const
{
    return columnCount(QModelIndex()) - reservedColumns;
}

std::optional<float> DataModel::getC_nu() const
{
    return m_dataModel.getC_nu();
}
