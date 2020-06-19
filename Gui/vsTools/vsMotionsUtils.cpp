/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2020 by VisSim                                          *
 *                                                                         *
 *   Authors: Ibraheem Aldhamari, Yasser Grimes                            *
 *                                                                         *
 ***************************************************************************/
#include "vsMotionsUtils.h"

#include <QBitArray>
#include <QDebug>
#include <QFileDialog>
#include "vsOpenSimTools.h"
#include <vsModeling/vsNavigatorModel.h>
#include <vsModeling/vsOneMotionNode.h>

vsMotionsUtils* vsMotionsUtils::instance = nullptr;

vsMotionsUtils::vsMotionsUtils(QObject *parent) : QObject(parent)
{
    //handle the changes in this class too
    connect(this,&vsMotionsUtils::notifyObservers,this,&vsMotionsUtils::update);
}

vsMotionsUtils *vsMotionsUtils::getInstance()
{
    if(!instance) instance = new vsMotionsUtils();
    return instance;
}

void vsMotionsUtils::openLoadMotionDialog(OpenSim::Model *model)
{
    try {
        QString motionFile = QFileDialog::getOpenFileName(nullptr,"Load Motion File To Current Model","","Motions (*.mot *.sto)");
        if(motionFile == "") throw  QString("error");

        OpenSim::Storage *storage = new OpenSim::Storage(motionFile.toStdString());

        activeModel = model;

        loadMotionStorage(storage,true,motionFile.toStdString());
        vsOpenSimTools::tools->log("motion file selected :"+motionFile,"vsModelNode");
    }
    catch (QString s) {
        vsOpenSimTools::tools->log("no file was selected :","vsModelNode",vsOpenSimTools::Error);
    }
    catch(...){
        vsOpenSimTools::tools->log("loading motion file failed :","vsModelNode",vsOpenSimTools::Error);
    }
}

void vsMotionsUtils::loadMotionFile(std::string fileName)
{

}

void vsMotionsUtils::loadMotionStorage(OpenSim::Storage *newMotion, bool primary, std::string filePath)
{
    if(!activeModel) return;
    if(testMotionAssociationPossible(activeModel,newMotion)){
        addMotion(activeModel,newMotion,nullptr);
        vsOpenSimTools::tools->log("Motion : "+QString::fromStdString(newMotion->getName())+
                                   " Loaded to model : "+QString::fromStdString(activeModel->getName()),
                                   "vsMotionUtils",vsOpenSimTools::Success);
    }
    else{
        vsOpenSimTools::tools->log("Could not apply motion to active model","vsMotionUtils",vsOpenSimTools::Error);
    }
}

bool vsMotionsUtils::testMotionAssociationPossible(OpenSim::Model *model, OpenSim::Storage *storage)
{
    auto modelCS = model->getCoordinateSet();
    // Keep track of how many columns correspond to Coords or Markers
    int numberUsedColumnes = 0;
    for (int i = 0; i < modelCS.getSize(); ++i) {
        if(storage->getStateIndex(modelCS.get(i).getName()) != -1 ||
                storage->getStateIndex(modelCS.get(i).getStateVariableNames().get(0)) != -1){
            numberUsedColumnes ++;
            return true;
        }
    }

    OpenSim::Array<std::string> markersNames;
    model->getMarkerSet().getNames(markersNames);

    for(int i=0; i<markersNames.getSize(); i++){
        if ((storage->getStateIndex(markersNames.get(i)+"_tx")!=-1) ||
                (storage->getStateIndex(markersNames.get(i)+"_Tx")!=-1) ||
                (storage->getStateIndex(markersNames.get(i)+"_TX")!=-1))
            numberUsedColumnes++;
    }
    return (numberUsedColumnes >= 1);
}

void vsMotionsUtils::addMotion(OpenSim::Model *model, OpenSim::Storage *newMotion, OpenSim::Storage *parentMotion)
{
    //test for the Model For Experement Data
    bool convertAngels = newMotion->isInDegrees();
    if(convertAngels) model->getSimbodyEngine().convertDegreesToRadians(*newMotion);

    auto modelMotions = mapModelsToMotions.value(model,nullptr);
    if(!modelMotions){
        modelMotions = new QList<OpenSim::Storage*>();
        mapModelsToMotions.insert(model,modelMotions);
    }

    modelMotions->append(newMotion);
    //TODO change 4 to bitnumbers
    //mapMotionToBitArray.insert(newMotion,new QBitArray(4));
    MotionEventObject evntObj(model,newMotion,parentMotion?MotionOperation::Assoc:MotionOperation::Open);
    emit notifyObservers(evntObj);

    if(parentMotion == nullptr) setCurrentMotion(model,newMotion);
}

void vsMotionsUtils::setCurrentMotion(OpenSim::Model *model, OpenSim::Storage *motion)
{
    //TODO add multiple motions support
    currentMotion= new QPair<OpenSim::Model*,OpenSim::Storage*>(model,motion);
    applyTimeToModel(model,motion,motion->getFirstTime());
    MotionEventObject evntObj(model,motion,MotionOperation::CurrentMotionsChanged);
    emit notifyObservers(evntObj);
    emit currentMotionChanged();
}

void vsMotionsUtils::applyTimeToModel(OpenSim::Model *model, OpenSim::Storage *motion,double time)
{
    SimTK::Vector stateData(motion->getColumnLabels().getSize());
    //double ** stateData;
    //the minus one is to exclude the time column
    motion->getDataAtTime(time,motion->getColumnLabels().getSize(),stateData);
    //auto stateVector = motion->getStateVector(time);
    //motion->get
    model->setStateVariableValues(model->updWorkingState(),stateData);
    //qDebug() << "the state of the model is updated" << motion->getColumnLabels().getSize();
    //TODO update the decorations instead of removing
    //model->realizeDynamics(model->updWorkingState());
    vsNavigatorNode::visualizerVTK->removeModelActors(model);
    vsNavigatorNode::visualizerVTK->addOpenSimModel(model);
}

void vsMotionsUtils::update(MotionEventObject eventObj)
{
    //TODO support other events
    qDebug()<< "motion is being updated";

    //motion event
    switch (eventObj.m_motionOperation) {
    case MotionOperation::Open:{
        vsNavigatorNode *modelNode = vsOpenSimTools::tools->getObjectNode(eventObj.m_model);
        vsNavigatorNode *motionsNode = modelNode->findChildNode("Motions");
        if(motionsNode == nullptr){
            motionsNode = new vsNavigatorNode(nullptr,"Motions",modelNode,modelNode);
            motionsNode->iconPath = ":/Data/Images/Nodes/motionsNode.png";
        }

        //TODO add suppoet for experemental data model
        vsOneMotionNode *newMotionNode = new vsOneMotionNode(eventObj.m_storage,motionsNode,motionsNode);
        qDebug()<< "Motion Name : " << QString::fromStdString(eventObj.m_storage->getName());
        emit vsOpenSimTools::tools->getNavigatorModel()->layoutChanged();
    }break;

    default:
        break;
    }

}
