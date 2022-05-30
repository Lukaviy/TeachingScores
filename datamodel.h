#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractItemModel>
#include <Data.h>
#include <algorithm.h>

#include <ranges>

namespace ts {
    class ComputedDataModel {
    public:
        static ComputedDataModel compute(VerifiedData&& data);

        void setAppearance(Subject::Id, Article::Id, bool appearance);
        void setFirstAppearance(Subject::Id, Article::Id);

        Subject::Id addSubject(std::string&& name);
        Article::Id addArticle(std::string&& article);

        void renameArticle(int index, std::string&& name);
        void renameSubject(int index, std::string&& name);

        const std::vector<Subject>& getSubjects() const noexcept;
        const std::vector<Article>& getArticles() const noexcept;

        void setSubjects(std::vector<Subject>&& subjects);
        void removeArticle(Article::Id articleId);

        bool isArticleAppearedAt(Article::Id, Subject::Id) const;
        bool isArticleFirstAppearedAt(Article::Id, Subject::Id) const;

        const algorithm::ComputedData& getComputedDataForArticle(Article::Id id) const;

        void toggleSubjectAppearance(Article::Id);

        std::optional<float> getC_nu() const noexcept;

        void sort();

        VerifiedData getData() const noexcept;
    private:
        ComputedDataModel(Data&& data, std::map<Article::Id, algorithm::ComputedData>&& computedData, std::optional<float> C_nu, Article::Id lastArticleId, Subject::Id lastSubjectId);

        static std::optional<float> computeC_nu(const std::map<Article::Id, algorithm::ComputedData>& computedData);
        static float recomputeC_nu(float C_nu, float oldC, float newC, int size);

        algorithm::ComputedData computeData(Article::Id articleId) const;

        Data m_data;

        std::map<Article::Id, algorithm::ComputedData> m_computedData;
        std::optional<float> m_C_nu;
        unsigned m_lastArticleId = 0;
        unsigned m_lastSubjectId = 0;
    };
}

class DataModel : public QAbstractItemModel
{
    Q_OBJECT
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

    void addSubject(std::string&& name);
    void addArticle(std::string&& name);

    void sort();

    void toggleWholeRow(const QModelIndex &index);

    std::optional<int> getSubjectIndex(int column) const;

    const ts::ComputedDataModel& getData() const;

    const std::vector<ts::Subject>& getSubjects() const;

    void setSubjects(std::vector<ts::Subject>&& subjects);

    void toggleAppearance(const QModelIndex &index);

    void removeArticle(int row);

    static constexpr auto subjectsStart = 1;
    static constexpr auto reservedColumns = 3;

    int getSubjectsColumnIndexEnd() const;
    std::optional<float> getC_nu() const;

signals:
    void C_nu_changed(std::optional<float>);
private:
    ts::ComputedDataModel m_dataModel;
};

#endif // DATAMODEL_H
