#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <tuple>

#include <App/Header.h>
#include <App/Layer.h>
#include <App/Model.h>
#include <App/Reader.h>

#include <Translators/MTT/Reader.h>
#include <Translators/MTT/Writer.h>

namespace py = pybind11;

/*
 * Must construct some basic types that can be represented in python
 */

#if 0

class PySLMHeader
{
public:
    PySLMHeader() :  zUnit(0),
                     vMajor(0),
                     vMinor(0)
    {

    }

    ~PySLMHeader()
    {

    }

    // Getters
    std::tuple<quint64,quint64> getVersion() const {return  std::make_tuple(this->vMajor, this->vMinor);}
    quint64 getZUnit() const { return this->zUnit;}
    std::string getFilename() const { return this->mFilename;}

    // Setters
    void setFilename(const std::string &filename) { this->mFilename = filename;}
    void setZUnit(const quint64 unit) { this->zUnit = unit; }
    void setVersionByTuple(const std::tuple<quint64,quint64> &version) {this->vMajor = std::get<0>(version); this->vMinor = std::get<1>(version);}
    void setVersion(const quint64 majorVersion,const quint64 minorVersion) { this->vMajor = majorVersion; this->vMinor = minorVersion;}

    static PySLMHeader create(std::string filename, quint64 zunit, std::tuple<quint64,quint64> &version)
    {
        PySLMHeader header = PySLMHeader();
        header.setFilename(filename);
        header.setZUnit(zunit);
        header.setVersionByTuple(version);
        return header;
    }

protected:
    quint64 zUnit;
    quint64 vMajor;
    quint64 vMinor;
    std::string mFilename;
};

class PyBuildStyle
{
public:
    PyBuildStyle() : mBid(0),
                     mPointDistance(0),
                     mPointExposureTime(0),
                     mLaserPower(0.0),
                     mLaserSpeed(0.0),
                     mLaserFocus(0)
    {
    }

    ~PyBuildStyle(){}

    // Setters
    void setBid(const quint64 id) {this->mBid = id;}

    void setStyle(quint64 bid,
                  quint64 focus,
                  float power,
                  quint64 pExpTime,
                  quint64 pDistTime,
                  float speed = 0)
    {
        // Convenience function
        this->mBid = bid;
        this->mLaserFocus = focus;
        this->mLaserPower = power;
        this->mLaserSpeed = speed;
        this->mPointExposureTime = pExpTime;
        this->mPointDistance = pDistTime;
    }

    void setLaserFocus(const quint64 &focus) {this->mLaserFocus = focus;}
    void setPointExposureTime(quint64 pointExpTime)  { this->mPointExposureTime = pointExpTime;}
    void setPointDistance(quint64 pointDistance)  { this->mPointDistance = pointDistance;}
    void setLaserPower(float power)  { this->mLaserPower = power;}
    void setLaserSpeed(float speed)  { this->mLaserSpeed = speed;}

    // Getters
    quint16 bid() const { return this->mBid;}
    quint64 laserFocus() const { return this->mLaserFocus;}
    quint64 pointExposureTime() const { return this->mPointExposureTime;}
    quint64 pointDistance() const { return this->mPointDistance;}
    float laserPower() const { return this->mLaserPower;}
    float laserSpeed() const { return this->mLaserSpeed;}

protected:
    quint16 mBid;
    quint64 mLaserFocus;
    quint64 mPointExposureTime;
    quint64 mPointDistance;
    float mLaserPower;
    float mLaserSpeed;

};

class PyModel
{
public:
    PyModel() : mid(0),
                mTopLayerId(0)
    {
    }

    PyModel(quint64 mid, quint64 topLayerId) : mid(mid), mTopLayerId(topLayerId)
    {
    }

    ~PyModel(){}

    // Setters
    void setMid(const quint64 &id) {this->mid = id;}
    void setTopLayerId(const quint64 &id) {this->mTopLayerId = id;}
    void setBuildStyles(std::vector<PyBuildStyle *> bstyles) { this->mBuildStyles =  bstyles; }

    // Getters
    quint64 topLayerId() const { return this->mTopLayerId;}
    quint64 getMid() const { return this->mid;}

    std::vector<PyBuildStyle *> getBuildStyles() const { return this->mBuildStyles; }

    static PyModel create(quint64 mid, quint64 topLayerId) { return PyModel(mid, topLayerId); }

protected:
    quint64 mTopLayerId;
    quint64 mid;

    std::vector<PyBuildStyle *> mBuildStyles;

};

class PyLayerGeometry
{
public:
    PyLayerGeometry() : mid(0), bid(0) {
    }

    ~PyLayerGeometry(){}

    // Setters
    void setBid(const uint &id) {this->bid = id;}
    void setMid(const uint &id) {this->mid = id;}

    void setType(const std::string &type_) { geomType = type_; }
    void setCoords(py::array_t<float> pnts)
    {
        auto r =  pnts.unchecked();

        this->coords = QVector<QPointF>(r.shape(1)); // Reserve some points
        for (ssize_t i = 0; i < r.shape(0); i++){
            this->coords[i].rx() = r(i,0);
            this->coords[i].ry() = r(i,1);
        }

    }

    py::array_t<float> getPyCoords() const
    {

        py::array_t<float> pnts;
        pnts.resize({this->coords.length(), 2});

        auto r =  pnts.mutable_unchecked();

        int i = 0;
        for(QVector<QPointF>::const_iterator it = this->coords.cbegin(); it != this->coords.cend(); it++, i++) {
            r(i,0) = it->x(); r(i,1) = it->y();
        }

        return pnts;
    }

    const QVector<QPointF> & getCoords() const { return this->coords; }


    // Getters
    quint16 getBid() const { return this->bid;}
    quint16 getMid() const { return this->mid;}
    const std::string &getType() const { return geomType; }

protected:
    std::string geomType;
    quint16 bid;
    quint16 mid;
    QVector<QPointF> coords;

};

class PyLayer
{
public:
    PyLayer() {

    }

    ~PyLayer() {

    }

    // Getters
    quint64 layerId() const { return this->lid;}
    quint64 getZ() const { return this->z;}
    std::vector<PyLayerGeometry *> getLayerGeometry() const { return this->layerGeometry; }

    // Setters
    void setLayerGeometry(std::vector<PyLayerGeometry *> geoms) { this->layerGeometry =  geoms; }

    void setLayerId(quint64 id) { this->lid = id;}
    void setZ(quint64 val) { z = val; }

protected:
    quint64 lid;
    quint64 z;
    std::vector<PyLayerGeometry *> layerGeometry;

};

void exportMTT(PySLMHeader *pyHeader, const std::vector<PyModel *> & models, const std::vector<PyLayer *> &layers)
{
    /*
     * Convert the python structures into equivalent structures
     */

    QVector<MTT::Model *> slmModels;
    QVector<MTT::Layer *> slmLayers;

    for(PyModel *model : models) {

        MTT::Model *slmModel = new MTT::Model();
        slmModel->setId(model->getMid());
        slmModel->setTopSlice(model->topLayerId());


        for(PyBuildStyle *bstyle : model->getBuildStyles()) {
            MTT::BuildStyle *slmBuildStyle = new MTT::BuildStyle();
            slmBuildStyle->id.setValue(bstyle->bid());
            slmBuildStyle->laserFocus.setValue(bstyle->laserFocus());
            slmBuildStyle->laserPower.setValue(bstyle->laserPower());
            slmBuildStyle->laserSpeed.setValue(bstyle->laserSpeed());
            slmBuildStyle->pointDistance.setValue(bstyle->pointDistance());
            slmBuildStyle->pointExposureTime.setValue(bstyle->pointExposureTime());

            slmModel->addBuildStyle(slmBuildStyle);
        }

        slmModels.append(slmModel);

    }

    for(PyLayer *layer : layers) {

        MTT::Layer *slmLayer = new MTT::Layer();
        slmLayer->setLayerId(layer->layerId());
        slmLayer->setZ(layer->getZ());

        for(PyLayerGeometry *geom : layer->getLayerGeometry()) {
            MTT::LayerGeometry *lgeom = nullptr;
            MTT::LayerGeometry::TYPE geomType;
            if(strcmp(geom->getType().c_str(), "contour") == 0) {
                lgeom = new MTT::ContourGeometry(); geomType = MTT::LayerGeometry::POLYGON;
            } else if(strcmp(geom->getType().c_str(), "hatch") == 0) {
                lgeom = new MTT::HatchGeometry(); geomType = MTT::LayerGeometry::HATCH;
            } else if(strcmp(geom->getType().c_str(), "point") == 0) {
                lgeom = new MTT::PntsGeometry(); geomType = MTT::LayerGeometry::PNTS;
            } else {
                continue;
            }

            lgeom->coords = geom->getCoords();
            lgeom->bid.setValue(geom->getBid());
            lgeom->mid.setValue(geom->getMid());

            switch(geomType) {
                case MTT::LayerGeometry::POLYGON: slmLayer->addContourGeometry(lgeom); break;
                case MTT::LayerGeometry::HATCH: slmLayer->addHatchGeometry(lgeom);  break;
                case MTT::LayerGeometry::PNTS: slmLayer->addPntsGeometry(lgeom); break;
            }

        }

        slmLayers.append(slmLayer);

    }

    /*
     *  Process the layers
     */

    // Parse layers into data structure
    MTT::Header header;

    header.fileName = pyHeader->getFilename().c_str();
    header.zUnit    = pyHeader->getZUnit();
    header.vMajor   = std::get<0>(pyHeader->getVersion());
    header.vMinor   = std::get<1>(pyHeader->getVersion());


    MTT::Writer writer(header.fileName);


    writer.write(header, slmModels, slmLayers);

    slmModels.clear();
    slmLayers.clear();

}

#endif

using namespace slm;

PYBIND11_MODULE(slm, m) {

    m.doc() = R"pbdoc(
        PySLM Support Module
        -----------------------
        .. currentmodule:: slm
        .. autosummary::
           :toctree: _generate

    )pbdoc";

    py::class_<slm::LayerGeometry> layerGeomPyType(m, "LayerGeometry");

    layerGeomPyType.def(py::init())
        .def_readwrite("bid", &LayerGeometry::bid)
        .def_readwrite("mid", &LayerGeometry::mid)
        .def_readwrite("coords", &LayerGeometry::coords)
        .def_property("type", &LayerGeometry::getType, nullptr);

    py::class_<slm::ContourGeometry, slm::LayerGeometry>(m, "ContourGeometry")
        .def_property("type", &ContourGeometry::getType, nullptr);

    py::class_<slm::HatchGeometry, slm::LayerGeometry>(m, "HatchGeometry")
         .def_property("type", &HatchGeometry::getType, nullptr);

    py::class_<slm::PntsGeometry, slm::LayerGeometry>(m, "PointsGeometry")
         .def_property("type", &PntsGeometry::getType, nullptr);

    py::enum_<slm::LayerGeometry::TYPE>(layerGeomPyType, "Kind")
        .value("Invalid", slm::LayerGeometry::TYPE::INVALID)
        .value("Pnts", slm::LayerGeometry::TYPE::PNTS)
        .value("Polygon", slm::LayerGeometry::TYPE::POLYGON)
        .value("Hatch", slm::LayerGeometry::TYPE::HATCH)
        .export_values();


    py::class_<slm::Header>(m, "Header")
        .def(py::init())
        //.def(py::init(&PySLMHeader::create))
        .def_readwrite("filename", &Header::fileName)
        .def_property("version",  &Header::version, &Header::setVersion)
        .def_readwrite("zUnit",    &Header::zUnit);

    py::class_<slm::BuildStyle>(m, "BuildStyle")
        .def(py::init())
        .def_readwrite("bid",               &BuildStyle::id)
        .def_readwrite("laserPower",        &BuildStyle::laserPower)
        .def_readwrite("laserSpeed",        &BuildStyle::laserSpeed)
        .def_readwrite("laserFocus",        &BuildStyle::laserFocus)
        .def_readwrite("pointDistance",     &BuildStyle::pointDistance)
        .def("setStyle", &BuildStyle::setStyle, "Sets the paramters of the buildstyle",
                         py::arg("bid"),
                         py::arg("focus"),
                         py::arg("power"),
                         py::arg("pointExposureTime"),
                         py::arg("pointExposureDistance"),
                         py::arg("speed") = 0);

    py::class_<slm::Model>(m, "Model")
        .def(py::init())
        .def_property("mid", &Model::getId, &Model::setId)
        .def("__len__", [](const Model &s ) { return s.getBuildStyles().size(); })
        .def_property("buildStyles", &Model::getBuildStyles, &Model::setBuildStyles)
        .def_property("topLayerId",  &Model::getTopSlice, &Model::setTopSlice)
        .def_property("name", &Model::getName, &Model::setName);

    py::enum_<slm::ScanMode>(m, "ScanMode")
        .value("None", ScanMode::NONE)
        .value("ContourFirst", ScanMode::CONTOUR_FIRST)
        .value("HatchFirst", ScanMode::HATCH_FIRST)
        .export_values();

    py::class_<slm::Layer>(m, "Layer")
        .def(py::init())
        .def(py::init<uint64_t, uint64_t>())
        .def("__len__", [](const Layer &s ) { return s.getGeometry().size(); })
        .def("getPointsGeometry", &Layer::getPntsGeometry)
        .def("getHatchGeometry", &Layer::getHatchGeometry)
        .def("getContourGeometry", &Layer::getContourGeometry)
        .def_property("z", &Layer::getZ, &Layer::setZ)
        .def_property("layerId", &Layer::getLayerId, &Layer::setLayerId)
        .def("getGeometry", &Layer::getGeometry, py::arg("scanMode") = slm::ScanMode::NONE);





#if 0
    py::class_<slm::MTT::Reader>(m, "Reader")
        .def(py::init())
        .def("setFilePath", &MTT::Reader::setFilePath, py::arg("filename"))
        .def("getFilePath", &MTT::Reader::getFilePath)
        .def("parse", &MTT::Reader::parse)
        .def("getLayerThickness", &MTT::Reader::getLayerThickness)
        .def("getZUnit", &MTT::Reader::getZUnit)
        .def_property("layers", &base::Reader::getLayers, nullptr)
        .def_property("models", &base::Reader::getModels, nullptr);

    py::class_<slm::MTT::Writer>(m, "Writer")
        .def(py::init())
        .def(py::init<std::string>())
        .def("setFilePath", &MTT::Writer::setFilePath)
        .def("getFilePath", &MTT::Writer::getFilePath)
        .def("write", &MTT::Writer::write, py::arg("header"), py::arg("models"), py::arg("layers"))
        .def_property("checksum", &MTT::Writer::getChecksum, nullptr);

#endif

/*
    m.def("exportMTT", &exportMTT, R"pbdoc(
        Exports a set of
    )pbdoc");
*/

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif

}

