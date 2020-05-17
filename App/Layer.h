#ifndef SLM_LAYER_H_HEADER_HAS_BEEN_INCLUDED
#define SLM_LAYER_H_HEADER_HAS_BEEN_INCLUDED

#include "SLM_Export.h"

#include <cstdint>
#include <vector>
#include <memory>

#include <Eigen/Dense>

namespace slm
{

enum ScanMode {
    NONE          = 0,
    CONTOUR_FIRST = 1,
    HATCH_FIRST   = 2
};

/**
 * @brief The LayerGeometry base class describes geometry information for each layer
 */
class SLM_EXPORT LayerGeometry
{
public:

    typedef std::shared_ptr<LayerGeometry> Ptr;
    LayerGeometry(uint32_t modelId, uint32_t buildStyleId );
    LayerGeometry();
    virtual ~LayerGeometry();

public:
    enum TYPE {
        INVALID = 0,
        POLYGON = 1,
        HATCH   = 2,
        PNTS    = 3
    };

    Eigen::MatrixXf coords;

#if 0
    uint32_t mid() const { return mid; }
    uint32_t bid() const { return bid; }
#endif

protected:
    uint32_t modelId = 0;
    uint32_t buildId = 0;

public:
    uint32_t mid = 0;
    uint32_t bid = 0;
    //  Type may only be set upon initialisation
    virtual TYPE getType() const { return type; }

public:
    const static TYPE type = INVALID;
};

template <LayerGeometry::TYPE T>
class SLM_EXPORT LayerGeometryT : public LayerGeometry
{

public:

    typedef std::shared_ptr<LayerGeometryT> Ptr;

    LayerGeometryT() {}
    LayerGeometryT(uint32_t modelId, uint32_t buildStyleId) : LayerGeometry(modelId, buildStyleId) {}
    ~LayerGeometryT() {}

public:
    const static TYPE type = T;
    virtual TYPE getType() const { return type; }
};
#if 0

class SLM_EXPORT ContourGeometry : public LayerGeometry
{
public:
    ContourGeometry() {}
    ~ContourGeometry() {}

public:
    const static TYPE type = POLYGON;
    virtual TYPE getType() const { return type; }
};

class SLM_EXPORT HatchGeometry : public LayerGeometry
{
public:
    HatchGeometry() {}
    ~HatchGeometry() {}

public:
    const static TYPE type = HATCH;
    virtual TYPE getType() const { return type; }
};




class SLM_EXPORT PntsGeometry : public LayerGeometry
{
public:
    PntsGeometry() {}
    ~PntsGeometry() {}

public:
    const static TYPE type = PNTS;
    virtual TYPE getType() const { return type; }
};

#endif

class SLM_EXPORT Layer
{
public:

    typedef std::shared_ptr<Layer> Ptr;

    Layer();
    Layer(uint64_t id, uint64_t zVal);
    ~Layer();

public:

    void clear();

    /**
     * Setters
     */
    void setLayerId(uint64_t id);
    void setZ(uint64_t val);

    // Add different types of geometry to each layer
    template <class T>
    int64_t addGeometry(typename T::Ptr geom) {
        if(!geom)
            return -1;

        geometry.push_back(geom);

        return geometry.size();
    }

    int64_t addContourGeometry(LayerGeometry::Ptr geom);
    int64_t addHatchGeometry(LayerGeometry::Ptr geom);
    int64_t addPntsGeometry(LayerGeometry::Ptr geom);

    void setGeometry(const std::vector<LayerGeometry::Ptr> &geoms);

    template <class T>
    std::vector<LayerGeometry::Ptr> getGeometryByType () {
        std::vector<LayerGeometry::Ptr> geoms;
        for(LayerGeometry::Ptr geom : geometry) {
            if(geom->getType() == T::type){geoms.push_back(geom);}
        }
        return geoms;
    }

    std::vector<LayerGeometry::Ptr> getGeometry(ScanMode mode = NONE) const;
    std::vector<LayerGeometry::Ptr> getContourGeometry() const;
    std::vector<LayerGeometry::Ptr> getHatchGeometry() const;
    std::vector<LayerGeometry::Ptr> getPntsGeometry() const;

    /**
     * Getters
     */
    uint64_t getZ()       const { return z; }
    uint64_t getLayerId() const { return lid; }

protected:
    uint64_t lid = 0;    // Layer ID
    uint64_t z = 0;      // Z Layer Position
    std::vector<LayerGeometry::Ptr> geometry;
};

using HatchGeometry   = slm::LayerGeometryT<LayerGeometry::HATCH>;
using ContourGeometry = slm::LayerGeometryT<LayerGeometry::POLYGON>;
using PntsGeometry    = slm::LayerGeometryT<LayerGeometry::PNTS>;

} // End of Namespace slm


#endif // SLM_LAYER_H_HEADER_HAS_BEEN_INCLUDED
