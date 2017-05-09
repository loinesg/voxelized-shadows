# Voxelized Shadows
This is a research project investigating Voxelized Shadows for compressing precomputed shadow map textures for large outdoor scenes. It allows very high resolution shadow maps to be stored with high compression rates, while retaining the ability to sample filtered shadows in real time.

This is based on the work by Sintorn et al in "Compact Precomputed Voxelized Shadows" (ACM Transactions on Graphics 2014).

![screenshot of the application](Readme/screenshot-top.jpg?raw=true)

## Development

This project was created using C++ with the OpenGL and Qt libraries. It runs on Mac and Linux.

## Implemented Features

- Cascaded Shadow Mapping with 1, 2 or 4 cascades
- Static Shadow Maps compressed using Voxelised Shadows
- "Combined" shadowing mode mixing static and dynamic shadows
- Extensive configuration of the above techniques from the user interface

## How To Run

## Camera Controls

- Click and drag to rotate the camera
- Hold w, a, s and d to move the camera forwards, backwards, left and right
- Hold q and e to move the camera up and down
- Hold shift to move faster
