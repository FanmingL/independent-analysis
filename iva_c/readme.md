

## Usage

1. Make input data. We will use the file `data.prototxt` to generate the input data for iva_c.
```bash
cat data.prototxt | cut -c 7- > data_c.prototxt
```
2. Change the data path in `main.c`,
```c
fp=fopen("/full/path/to/data_c.prototxt", "r");
fp2=fopen("/full/path/to/out_c.prototxt","w");
```
Here, `"/full/path/to/data_c.prototxt"` is the source data, `"/full/path/to/out_c.prototxt"` is the output path.
3. Make
```bash
make -j 
```
Or
```bash
mkdir build
cd build
cmake ..
make -j
```
4. Run
```bash
./iva 
```
