from pybind11_debugging import _core as m


def test_version():
    assert m.__version__ == "0.0.1"


def test_add():
    assert m.add(1, 2) == 3


def test_sub():
    assert m.subtract(1, 2) == -1


class Cat(m.Animal):
    def go(self, n_times):
        return "meow! " * n_times


def test_dog():
    c = Cat()
    assert m.call_go(c) == u'meow! meow! meow! '


def test_dog_list():
    dl = m.dog_list()
    assert m.call_go_list(dl) == ["woof! "] * 3

    dl.append(Cat())
    assert m.call_go_list(dl) == ["woof! "] * 3 + ["meow! "]
    print(m.call_go_list(dl))


def test_cat_list():
    cats = [Cat() for _ in range(3)]
    assert m.call_go_list(cats) == ["meow! "] * 3


def test_pound():
    # This works, but is flawed, as you can see in the next test.
    cats = [Cat() for _ in range(3)]
    p = m.Pound(cats)

    assert p.go() == ["meow! "] * 3


def test_pound_with_del_cats():
    cats = [Cat() for _ in range(3)]
    p = m.Pound(cats)

    # This appears to kill off *all* the Cats. Just leaving the PyAnimal class
    # which can no longer locate the python objects.
    del cats

    # Virtual Call!
    assert p.go() == ["meow! "] * 3


def test_pound_with_del_dogs():
    dogs = [m.Dog() for _ in range(3)]
    p = m.Pound(dogs)

    del dogs

    # This works okay.
    assert p.go() == ["woof! "] * 3
