BROWSER-ONLY BUILD INSTRUCTIONS

1. Create a private GitHub repository (github.com/new). Do not add a README when creating it.
2. Unzip this package on your computer.
3. In the new repo, choose 'uploading an existing file'. Drag the CONTENTS of the unzipped folder, including .github, into the upload page (you may need to upload the nested workflow file separately using GitHub's Create new file and the path .github/workflows/build.yml if your browser omits hidden folders).
4. Commit the files to the main branch. Open Actions > Build portable Windows OpenGL app > Run workflow. A push to main may also start it automatically.
5. After the run succeeds, click its 'PortableOpenGL-Win32' artifact and extract the downloaded ZIP.
6. From your original assignment ZIP, copy the sand.bmp, rock.bmp, seabed_heightmap.ppm and caust_001.bmp ... caust_016.bmp from the AllWeeks directory into the folder alongside the downloaded EXE and freeglut.dll.
7. Double-click the EXE. The restricted computer must allow running downloaded applications and OpenGL.

Your original project is untouched. This is an UNTESTED cloud-build recipe; check the Actions log if compilation fails. Building in GitHub Actions does not install software on your machine. The repository is private because this is university assessment source code.
