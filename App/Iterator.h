#ifndef SLM_ITERATOR_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_ITERATOR_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <Eigen/Dense>

#include "Layer.h"
#include "Slm.h"

namespace slm
{

class Layer;

// Current state of laser power at time t
struct State
{
    bool    laserOn;
    float   power;            // Laser Power (W)
    int     pntExposureTime;  // Point Exposure Time (ms)
    int     pntDistance;      // Point Distance (microm)
    inline float   laserSpeed() { return (float) pntDistance * 1. / (float) pntExposureTime; }
    Eigen::Vector2f position;         // s = (x,y)'
    Eigen::Vector2f velocity;         // v = (u,v)' // Note w is zero
    uint    layer;
    double  time;                // Current time
};

struct LaserScan
{
    Eigen::Vector2f  start;
    Eigen::Vector2f  end;
    double  tStart;
    double  tEnd;
    uint    layer;
    LayerGeometry::TYPE type;
};

class SLM_EXPORT Iterator
{
public:
    typedef Slm::node node;
    Iterator(Slm *val);
    Iterator();
    ~Iterator();

    void setTimeIncrement(double val) { _timeInc = val; }

public:
    void  seek(const double &time);
    void  seekLayer(const int &layerNum);

    Iterator& operator++();       // Prefix increment operator.
    Iterator operator++(int);     // Postfix increment operator.

    bool  more() const;
    void  next();
    State value() const;

    int getCurrentLayerNumber() const;
    Layer::Ptr getCurrentLayer() const;

    // Add operator ++ to increment automtically

protected:
    Slm::Ptr obj; // TODO make a smart pointer
    double _inc;       // Current increment
    double _endTime;
    double _timeInc;   // The finite time difference to iterate with
    int _layerInc;     // Layer increment
    int _layerGeomInc; // Layer Geometry Increment for current layer
};


class SLM_EXPORT LayerIterator: public Iterator
{
public:
    LayerIterator(Slm *val, int layerId);
    ~LayerIterator();

private:
    double _layerTime;
};


class SLM_EXPORT LayerGeomIterator : public Iterator
{
public:
    LayerGeomIterator(Slm::Ptr val);
    ~LayerGeomIterator();

public:
    void seek(const double &time);
    void seekLayer(const int &layerNum);
    int getCurrentLayerNumber() const;
    Layer::Ptr getCurrentLayer() const;

    double getCurrentTime() const;

    LayerGeomIterator& operator++();       // Prefix increment operator.
    LayerGeomIterator operator++(int);     // Postfix increment operator.

    LayerGeometry getLayerGeometry() const;

    bool more() const;
    void next();
    LayerGeometry value() const;

private:
    std::vector<Layer::Ptr>::const_iterator _layerIt; // Current Layer Iterator
    std::vector<LayerGeometry::Ptr> _layerGeomCache;
    std::vector<Layer::Ptr> _layerCache;
    std::vector<LayerGeometry>::const_iterator _layerGeomIt; // Current Layer Geometry Iterator
};

class SLM_EXPORT LaserScanIterator : public LayerGeomIterator
{
public:
    LaserScanIterator(Slm *val);
    ~LaserScanIterator();

public:

    LaserScanIterator& operator++();       // Prefix increment operator.
    LaserScanIterator operator++(int);     // Postfix increment operator.

    void next();
    double getCurrentTime() const;

    bool more() const;
    LaserScan value() const;

private:
    double calcScanTime() const;
    double _layerGeomTime,
           _relTime; // Relative time

    LayerGeometry _curLayerGeom;

    QVector<QPointF>::const_iterator _pntIt; // Laser Point Iterator
};

} // End of namespace SLM
#endif // SLM_ITERATOR_H_HEADER_HAS_BEEN_INCLUDED
