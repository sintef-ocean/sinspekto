#pragma once

namespace sinspekto
{
  /**
     @brief Loads all sinspekto qml types.

     This function registers sinspekto's QML-enabled elements so that they are loadable in
     QML scripts. The name mapping from class name to qml element is one-to-one, so for
     instance `DdsBitSubscriber` is named `DdsBitSubscriber` in QML. In your `qml` file,
     you need to add `import fkin.Dds 1.0` to import the sinspekto QML elements, <a
     href="../rst/intro.html#minimal-qml-example">see the minimal example</a>.

  */
  void LoadSinspektoQmlTypes();
}
