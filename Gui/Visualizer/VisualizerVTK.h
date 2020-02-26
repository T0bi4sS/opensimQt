#ifndef VTKVISUALIZER_H
#define VTKVISUALIZER_H

#include <QVTKOpenGLWidget.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <QVector3D>
#include <QVector4D>
#include <OpenSim.h>

enum class BackgroundType{
    Solid,GroundAndSky
};

class VisualizerVTK : public QVTKOpenGLWidget
{
    Q_OBJECT
public:
    VisualizerVTK(QWidget *parent = nullptr);
    void renderingTest();
    vtkSmartPointer<vtkActor> renderGeometry(OpenSim::Geometry *geometry);
    vtkSmartPointer<vtkActor> addBox();
    vtkSmartPointer<vtkActor> addGround();
    vtkSmartPointer<vtkActor> addSkyBox();
    void addOpenSimModel(OpenSim::Model *model);

    BackgroundType backgroundType() const;
    void setBackgroundType(const BackgroundType &backgroundType);

private:
    BackgroundType m_backgroundType;


};

#endif // VTKVISUALIZER_H
