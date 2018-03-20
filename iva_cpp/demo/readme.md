Compiling demo
===

Note:
----
careful configure the include directory and library path in `CMakeLists` before compiling.<br>
careful configure the configuration document  `iva.prototxt`, which is required.<br>
if want to use the audio data offline, careful configure the document `data.prototxt`.<br>

Use IVA libarary with flexible sources number
----
 class name is `iva::iva`.


Use IVA libarary with fixed sources number
----
default sources number is 2.<br>
iva library with fixed sources number is faster than the flexible sources number one.<br>
careful configure the audio sources number in demo CMakeLists and library CMakeLists(if your sources number is 2, ignore this ).<br>
class  name is  `iva::iva_optimized`.


