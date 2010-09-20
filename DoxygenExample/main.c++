/**
 * @file main.c++
 * This file includes the main function of the project.
 * @author
 * Jeremy Guillory
 *
 * @date
 * Created on September 20, 2010, 10:06 AM
 */

/**
 * @mainpage
 *
 * @section intro Introduction
 * C.A.P.E. Command and Data Handling Senior Design Project
 *
 * This document details the system software for the CDH subsystem of CAPE2.
 *
 * @section format Format
 * CDH system software adheres to 
 * <A HREF="http://geosoft.no/development/cppstyle.html">Geotechnical Software
 * Services' C++ Coding Standards</A>.
 */

#include <stdlib.h>
#include <stdio.h>

#include "TestClass.h"

/**
 * The main function.
 *
 * A detailed description about what main() does goes here.
 */
int main() {

    TestClass testClass;
    testClass.setNumberItems(10);
    int i = testClass.getNumberItems();
    printf("%i\n", i);
    return (EXIT_SUCCESS);
}

