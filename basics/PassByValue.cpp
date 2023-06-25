#include <iostream>

class A {
public:
    A () {
        std::cout << "construct A" << std::endl;
    }

    ~A () {
        std::cout << "deconstruct A" << std::endl;
    }

    A (A const& oth) {
        std::cout << "copy construct A" << std::endl;
    }

    A& operator= (A const& oth) {
        std::cout << "copy assign A" << std::endl;
        return *this;
    }

    A (A&& oth) {
        std::cout << "move construct A" << std::endl;
    }

    A& operator= (A&& oth) {
        std::cout << "move assign A" << std::endl;
        return *this;
    }
};

A MakeA () {
    return A();
}

/*
 * passing arguments by value
 */
template <typename T>
void f (T arg) {}

int main (int argc, char *argv[])
{
    std::cout << "Start testing variable(lvalue) ..." << std::endl;
    {
        A a;
        f(a);
    }
    std::cout << "End testing variable(lvalue)" << std::endl << std::endl;

    std::cout << "Start testing std::move(xvalue) ..." << std::endl;
    {
        A a;
        f(std::move(a));
    }
    std::cout << "End testing std::move(xvalue)" << std::endl << std::endl;

    /*
     * no copy-construct when passing the pvalue argument
     */
    std::cout << "Start testing temporary-object(pvalue) ..." << std::endl;
    {
        f(A());
    }
    std::cout << "End testing temporary-object(pvalue)" << std::endl << std::endl;

    std::cout << "Start testing returned-value-by-function(pvalue) ..." << std::endl;
    {
        f(MakeA());
    }
    std::cout << "End testing returned-value-by-function(pvalue)" << std::endl << std::endl;

    return 0;
}
 