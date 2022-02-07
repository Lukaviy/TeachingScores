#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractItemModel>
#include <Data.h>
#include <algorithm.h>

#include <ranges>

namespace ts {
    class ComputedDataModel {
    public:
        static ComputedDataModel compute(Data&& data);

        void setAppearance(Subject::Id, Article::Id, bool appearance);
        void setFirstAppearance(Subject::Id, Article::Id);

        void addSubject(Subject&& subject);
        void addArticle(Article&& article);

        void renameArticle(int index, std::string&& name);
        void renameSubject(int index, std::string&& name);

        const std::vector<Subject>& getSubjects() const noexcept;
        const std::vector<Article>& getArticles() const noexcept;

        bool isArticleAppearedAt(Article::Id, Subject::Id) const noexcept;
        bool isArticleFirstAppearedAt(Article::Id, Subject::Id) const noexcept;

        const algorithm::ComputedData getComputedDataForArticle(Article::Id id) const noexcept;
    private:
        ComputedDataModel(Data&& data, std::map<Article::Id, algorithm::ComputedData>&& computedData);

        Data m_data;

        std::map<Article::Id, algorithm::ComputedData> m_computedData;
    };
}

class DataModel : public QAbstractItemModel
{
public:
    DataModel(ts::ComputedDataModel&& dataModel);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    void addSubject(ts::Subject&& subject);
    void addArticle(ts::Article&& article);

    std::optional<int> getSubjectIndex(int column) const;

    static constexpr auto subjectsStart = 1;
    static constexpr auto reservedColumns = 3;

    int getSubjectsColumnIndexEnd() const;
private:
    ts::ComputedDataModel m_dataModel;
};

#endif // DATAMODEL_H
