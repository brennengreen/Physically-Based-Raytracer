# Physically Based Raytracer

My implementation of a physically based raytracer which utilizes a simple monte carlo approach to importance sampling using BxDFs.

This raytracer has support for the following:
- Lambertians
- Dielectrics
- Metals
- Emitters (Area and Point Lights, emissive materials)
- Texture Mapping
- Procedural Textures
- Instancing
- Constant Density Volumetrics
- Motion Blur
- Depth of Field

To improve performance of the raytracer, I utilized a Bounding Volume Hierarchy (BVH) to improve intersection testing efficiency.

Dependencies:
- stb: https://github.com/nothings/stb


## Examples
Ye olde Cornell Box rendered with a couple of diffuse cubes

<img style="width:350px;" src="https://user-images.githubusercontent.com/32919777/147679278-a8bc2612-e2f7-4091-bfbe-5a0fe5cef5b1.jpg"></img>

My implementation of the typical approach to rendering the moon, using the Oren–Nayar reflectance model and some textures from NASSA

<img style="width:350px;" src="https://user-images.githubusercontent.com/32919777/147679305-2dcf66b4-6de3-445b-b151-8c2ea907877c.jpg"></img>

A marble sitting atop a floor textured with perlin noise, surrounded by volumetric fog

<img style="width:350px;" src="https://user-images.githubusercontent.com/32919777/147679332-c0008809-2c55-4652-8da5-b7ea8105141a.jpg"></img>
