cmake_minimum_required(VERSION 3.20)

project(ThermalPrinterDriver)

add_executable(tp_interface.elf
                main.cpp
                logger.cpp
                tp_device.cpp
                image.cpp)
