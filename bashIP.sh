#!/usr/bin/env bash

function main () {
    if (($# != 4)); then
        echo -e "\e[1;91mParameters: <kernelIP> <memoriaIP> <cpuIP> <filesystemIP>\e[0m"
        exit 1
    fi

    local -r kernelIP=$1
    local -r memoriaIP=$2
    local -r cpuIP=$3
    local -r filesystemIP=$4

    # Cambiar IP de memoria y cpu en kernel
    perl -pi -e "s/(?<=IP_MEMORIA=).*/${memoriaIP}/g" Kernel/*
    perl -pi -e "s/(?<=IP_FILESYSTEM=).*/${filesystemIP}/g" Kernel/*
    perl -pi -e "s/(?<=IP_CPU=).*/${cpuIP}/g" Kernel/*


    # Cambiar IP en fileSystem
    perl -pi -e "s/(?<=IP_MEMORIA=).*/${memoriaIP}/g" File_System/*
    
    # Cambiar IP en memoria
    perl -pi -e "s/(?<=IP_FILESYSTEM=).*/${filesystemIP}/g" Memoria/*    

    
}

main "$@"