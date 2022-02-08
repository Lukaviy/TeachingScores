#ifndef SUBJECTSDATAMODEL_H
#define SUBJECTSDATAMODEL_H

#include <QAbstractItemModel>
#include <Data.h>

class SubjectsDataModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit SubjectsDataModel(std::vector<ts::Subject>&& subjects);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void moveUp(int index);
    void moveDown(int index);
    void remove(int index);
    void add(int index, std::vector<std::string>&& names);

    std::vector<ts::Subject> getSubjects() const;
private:

    std::vector<ts::Subject> m_subjects;
    ts::IdGenerator<ts::Subject> m_idGenerator;
};

#endif // SUBJECTSDATAMODEL_H
