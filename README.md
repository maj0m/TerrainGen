# (08.16) Update: Erosion on the GPU!
- Changed the way erosion is performed: instead of simulating particles on the cpu, it is now done in parallel on the GPU using a compute shader making it blazingly fast (real time!!!).
Before / After erosion:
![gpuerosion_beforeafter](https://github.com/maj0m/TerrainGen/assets/112952510/33703fd2-e1f3-4505-81f9-85330ede3cb0)


# (08.04) Update:  GLFW Integration, GUI Support with ImGui, Hydraulic Erosion, and Height-based Coloring Refactor Ahead!

- Added hydraulic erosion (Particle simulation, implementation is based on
https://nickmcd.me/2020/04/10/simple-particle-based-hydraulic-erosion/)
- Switched graphics libraries and reordered the file structure to be more intuitive
- Various optimizations and improvements for better maintainability
- Removed height-based coloring to give space for a more sophisticated biome system
- Added a GUI
  
Erosion Before / After (Open for high-res version):
![erosion_beforeafter](https://github.com/maj0m/Terrain-Generator/assets/112952510/10d2f3d8-a628-4556-a56a-e04bffab9d9e)


# (07.24) Update: Various shader improvements & dynamic foam where land meets water
I temporarily disabled fog and atmosphere in this update to rework them later.
Coming up: Procedural clouds with gpu instancing
![foam1](https://github.com/maj0m/Terrain-Generator/assets/112952510/e8f155e5-975a-4016-8141-eabef38e6b95)
![foam2](https://github.com/maj0m/Terrain-Generator/assets/112952510/eb4fbc08-e995-4b7a-ab6f-e872717885f2)

# LowPolyTerrain
Procedural terrain generation in C++ using Simplex Noise with mesh generation on the CPU and rendering on the GPU
![terraingenerator](https://github.com/maj0m/Terrain-Generator/assets/112952510/911f3696-857d-4600-949a-3701aa386b61)

