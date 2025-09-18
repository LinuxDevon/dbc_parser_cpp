# Examples

You will find all the examples in the `src/` directory.
Each example shall be prefixed with `example_` and then named
with the use case then end with `.cpp`.

There are tests to build and validate that all examples are working.
You can validate output as well. You need to enclose the expected output
with `=== +++ ===`. The python script will parse out the text between
and compare the stdout to the text. You are not required to have test
output. 

## Build and Run Examples

They are turned off by default and enabled via the `DBC_BUILD_EXAMPLES`
option. 

A quick getting started is:

```shell
cmake -DDBC_BUILD_EXAMPLES=ON -Bbuild -H.
cmake --build build --target read_simple_dbc
./build/examples/read_simple_dbc
```

## Testing Examples

There is a cmake target to build the examples and run the tests. 

You can do so with:

```shell
cmake -DDBC_BUILD_EXAMPLES=ON -Bbuild -H.
cmake --build build --target test_examples
```