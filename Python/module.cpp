#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

#include <tuple>

#include <App/Header.h>
#include <App/Layer.h>
#include <App/Model.h>
#include <App/Reader.h>

#include <Translators/MTT/Reader.h>
#include <Translators/MTT/Writer.h>

namespace py = pybind11;

using namespace slm;

PYBIND11_MODULE(slm, m) {

    m.doc() = R"pbdoc(
        PySLM Support Module
        -----------------------
        .. currentmodule:: slm
        .. autosummary::
           :toctree: _generate

    )pbdoc";

    py::class_<slm::LayerGeometry, std::shared_ptr<slm::LayerGeometry>> layerGeomPyType(m, "LayerGeometry", py::dynamic_attr());

    layerGeomPyType.def(py::init())
        .def_readwrite("bid", &LayerGeometry::bid)
        .def_readwrite("mid", &LayerGeometry::mid)
        .def_readwrite("coords", &LayerGeometry::coords)
        .def_property("type", &LayerGeometry::getType, nullptr);

    py::class_<slm::ContourGeometry, slm::LayerGeometry, std::shared_ptr<slm::ContourGeometry>>(m, "ContourGeometry", py::dynamic_attr())
        .def(py::init())
        .def(py::init<uint32_t, uint32_t>(), py::arg("mid"), py::arg("bid"))
        .def_property("type", &ContourGeometry::getType, nullptr);

    py::class_<slm::HatchGeometry, slm::LayerGeometry, std::shared_ptr<HatchGeometry>>(m, "HatchGeometry", py::dynamic_attr())
         .def(py::init())
         .def(py::init<uint32_t, uint32_t>(), py::arg("mid"), py::arg("bid"))
         .def_property("type", &HatchGeometry::getType, nullptr);

    py::class_<slm::PntsGeometry, slm::LayerGeometry, std::shared_ptr<slm::PntsGeometry>>(m, "PointsGeometry", py::dynamic_attr())
         .def(py::init())
         .def(py::init<uint32_t, uint32_t>(), py::arg("mid"), py::arg("bid"))
         .def_property("type", &PntsGeometry::getType, nullptr);

    py::enum_<slm::LayerGeometry::TYPE>(layerGeomPyType, "LayerGeometryType")
        .value("Invalid", slm::LayerGeometry::TYPE::INVALID)
        .value("Pnts", slm::LayerGeometry::TYPE::PNTS)
        .value("Polygon", slm::LayerGeometry::TYPE::POLYGON)
        .value("Hatch", slm::LayerGeometry::TYPE::HATCH)
        .export_values();


    py::class_<slm::Header>(m, "Header", py::dynamic_attr())
        .def(py::init())
        //.def(py::init(&PySLMHeader::create))
        .def_readwrite("filename", &Header::fileName)
        .def_property("version",  &Header::version, &Header::setVersion)
        .def_readwrite("zUnit",    &Header::zUnit);

    py::class_<slm::BuildStyle, std::shared_ptr<slm::BuildStyle>>(m, "BuildStyle", py::dynamic_attr())
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

    py::class_<slm::Model, std::shared_ptr<slm::Model>>(m, "Model", py::dynamic_attr())
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

    py::class_<slm::Layer, std::shared_ptr<slm::Layer>>(m, "Layer", py::dynamic_attr())
        .def(py::init())
        .def(py::init<uint64_t, uint64_t>(), py::arg("id"), py::arg("z"))
        .def("__len__", [](const Layer &s ) { return s.geometry().size(); })
        .def("getPointsGeometry", &Layer::getPntsGeometry)
        .def("getHatchGeometry", &Layer::getHatchGeometry)
        .def("getContourGeometry", &Layer::getContourGeometry)
        .def("appendGeometry", &Layer::appendGeometry)
        .def_property("geometry", &Layer::geometry, &Layer::setGeometry)
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

