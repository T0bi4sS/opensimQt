#include "vsModelNode.h"
#include "vsNavigatorModel.h"
#include <QDebug>
#include <QIcon>
vsNavigatorModel::vsNavigatorModel()
{
    m_rootNNode = new vsNavigatorNode(nullptr,"the root",nullptr,this);
}

void vsNavigatorModel::loadOpenSimModel(OpenSim::Model *openSimModel)
{
    m_activeModel = openSimModel;
    SimTK::State *stat = &m_activeModel->initSystem();
    //loading the model it self
    m_openModels.append(openSimModel);
    //m_rootOpenSimModel->initSystem();
    //m_rootOpenSimModel.getn

    vsModelNode *modelNNode = new vsModelNode(m_activeModel,m_rootNNode,this);


    emit layoutChanged();
//    m_activeModel->setUseVisualizer(true);
//    m_activeModel->updMatterSubsystem().setShowDefaultGeometry(true);
//    m_visualiser = &openSimModel->updVisualizer().updSimbodyVisualizer();
//    m_visualiser->setBackgroundType(m_visualiser->SolidColor);
//    m_visualiser->setBackgroundColor(SimTK::White);

    //OpenSim::simulate(*m_activeModel,*stat,10.0);

}

QModelIndex vsNavigatorModel::index(int row, int column, const QModelIndex &parent) const
{
    if(hasIndex(row,column,parent))
    {
        vsNavigatorNode *parentNode = nodeForIndex(parent);
        vsNavigatorNode *childNode = parentNode->childNodes.at(row);
        return createIndex(row,column,childNode);
    }
    return QModelIndex();
}

QModelIndex vsNavigatorModel::parent(const QModelIndex &child) const
{
    vsNavigatorNode *childNode = nodeForIndex(child);
    vsNavigatorNode *parentNode = childNode->parentNode;
    if(parentNode ==  m_rootNNode)
        return QModelIndex();
    int row = rowForNode(parentNode);
    int column = 0;
    return createIndex(row,column,parentNode);

}

int vsNavigatorModel::rowCount(const QModelIndex &parent) const
{
    vsNavigatorNode *parentNode = nodeForIndex(parent);
    return parentNode->childNodes.count();
}

int vsNavigatorModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant vsNavigatorModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role==Qt::DisplayRole)
    {
        vsNavigatorNode *nNode = nodeForIndex(index);
        return  nNode->displayName;
    }
    if (index.isValid() && role==Qt::DecorationRole) {
        vsNavigatorNode *nNode = nodeForIndex(index);
        return QVariant::fromValue(QPixmap(nNode->iconPath));
    }
    return QVariant();

}

QModelIndex vsNavigatorModel::indexForNNode(vsNavigatorNode *nNode)
{
    if(nNode == m_rootNNode)
        return QModelIndex();
    int row = rowForNode(nNode);
    int column = 0;
    return createIndex(row,column,nNode);
}

vsNavigatorNode* vsNavigatorModel::nodeForIndex(const QModelIndex &index) const
{
    if(index.isValid())
        return static_cast<vsNavigatorNode*>(index.internalPointer());
    return m_rootNNode;

}

int vsNavigatorModel::rowForNode(vsNavigatorNode *node) const
{
    return node->parentNode->childNodes.indexOf(node);
}
