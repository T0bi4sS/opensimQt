#ifndef VSMARKERTASKSMODEL_H
#define VSMARKERTASKSMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <OpenSim.h>
#include <QItemSelectionModel>


class vsMarkerTasksModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    vsMarkerTasksModel();

    void updateTasks(OpenSim::Model *model);
    void enableAllSelected();
    void disableAllSelected();

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool getAllEnabled() const;
    bool getAllDisabled() const;

public slots:

    void selectionModelChanged(QModelIndexList selected);
    void updateIKUI();

signals :
    void uiUpdated();

private:
    QList<OpenSim::IKMarkerTask*> m_ikMarkerTasks;
    QList<OpenSim::IKMarkerTask*> m_selectedTasks;
    bool m_allEnabled = false;
    bool m_allDisabled = false;

};

#endif // VSMARKERTASKSMODEL_H
