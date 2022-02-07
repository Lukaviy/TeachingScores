#include "datamodel.h"

using namespace ts;

ComputedDataModel ComputedDataModel::compute(VerifiedData &&verified_data)
{
    auto data = std::move(verified_data).data();
    std::map<Article::Id, algorithm::ComputedData> computedData;
    for (const auto& article : data.articles) {
        auto computedDataForArticle = ts::algorithm::computeOuterLinks(data.subjects, data.firstAppearance, data.appearance, article.id);
        computedData.insert_or_assign(article.id, std::move(computedDataForArticle));
    }

    return ComputedDataModel(std::move(data), std::move(computedData));
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

    m_computedData.insert_or_assign(articleId, algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId));
}

void ComputedDataModel::setFirstAppearance(Subject::Id subjectId, Article::Id articleId)
{
    m_data.firstAppearance.insert_or_assign(articleId, subjectId);

    m_computedData.insert_or_assign(articleId, algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId));
}

void ComputedDataModel::addSubject(Subject &&subject)
{
    m_data.subjects.emplace_back(std::move(subject));

    for (auto& [articleId, data] : m_computedData) {
        data = ts::algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId);
    }
}

void ComputedDataModel::addArticle(Article &&article)
{
    auto articleId = article.id;

    m_data.articles.emplace_back(std::move(article));

    m_data.appearance.insert_or_assign(articleId, std::set<Subject::Id> { m_data.subjects.front().id });
    m_data.firstAppearance.insert_or_assign(articleId, m_data.subjects.front().id);

    m_computedData.insert_or_assign(articleId, algorithm::computeOuterLinks(m_data.subjects, m_data.firstAppearance, m_data.appearance, articleId));
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

bool ComputedDataModel::isArticleAppearedAt(Article::Id articleId, Subject::Id subjectId) const noexcept
{
    auto articleColumn = m_data.appearance.find(articleId);

    if (articleColumn == m_data.appearance.end()) {
        return false;
    }

    return articleColumn->second.contains(subjectId);
}

bool ComputedDataModel::isArticleFirstAppearedAt(Article::Id articleId, Subject::Id subjectId) const noexcept
{
    auto articleColumn = m_data.firstAppearance.find(articleId);

    if (articleColumn == m_data.firstAppearance.end()) {
        return false;
    }

    return articleColumn->second == subjectId;
}

const algorithm::ComputedData ComputedDataModel::getComputedDataForArticle(Article::Id id) const noexcept
{
    auto article = m_computedData.find(id);

    if (article == m_computedData.end()) {
        return algorithm::ComputedData();
    }

    return article->second;
}

VerifiedData ComputedDataModel::getData() const noexcept
{
    return ts::VerifiedData::unverifiedFromRawData(ts::Data(m_data));
}

ComputedDataModel::ComputedDataModel(Data&& data, std::map<Article::Id, algorithm::ComputedData>&& computedData) : m_data(std::move(data)), m_computedData(std::move(computedData))
{

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


    if (auto subjectIndex = getSubjectIndex(index.column())) {
        const auto& subject = m_dataModel.getSubjects().at(subjectIndex.value());

        if (role == Qt::CheckStateRole) {
            return m_dataModel.isArticleAppearedAt(article.id, subject.id) ? Qt::Checked : Qt::Unchecked;
        }
        if (role == Qt::DisplayRole) {
            return m_dataModel.isArticleFirstAppearedAt(article.id, subject.id) ? QString::fromWCharArray(L"✔️") : QString::fromWCharArray(L"❌");
        }
        if (role == Qt::EditRole) {
            return m_dataModel.isArticleFirstAppearedAt(article.id, subject.id);
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
    if (index.column() < getSubjectsColumnIndexEnd()) {
        auto flags = Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
        return index.column() >= subjectsStart ? (flags | Qt::ItemIsUserCheckable) : flags;
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

            return true;
        }

        if (role == Qt::EditRole && value.toBool()) {
            m_dataModel.setFirstAppearance(subject.id, article.id);

            emit dataChanged(createIndex(index.row(), subjectsStart), createIndex(index.row(), columnCount(QModelIndex()) - 1));

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

void DataModel::addSubject(ts::Subject &&subject)
{
    const auto subjectsEndColumnIndex = getSubjectsColumnIndexEnd();
    beginInsertColumns(QModelIndex(), subjectsEndColumnIndex, subjectsEndColumnIndex);
    m_dataModel.addSubject(std::move(subject));
    endInsertColumns();
}

void DataModel::addArticle(ts::Article &&article)
{
    beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
    m_dataModel.addArticle(std::move(article));
    endInsertRows();
}

std::optional<int> DataModel::getSubjectIndex(int column) const
{
    if (column >= subjectsStart && column < getSubjectsColumnIndexEnd()) {
        return column - subjectsStart;
    }

    return std::nullopt;
}

VerifiedData DataModel::getData() const
{
    return m_dataModel.getData();
}

int DataModel::getSubjectsColumnIndexEnd() const
{
    return columnCount(QModelIndex()) - reservedColumns;
}
