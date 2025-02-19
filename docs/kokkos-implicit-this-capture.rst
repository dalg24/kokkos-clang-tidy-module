.. title:: clang-tidy - kokkos-implicit-this-capture

kokkos-implicit-this-capture
============================

The implicit-this-capture check checks if a lambda passed to any of
parallel_reduce, parallel_for, or parallel_scan functions captures the this
pointer.

Example of a class that should trigger the check
.. code-block:: c++
  
  class Foo {
    int x = 0; 

    void operator()(){
      Kokkos::parallel_for(10, KOKKOS_LAMBDA(int y){ printf("%d", x+y)});
    }
  };
