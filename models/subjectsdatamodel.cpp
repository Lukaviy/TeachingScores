#include "subjectsdatamodel.h"

SubjectsDataModel::SubjectsDataModel(std::vector<ts::Subject>&& subjects) : m_subjects(std::move(subjects))
{
    if (!m_subjects.empty()) {
        auto lastSubjectId = m_subjects.front().id;

        for (const auto& subject : m_subjects) {
            lastSubjectId = std::max(subject.id, lastSubjectId);
        }

        m_idGenerator = ts::IdGenerator(std::move(lastSubjectId));
    }
}

QVariant SubjectsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal && section == 0) {
            return "Subject Name";
        }

        if (orientation == Qt::Vertical) {
            return section;
        }
    }

    return QVariant();
}

QModelIndex SubjectsDataModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex SubjectsDataModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int SubjectsDataModel::rowCount(const QModelIndex &parent) const
{
    return int(m_subjects.size());
}

int SubjectsDataModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant SubjectsDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return QString::fromStdString(m_subjects[index.row()].name);
    }

    return QVariant();
}

bool SubjectsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        m_subjects[index.row()].name = value.toString().toStdString();

        emit dataChanged(index, index, QVector<int> { role });
        return true;
    }

    return false;
}

Qt::ItemFlags SubjectsDataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void SubjectsDataModel::moveUp(int index)
{
    if (index > 0 && index < m_subjects.size()) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index - 1);
        beginMoveRows(QModelIndex(), index - 1, index - 1, QModelIndex(), index);
        std::swap(m_subjects[index - 1], m_subjects[index]);

        emit dataChanged(createIndex(index - 1, 0), createIndex(index, 0));
        endMoveRows();
    }
}

void SubjectsDataModel::moveDown(int index)
{
    if (index >= 0 && index < m_subjects.size() - 1) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), index + 1);
        beginMoveRows(QModelIndex(), index+1, index+1, QModelIndex(), index);
        std::swap(m_subjects[index], m_subjects[index + 1]);

        emit dataChanged(createIndex(index, 0), createIndex(index + 1, 0));
        endMoveRows();
    }
}

void SubjectsDataModel::remove(int index)
{
    beginRemoveRows(QModelIndex(), index, index);

    m_subjects.erase(m_subjects.begin() + index);

    endRemoveRows();
}

void SubjectsDataModel::add(int index, std::vector<std::string>&& names)
{
    if (names.empty()) {
        return;
    }

    beginInsertRows(QModelIndex(), index + 1, index + int(names.size()));

    for (auto&& name : names) {
        m_subjects.insert(m_subjects.begin() + (index + 1), ts::Subject{ .id = m_idGenerator.newId(), .name = std::move(name) });
    }

    endInsertRows();
}

std::vector<ts::Subject> SubjectsDataModel::getSubjects() const
{
    return m_subjects;
}
