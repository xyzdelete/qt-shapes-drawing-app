In order to configure and compile you have to install:
1. The CMake 3.21 or newer
2. The latest C++ MSVC Build tools for x64 from (Microsoft Visual Studio Community 2019, 2022, 2026 or newer)
3. The latest QtCreator or Microsoft Visual Studio IDE
4. The Qt 6.x.x

How to compile by using C++ MSVC Build tools for x64 from Microsoft Visual Studio Community (2019, 2022, 2026 or newer)

For example for Microsoft Visual Studio 2022 compatibility you have to install either Microsoft Visual Studio 2022 or newer.
And, most important, install C++ MSVC Build tools for x64 (v143 or newer) and do steps below:

1. Launch your Visual Studio Developer Command Prompt inside the source code directory
2. In order to configure and generate the Microsoft Visual Studio solution (.slnx) file run the command in your terminal.
I used CMake Microsoft Visual Studio 2026 generator and C++ 2022 MSVC build tools (v143).
You may want to use "Visual Studio 17 2022" generator and different MSVC build tools version.

  2.1 In case you want to use Microsoft Visual Studio 2022 run the command in your terminal

    cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S . -B build

  2.2 In case you want to use Microsoft Visual Studio 2026 run the command in your terminal

    cmake -G "Visual Studio 18 2026" -A x64 -T v143 -S . -B build
  
  2.3 In case it cannot find Qt dependencies you have to add one more parameter in the end to the commands above.
  Or you can open and modify CMakeLists.txt, search for this string "C:/Qt/6.*" and modify it or add another one.
  If you do it then you can skip the parameter below as it will search there first when you use commands above.
  
    -DCMAKE_PREFIX_PATH="<YourPathToQtDir>\lib\cmake"

3. In order to build / compile via Microsoft Visual Studio IDE open the generated build/qt-shapes-drawing-app.slnx file
  
  3.1 In order to build / compile via terminal run the command in your terminal

    cmake --build build --config Release

How to compile by using QtCreator:
1. Run the QtCreator
2. Open the project's CMakeLists.txt
3. Select the Qt 6.x.x kit (Release / Debug) and hit the configure button
4. Press the build button

In order to run .exe on Windows you have to install:
1. The latest Microsoft Visual C++ Redistributable package

<img width="1604" height="1260" alt="Untitled" src="https://github.com/user-attachments/assets/24908fb8-0733-42cd-97f1-2ff6536d9219" />

How to use the app and its features:
1. In order to draw / create figures (Squares, Rectangles, Triangles, Circles) you have to press the corresponding icon first at the top right menu bar. Then use your left mouse button's click and drag it in order to draw it.
2. In order to select multiple things you can press on the "Cross" icon and start selecting by holding you left mouse button's click, the selection rectangle should appear, after that, you may also hold you "cntrl" button and click on figures to add more figures to the current selection.
3. In order to delete items you can either select it first as described in the step "2" and just press "d" on your keyboard or click "File" at the top left corner, then select "New" and it should clear all figures.
4. When you selected figures, you can press and hold your left mouse button and drag it to move figures the way you want.
5. When you selected figures, you can press and hold your right mouse button and drag it to rotate figures the way you want.
6. When you selected figures, you can press and hold your middle mouse button and drag it to make copies of the selected figures.
7. You can also select your drawing pen's width, pen's color, fill color, and click "Fill shape" checkbox in order to to make your next created figures filled with the color you have chosen.
8. If you press "File" at the top left corner, you will be able to save, save as, create new, exit the app. If you exit the app, your current drawings will be saved in the same directory as the app, in the qt-shapes-drawing-app.png file. You can open the app and it should autoload it. Or you can manually load it by using the "File" menu.

