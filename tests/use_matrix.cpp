#include "log.h"

#include <iostream>
#include <arr2d/matrix.h>
#include <vector>

using namespace arr2d;

template <std::size_t m, std::size_t n>
using matrix_int = matrix_size_t<int, m, n>;

template <std::size_t m, std::size_t n>
using matrix_double = matrix_size_t<double, m, n>;

int main()
{
    using std::cout;
    using std::endl;

    int arr[2][3] = {{1, 2, 3}, {4, 5, 6}};
    matrix_int<2, 3> mt = arr;
    matrix_int<2, 3> mt2 = mt * 2;

    log("Matrix displaying:");
    log(mt);
    log(mt2);

    log("Matrix transposing:");
    log("transpose(mt)");
    cout << transpose(mt) << endl;

    log("Matrix binary + operation:");
    log("+mt");
    cout << +mt << endl;

    log("Matrix binary - operation:");
    log("-mt");
    cout << -mt << endl;

    log("Matrix adding:");
    log("mt + mt2");
    log(mt + mt2);
    // log("mt - mt2");
    // log(mt - mt2);

    log("Matrix scaling:");
    log("2 * mt");
    log(2 * mt);

    log("Matrix multiplication:");
    log("mt * transpose(mt)");
    cout << mt * transpose(mt) << endl;
    log("transpose(mt) * mt");
    cout << transpose(mt) * mt << endl;

    //  x -  y =  4
    // 2x + 3y = -6

    int linear_equation[][3] = {{1, -1, 4}, {2, 3, -6}};

    // int linear_equation[][4] = {{3, -1, 2, 4}, {-5, 3, 4, 0}, {8, -5, 1, -10}, {2, 3, -6, -4}};

    matrix_double<2, 3> mt3 = linear_equation;
    log("mt3:");
    log(mt3);

    log("mt3.make_ref():");
    mt3.make_ref();
    log(mt3);

    log("mt3.make_rref():");
    mt3.make_rref();
    log(mt3);

    return 0;
}
