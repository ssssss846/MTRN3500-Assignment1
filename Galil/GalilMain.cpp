#include "Galil.h"
#include <iostream>



int main(void)
{
    EmbeddedFunctions funcs;

    Galil myGalil(
        &funcs,
        "192.168.0.120 -d"
    );
}