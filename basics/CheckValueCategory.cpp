#include <iostream>
#include <type_traits>

#define PrintValueCategory(expr)                                           \
do {                                                                       \
    if constexpr (std::is_lvalue_reference_v<decltype((expr))>) {          \
        std::cout << "expression is lvalue\n";                             \
    } else if constexpr (std::is_rvalue_reference_v<decltype((expr))>) {   \
        std::cout << "expression is xvalue\n";                             \
    } else {                                                               \
        std::cout << "expression is pvalue\n";                             \
    }                                                                      \
} while (false)

int main (int argc, char *argv[])
{
    int a;
    PrintValueCategory(a);

    PrintValueCategory(2);

    PrintValueCategory(std::move(a));

    return 0;
}