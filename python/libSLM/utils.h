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
      .def(py::init([](py::iterable it) {
                auto v = std::unique_ptr<Vector>(new Vector());
                v->reserve(len_hint(it));
                for (py::handle h : it)
                   v->push_back(h.cast<vtype>());
                return v.release();
            }))
      .def(py::init<const Vector &>(), "Copy constructor");

    /* TODO - check if above requires py::keep_alive */

    cl.def(py::self == py::self)
      .def(py::self != py::self);

    cl.def("clear", &Vector::clear)
      .def("append", (void (Vector::*)(const vtype &)) &Vector::push_back,  py::keep_alive<1, 2>());

    cl.def("pop_back", &Vector::pop_back)
      .def("__len__", &Vector::size)
      .def("__bool__", &Vector::empty);

    cl.def("count",
        [](const Vector &v, const vtype &x) {
            return std::count(v.begin(), v.end(), x);
        },
        py::arg("x"),
        "Return the number of times ``x`` appears in the list"
    );

    cl.def("remove", [](Vector &v, const vtype &x) {
                auto p = std::find(v.begin(), v.end(), x);
                if (p != v.end())
                    v.erase(p);
                else
                    throw py::value_error();
            },
            py::arg("x"),
            "Remove the first item from the list whose value is x. "
            "It is an error if there is no such item."
        );


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


    cl.def("extend",
       [](Vector &v, const Vector &src) {
           v.insert(v.end(), src.begin(), src.end());
       },
       py::arg("L"),
       "Extend the list by appending all the items in the given list"
    );

    cl.def("extend",
       [](Vector &v, py::iterable it) {
           const size_t old_size = v.size();
           v.reserve(old_size + len_hint(it));
           try {
               for (py::handle h : it) {
                   v.push_back(h.cast<vtype>());
               }
           } catch (const py::cast_error &) {
               v.erase(v.begin() + static_cast<typename Vector::difference_type>(old_size), v.end());
               try {
                   v.shrink_to_fit();
               } catch (const std::exception &) {
                   // Do nothing
               }
               throw;
           }
       },
       py::arg("L"),
       "Extend the list by appending all the items in the given list"
    );

    cl.def(py::pickle(
                 [](const Vector &v) { // __getstate__
                     /* Return a tuple that fully encodes the state of the object */
                    py::list list;
                    for(vtype x : v)
                        list.append(x);
                     return list;

                 }, [](const py::list &t) {

                    auto v = std::unique_ptr<Vector>(new Vector());
                    v->reserve(t.size());

                     for(py::handle h : t)
                        v->push_back(h.cast<vtype>());
                     return v;
                }
             ));


    return cl;
}


}
