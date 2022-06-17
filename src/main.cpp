#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <memory>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

int add(int i, int j) { return i + j; }

class Animal {
   public:
    virtual ~Animal() {}
    virtual std::string go(int n_times) = 0;
};
using Animal_ptr = std::shared_ptr<Animal>;
using Animal_vec = std::vector<Animal_ptr>;

std::string call_go(Animal *animal) { return animal->go(3); }

class Dog : public Animal {
   public:
    std::string go(int n_times) override {
        std::string result;
        for (int i = 0; i < n_times; ++i) result += "woof! ";
        return result;
    }
};

Animal_vec dog_list() {
    Animal_vec dogs;
    dogs.push_back(std::make_shared<Dog>());
    dogs.push_back(std::make_shared<Dog>());
    dogs.push_back(std::make_shared<Dog>());
    return dogs;
}

using string_vec = std::vector<std::string>;

string_vec call_go_list(Animal_vec const &animals) {
    string_vec calls;

    for (const auto &p : animals) {
        calls.push_back(p->go(1));
    }

    return calls;
}

class PyAnimal : public Animal {
   public:
    /* Inherit the constructors */
    using Animal::Animal;

    /* Trampoline (need one for each virtual function) */
    std::string go(int n_times) override {
        PYBIND11_OVERRIDE_PURE(
            std::string, /* Return type */
            Animal,      /* Parent class */
            go,          /* Name of function in C++ (must match Python name) */
            n_times      /* Argument(s) */
        );
    }
};

class Pound {
   public:
    Animal_vec animals_;
    explicit Pound(Animal_vec animals) : animals_(std::move(animals)) {}
    string_vec go() { return call_go_list(animals_); }
};

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.def("add", &add);
    m.def("subtract", [](int i, int j) { return i - j; });
    py::class_<Animal, PyAnimal, std::shared_ptr<Animal>>(m, "Animal")
        .def(py::init<>())
        .def("go", &Animal::go)
        //
        ;

    py::class_<Dog, Animal, std::shared_ptr<Dog>>(m, "Dog")
        //
        .def(py::init<>())
        //
        ;

    py::class_<Pound>(m, "Pound")
        //
        .def(py::init<Animal_vec>())
        .def("go", &Pound::go)
        //
        ;

    m.def("call_go", &call_go);
    m.def("dog_list", &dog_list);
    m.def("call_go_list", &call_go_list);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
