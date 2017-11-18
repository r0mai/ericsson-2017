@0xf0897c6777e66a25;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("protocol");

enum Direction {
    up @0;
    left @1;
    down @2;
    right @3;
}

struct Position {
    row @0 : Int32;
    col @1 : Int32;
}

