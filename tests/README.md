# Chunk streaming checks

Configure with `-DBUILD_TESTING=ON`, then build `WorldMaker`,
`ChunkGenerationTests`, and `ChunkStreamingGLTests`.

Select both C and C++ compilers from the same toolchain in the IDE or CMake
configuration. Use separate build and FetchContent binary directories for
different toolchains; cached dependency sources may be shared. After switching
compilers, reset the CMake cache and rebuild all targets and dependencies.

Run the CPU regression suite with:

```sh
ctest --test-dir <build-directory> --output-on-failure -R ChunkGenerationConcurrency
```

It runs without an OpenGL context and checks parallel/serial equivalence,
all LODs, duplicate requests, configuration snapshots, cancellation,
regeneration, and shutdown while workers are busy.
It also verifies the updated biome modifier chains, multi-biome color/height
blending, river carving and bank transitions, and feature placement above water.
UI modifier edits are isolated from running jobs and applied to regenerated meshes;
incomplete feature configurations are skipped while preserving feature priority.

Run `ChunkStreamingGLTests` from the build directory on a machine with OpenGL
4.4. It uses a hidden window to verify GPU buffer contents, the two-upload
frame limit, stationary-camera completion, unloading, LOD changes, render
distance changes, seed/resolution regeneration, batched model transforms,
and reuse of cached models after unloading and revisiting a chunk.
It also checks that moving the camera retains the active biome configuration
until Generate mesh applies the edited modifiers, even without a seed change.

The streamer uses up to four persistent workers, leaving a CPU core free when
possible. It publishes at most two meshes per frame, stopping between uploads
after a 2 ms budget. Individual OpenGL calls and first-time model loading still
run on the main thread and can exceed that budget; this is not a hard frame-time
guarantee. Completed CPU meshes are bounded to eight plus in-flight workers.
