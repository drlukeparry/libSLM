#pragma once

#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <pybind11/complex.h>
#include <pybind11/eigen.h>


#include <algorithm>


namespace py = pybind11;

namespace slm {

template <typename Vector, typename holder_type = std::unique_ptr<Vector>>
py::class_<Vector, holder_type> bind_my_vector(py::handle scope, std::string const &name) {

    using Class_ = py::class_<Vector, holder_type>;

    // If the value_type is unregistered (e.g. a converting type) or is itself registered
    // module-local then make the vector binding module-local as well:
    using vtype = typename Vector::value_type;
    using SizeType = typename Vector::size_type;
    using DiffType = typename Vector::difference_type;

    auto wrap_i = [](DiffType i, SizeType n) {
        if (i < 0)
            i += n;
        if (i < 0 || (SizeType)i >= n)
            throw py::index_error();
        return i;
    };

    Class_ cl(scope, name.c_str());

    cl.def(py::init<>())
      .def("clear", &Vector::clear)
      .def("append", (void (Vector::*)(const vtype &)) &Vector::push_back,  py::keep_alive<1, 2>());

    cl.def("pop_back", &Vector::pop_back)
      .def("__len__", &Vector::size)
      .def("__bool__", &Vector::empty);


    // Iteration methods
    cl.def("__getitem__",
        [](const Vector &v, DiffType i) -> vtype {
            if (i < 0 && (i += v.size()) < 0)
                throw py::index_error();
            if ((SizeType)i >= v.size())
                throw py::index_error();
            return v[(SizeType)i];
        }
    );

    cl.def("__iter__", [](Vector &v) {
       return py::make_iterator(v.begin(), v.end());
    }, py::keep_alive<0, 1>()); /* Keep vector alive while iterator is used */

    cl.def("__contains__",
            [](const Vector &v, const vtype &x) {
                return std::find(v.begin(), v.end(), x) != v.end();
            },
            py::arg("x"),
            "Return true the container contains ``x``"
    );


    /// Slicing protocol
    cl.def("__getitem__",
        [](const Vector &v, py::slice slice) -> Vector * {
            size_t start, stop, step, slicelength;

            if (!slice.compute(v.size(), &start, &stop, &step, &slicelength))
                throw py::error_already_set();

            Vector *seq = new Vector();
            seq->reserve((size_t) slicelength);

            for (size_t i=0; i<slicelength; ++i) {
                seq->push_back(v[start]);
                start += step;
            }
            return seq;
        },
        py::arg("s"),
        "Retrieve list elements using a slice object"
    );

    // Modifiers

    cl.def("__setitem__",
        [wrap_i](Vector &v, DiffType i, const vtype &t) {
            i = wrap_i(i, v.size());
            v[(SizeType)i] = t;
        }, py::keep_alive<1, 2>()
    );


    cl.def("__setitem__",
        [](Vector &v, py::slice slice,  const Vector &value) {
            size_t start, stop, step, slicelength;
            if (!slice.compute(v.size(), &start, &stop, &step, &slicelength))
                throw py::error_already_set();

            if (slicelength != value.size())
                throw std::runtime_error("Left and right hand size of slice assignment have different sizes!");

            for (size_t i=0; i<slicelength; ++i) {
                v[start] = value[i];
                start += step;
            }
        },
        "Assign list elements using a slice object",
         py::keep_alive<1, 2>()
    );

    cl.def("__delitem__",
        [wrap_i](Vector &v, DiffType i) {
            i = wrap_i(i, v.size());
            v.erase(v.begin() + i);
        },
        "Delete the list elements at index ``i``"
    );

    cl.def("__delitem__",
        [](Vector &v, py::slice slice) {
            size_t start, stop, step, slicelength;

            if (!slice.compute(v.size(), &start, &stop, &step, &slicelength))
                throw py::error_already_set();

            if (step == 1 && false) {
                v.erase(v.begin() + (DiffType) start, v.begin() + DiffType(start + slicelength));
            } else {
                for (size_t i = 0; i < slicelength; ++i) {
                    v.erase(v.begin() + DiffType(start));
                    start += step - 1;
                }
            }
        },
        "Delete list elements using a slice object"
    );

    return cl;
}


}
