#ifndef SLM_SLM_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_SLM_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <cfloat>
#include <vector>

#include <Base/Tree.h>
#include "Layer.h"
#include "Model.h"

namespace slm {
    // Forward declaration
    class Iterator;
    class GenLayerIndex;
}

namespace slm
{

class SLM_EXPORT Slm
{
public:
    Slm();
    ~Slm();

    friend class Iterator;
    friend struct GenLayerIndex;

public:
    typedef base::Tree<uint64_t, double> tree;
    typedef base::Node<uint64_t, double> node;
    //typedef boost::property_tree::ptree tree;

    // Setters and Getters for manipulating the time between layers.
    void setLayerCoolingTime(const double &t) { this->layerCoolingTime = t; }
    void setLayerAdditionTime(const double &t) { this->layerAdditionTime = t; }
    inline double getLayerCoolingTime() const { return layerCoolingTime; }
    inline double getLayerAdditionTime() const { return layerAdditionTime; }

    void setBuild(const std::vector<Layer::Ptr> &layers,
                  const std::vector<Model::Ptr> &models,
                  ScanMode mode,
                  const double lThickness,
                  const double lAdditionTime = 0.,
                  const double lCoolingTime  = 0.);
    void clear();

    inline tree getTimeIndex()  const { return this->tindex;}

    /**
     * Layer Information
     */
    inline double getLayerThickness() const { return this->layerThickness;}
    int getLayerIdByTime(const double &t) const ;
    Layer::Ptr getLayerByTime(const double &t) const;

    double getTimeByLayerId(const int layerId) const;
    double getTimeByLayerGeomId(const int layerId, const int geomId) const;

    /**
      * @param  t - Current Time (s)
      * @param  x - Laser Position on current layer
      * @param  y - Laser Position on current layer
      * @param  z - Laser Position based on current layer and it's thickness
      * @param  isLaserOn - Determines if the laser is currently on (e.g. addition of powder layer)
      */
    void getLaserPosition(const double &t, double &x, double &y, double &z,  bool &isLaserOn) const;

    /**
      * @param  t - Current Time (s)
      * @param  power   - Laser Power (W)
      * @param  expTime - Laser Exposure Time (ms)
      * @param  pntDist - Laser Point Distance (microns)
      * @param  isLaserOn - Determines if the laser is currently on (e.g. addition of powder layer)
      */
    void getLaserParameters(const double &t, float &power, int &expTime, int &pntDist, bool &isLaserOn) const;

    /**
      * @param  t - Current Time (s)
      * @param  deltaX - Current X Velocity for laser
      * @param  deltaY - Curreny Y Velocity for laser
      * @param  pntDist - Laser Point Distance
      * @param  isLaserOn - Determines if the laser is currently on (e.g. addition of powder layer)
      */
    void getLaserVelocity(const double &t, double &deltaX, double &deltaY, bool &isLaserOn) const;

    bool isLaserOnByTime(const double t) const;

    /**
     * @brief getLayerGeometryByTime
     * @param t - Current Tmime
     * @return LayerGeometry * - the current Layer Geometry at time t
     */
    LayerGeometry::Ptr getLayerGeometryByTime(const double t) const;


    // Information for build
    double getBuildTime() const;
    double getBuildEnergy() const;

    const std::vector<Layer::Ptr> & getLayers() const { return layers; }
    const std::vector<Model::Ptr> & getModels() const { return models;}

    inline ScanMode getScanMode() const { return scanmode; }
    Model::Ptr getModelById(uint id) const;

protected:
    void rebuildCache();
    void invalidateCache();
    void parseGeometry();

    void createLayerIndex();

    void getPointsInGeom(const double &offset, LayerGeometry *lgeom, QPointF &p1, QPointF &p2) const;
    QPointF getPointInGeom(const double &offset, LayerGeometry *lgeom) const;

    double calcGeomTime(LayerGeometry *lgeom) const;

protected:
    tree tindex;

    // Convenience helper function to check if time is within bounds
    bool isBoundByTimeInterval(const double t, const double sTime, const double delta) const {
        return t > sTime - DBL_EPSILON && t < sTime + delta - DBL_EPSILON;
    }

    double layerThickness;
    double layerAdditionTime; // Time taken for new layer of powder to be added
    double layerCoolingTime;  // Time after last laser scan for layer to cool

    ScanMode scanmode;

    std::vector<Layer::Ptr> layers;
    std::vector<Model::Ptr> models;

private:
    tree _cache;
};

} // end of namespace slm

#endif // SLM_SLM_H_HEADER_HAS_BEEN_INCLUDED
